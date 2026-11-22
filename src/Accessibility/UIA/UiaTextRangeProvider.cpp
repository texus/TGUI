////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2026 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AccessibilityBackendUIA.hpp"
#include "UiaProviders.hpp"
#include <TGUI/TextBoundaries.hpp>

#include <algorithm>
#include <cmath>
#include <mutex>
#include <unordered_set>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    // All text ranges that currently exist, used to recognize our own ranges when UI Automation passes them back to us.
    // Ranges are created and released on the threads of UI Automation, so a mutex is needed.
    std::unordered_set<const ITextRangeProvider*> liveTextRanges;
    std::mutex liveTextRangesMutex;

    // Returns the positions where each unit of text starts, sorted from the start of the text to the end
    [[nodiscard]] std::vector<std::size_t> findUnitStarts(const tgui::priv::UiaElementSnapshot& text, TextUnit unit)
    {
        const tgui::String& str = text.getText();
        std::vector<std::size_t> starts;
        switch (unit)
        {
            case TextUnit_Character:
                starts = tgui::priv::findCharacterBoundaries(str);
                break;
            case TextUnit_Word:
                // The same words as when moving the caret by words in the text widgets
                starts = tgui::priv::findWordBoundaries(str);
                break;
            case TextUnit_Line:
            {
                for (const auto& line : text.getTextLines())
                    starts.push_back(line.first);
                break;
            }
            case TextUnit_Paragraph:
            {
                starts.push_back(0);
                for (std::size_t i = 0; i < str.length(); ++i)
                {
                    if ((str[i] == U'\n') && (i + 1 < str.length()))
                        starts.push_back(i + 1);
                }
                break;
            }
            default: // Format, Page and Document all cover the entire text, as the text has the same format everywhere
                starts.push_back(0);
                break;
        }

        if (starts.empty() || (starts.front() != 0))
            starts.insert(starts.begin(), 0);

        return starts;
    }

    // Returns the bounds of a character in screen coordinates, as they were stored in the snapshot
    [[nodiscard]] tgui::FloatRect getCharacterBounds(const tgui::priv::UiaElementSnapshot& text, std::size_t index)
    {
        if (!text.characterBounds || text.characterBounds->empty())
            return {};

        return (*text.characterBounds)[std::min(index, text.characterBounds->size() - 1)];
    }

    // Converts a rectangle that TGUI uses into a rectangle that UI Automation uses
    [[nodiscard]] UiaRect toUiaRect(const tgui::FloatRect& rect)
    {
        return {static_cast<double>(rect.left), static_cast<double>(rect.top), static_cast<double>(rect.width), static_cast<double>(rect.height)};
    }

    // Fills the variant with the special value with which UI Automation reports that an attribute isn't supported
    [[nodiscard]] HRESULT getNotSupportedValue(VARIANT& value)
    {
        IUnknown* notSupported = nullptr;
        const HRESULT result = UiaGetReservedNotSupportedValue(&notSupported);
        if (FAILED(result))
            return result;

        value.vt = VT_UNKNOWN;
        value.punkVal = notSupported;
        return S_OK;
    }

    // Converts a color to the COLORREF format that UI Automation expects
    [[nodiscard]] int toColorRef(tgui::Color color)
    {
        return static_cast<int>(RGB(color.getRed(), color.getGreen(), color.getBlue()));
    }

    // Returns whether two variants contain the same value, for the types that are used by the text attributes
    [[nodiscard]] bool areVariantsEqual(const VARIANT& left, const VARIANT& right)
    {
        if (left.vt != right.vt)
            return false;

        switch (left.vt)
        {
            case VT_BOOL:
                return left.boolVal == right.boolVal;
            case VT_I4:
                return left.lVal == right.lVal;
            case VT_R8:
                return left.dblVal == right.dblVal;
            case VT_BSTR:
                return (left.bstrVal && right.bstrVal) ? (std::wcscmp(left.bstrVal, right.bstrVal) == 0) : (left.bstrVal == right.bstrVal);
            default:
                return false;
        }
    }
} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        UiaTextProvider::UiaTextProvider(UiaElementProvider* element) :
            m_element{element}
        {
            m_element->AddRef();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        UiaTextProvider::~UiaTextProvider()
        {
            m_element->Release();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextProvider::QueryInterface(REFIID riid, void** ppvObject)
        {
            if (!ppvObject)
                return E_POINTER;

            if ((riid == __uuidof(IUnknown)) || (riid == __uuidof(ITextProvider)))
            {
                *ppvObject = static_cast<ITextProvider*>(this);
                AddRef();
                return S_OK;
            }

            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ULONG UiaTextProvider::AddRef()
        {
            return static_cast<ULONG>(InterlockedIncrement(&m_referenceCount));
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ULONG UiaTextProvider::Release()
        {
            const LONG referenceCount = InterlockedDecrement(&m_referenceCount);
            if (referenceCount == 0)
                delete this;

            return static_cast<ULONG>(referenceCount);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextProvider::GetSelection(SAFEARRAY** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;
            HRESULT result = S_OK;
            const auto text = getText(result);
            if (!text)
                return result;

            // When no text is selected, the selection is an empty range at the position of the caret
            *pRetVal = createRangeArray(text->textSelection.first, text->textSelection.second);
            return *pRetVal ? S_OK : E_OUTOFMEMORY;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextProvider::GetVisibleRanges(SAFEARRAY** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;
            HRESULT result = S_OK;
            const auto text = getText(result);
            if (!text)
                return result;

            // Find the first and last line that lie (partially) inside the visible area
            const FloatRect& visibleArea = text->visibleTextArea;
            std::size_t start = 0;
            std::size_t end = 0;
            bool visibleLineFound = false;
            for (const auto& line : text->getTextLines())
            {
                const FloatRect bounds = getCharacterBounds(*text, line.first);
                const bool visible = (bounds.top + bounds.height > visibleArea.top) && (bounds.top < visibleArea.top + visibleArea.height);
                if (visible && !visibleLineFound)
                {
                    start = line.first;
                    visibleLineFound = true;
                }
                if (visible)
                    end = line.second;
            }

            *pRetVal = createRangeArray(start, end);
            return *pRetVal ? S_OK : E_OUTOFMEMORY;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextProvider::RangeFromChild(IRawElementProviderSimple*, ITextRangeProvider** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            // The text of widgets never contains embedded objects, so no element can be a child of the text
            *pRetVal = nullptr;
            return E_INVALIDARG;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextProvider::RangeFromPoint(UiaPoint point, ITextRangeProvider** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;
            HRESULT result = S_OK;
            const auto text = getText(result);
            if (!text)
                return result;

            // Find the line at the height of the point. Points above or below the text belong to the first or last line.
            const auto& lines = text->getTextLines();
            std::size_t lineIndex = 0;
            for (std::size_t i = 0; i < lines.size(); ++i)
            {
                const FloatRect bounds = getCharacterBounds(*text, lines[i].first);
                if (point.y >= bounds.top)
                    lineIndex = i;
            }

            // Find the character on that line that is closest to the point. Unless this is the last line, the end of the
            // line is the start of the next line, so the position in front of the last character is the furthest we can go.
            std::size_t index = 0;
            if (!lines.empty())
            {
                const auto& line = lines[lineIndex];
                index = ((lineIndex + 1 < lines.size()) && (line.second > line.first)) ? line.second - 1 : line.second;
                for (std::size_t i = line.first; i < line.second; ++i)
                {
                    const FloatRect bounds = getCharacterBounds(*text, i);
                    if (point.x < bounds.left + (bounds.width / 2))
                    {
                        index = i;
                        break;
                    }
                }
            }

            *pRetVal = new UiaTextRangeProvider(m_element, index, index);
            return S_OK;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextProvider::get_DocumentRange(ITextRangeProvider** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;
            HRESULT result = S_OK;
            const auto text = getText(result);
            if (!text)
                return result;

            *pRetVal = new UiaTextRangeProvider(m_element, 0, text->getText().length());
            return S_OK;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextProvider::get_SupportedTextSelection(SupportedTextSelection* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = SupportedTextSelection_Single;
            return S_OK;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<const UiaElementSnapshot> UiaTextProvider::getText(HRESULT& result) const
        {
            auto text = m_element->getSnapshot();
            if (!text || !text->alive || !text->state.hasText)
            {
                result = static_cast<HRESULT>(UIA_E_ELEMENTNOTAVAILABLE);
                return nullptr;
            }

            result = S_OK;
            return text;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SAFEARRAY* UiaTextProvider::createRangeArray(std::size_t start, std::size_t end) const
        {
            SAFEARRAY* ranges = SafeArrayCreateVector(VT_UNKNOWN, 0, 1);
            if (!ranges)
                return nullptr;

            // SafeArrayPutElement adds its own reference, so we release the one that the new object starts with
            auto* range = new UiaTextRangeProvider(m_element, start, end);
            LONG index = 0;
            const HRESULT result = SafeArrayPutElement(ranges, &index, static_cast<IUnknown*>(range));
            range->Release();
            if (FAILED(result))
            {
                SafeArrayDestroy(ranges);
                return nullptr;
            }

            return ranges;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        UiaTextRangeProvider::UiaTextRangeProvider(UiaElementProvider* element, std::size_t start, std::size_t end) :
            m_element{element},
            m_start{std::min(start, end)},
            m_end{std::max(start, end)}
        {
            m_element->AddRef();
            const std::lock_guard<std::mutex> lock{liveTextRangesMutex};
            liveTextRanges.insert(this);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        UiaTextRangeProvider::~UiaTextRangeProvider()
        {
            {
                const std::lock_guard<std::mutex> lock{liveTextRangesMutex};
                liveTextRanges.erase(this);
            }

            m_element->Release();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextRangeProvider::QueryInterface(REFIID riid, void** ppvObject)
        {
            if (!ppvObject)
                return E_POINTER;

            if ((riid == __uuidof(IUnknown)) || (riid == __uuidof(ITextRangeProvider)))
            {
                *ppvObject = static_cast<ITextRangeProvider*>(this);
                AddRef();
                return S_OK;
            }

            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ULONG UiaTextRangeProvider::AddRef()
        {
            return static_cast<ULONG>(InterlockedIncrement(&m_referenceCount));
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ULONG UiaTextRangeProvider::Release()
        {
            const LONG referenceCount = InterlockedDecrement(&m_referenceCount);
            if (referenceCount == 0)
                delete this;

            return static_cast<ULONG>(referenceCount);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextRangeProvider::Clone(ITextRangeProvider** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = new UiaTextRangeProvider(m_element, m_start, m_end);
            return S_OK;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextRangeProvider::Compare(ITextRangeProvider* range, BOOL* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = FALSE;
            if (!range)
                return E_INVALIDARG;

            const UiaTextRangeProvider* other = getSameTextRange(range);
            *pRetVal = (other && (other->m_start == m_start) && (other->m_end == m_end)) ? TRUE : FALSE;
            return S_OK;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextRangeProvider::CompareEndpoints(TextPatternRangeEndpoint endpoint,
                                                       ITextRangeProvider* targetRange,
                                                       TextPatternRangeEndpoint targetEndpoint,
                                                       int* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = 0;
            const UiaTextRangeProvider* other = getSameTextRange(targetRange);
            if (!other)
                return E_INVALIDARG;

            const std::size_t pos = (endpoint == TextPatternRangeEndpoint_Start) ? m_start : m_end;
            const std::size_t targetPos = (targetEndpoint == TextPatternRangeEndpoint_Start) ? other->m_start : other->m_end;
            *pRetVal = (pos < targetPos) ? -1 : ((pos > targetPos) ? 1 : 0);
            return S_OK;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextRangeProvider::ExpandToEnclosingUnit(TextUnit unit)
        {
            HRESULT result = S_OK;
            const auto text = getText(result);
            if (!text)
                return result;

            clampEndpoints(text->getText().length());
            const auto unitRange = findUnit(*text, m_start, unit);
            m_start = unitRange.first;
            m_end = unitRange.second;
            return S_OK;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextRangeProvider::FindAttribute(TEXTATTRIBUTEID attributeId, VARIANT val, BOOL, ITextRangeProvider** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;

            // The attributes are the same for the entire text, so either the whole range matches or nothing does
            VARIANT value;
            VariantInit(&value);
            const HRESULT result = GetAttributeValue(attributeId, &value);
            if (FAILED(result))
                return result;

            if (areVariantsEqual(value, val))
                *pRetVal = new UiaTextRangeProvider(m_element, m_start, m_end);

            VariantClear(&value);
            return S_OK;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextRangeProvider::FindText(BSTR text, BOOL backward, BOOL ignoreCase, ITextRangeProvider** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;
            if (!text)
                return E_INVALIDARG;

            HRESULT result = S_OK;
            const auto snapshot = getText(result);
            if (!snapshot)
                return result;

            clampEndpoints(snapshot->getText().length());
            String haystack = snapshot->getText().substr(m_start, m_end - m_start);
            String needle{text};
            if (needle.empty())
                return E_INVALIDARG;

            if (ignoreCase)
            {
                haystack = haystack.toLower();
                needle = needle.toLower();
            }

            const std::size_t pos = backward ? haystack.rfind(needle) : haystack.find(needle);
            if (pos != String::npos)
                *pRetVal = new UiaTextRangeProvider(m_element, m_start + pos, m_start + pos + needle.length());

            return S_OK;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextRangeProvider::GetAttributeValue(TEXTATTRIBUTEID attributeId, VARIANT* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            pRetVal->vt = VT_EMPTY;
            HRESULT result = S_OK;
            const auto text = getText(result);
            if (!text)
                return result;

            const AccessibleTextAttributes& attributes = text->textAttributes;
            switch (attributeId)
            {
                case UIA_FontNameAttributeId:
                {
                    // An unknown font name is not supported, an empty name would be presented to the user as the font name
                    if (attributes.fontName.empty())
                        return getNotSupportedValue(*pRetVal);

                    const std::wstring fontName = attributes.fontName.toWideString();
                    pRetVal->bstrVal = SysAllocStringLen(fontName.c_str(), static_cast<UINT>(fontName.length()));
                    if (!pRetVal->bstrVal)
                        return E_OUTOFMEMORY;
                    pRetVal->vt = VT_BSTR;
                    break;
                }
                case UIA_FontSizeAttributeId:
                    pRetVal->vt = VT_R8;
                    pRetVal->dblVal = attributes.fontSize;
                    break;
                case UIA_FontWeightAttributeId:
                    pRetVal->vt = VT_I4;
                    pRetVal->lVal = attributes.bold ? FW_BOLD : FW_NORMAL;
                    break;
                case UIA_IsItalicAttributeId:
                    pRetVal->vt = VT_BOOL;
                    pRetVal->boolVal = attributes.italic ? VARIANT_TRUE : VARIANT_FALSE;
                    break;
                case UIA_UnderlineStyleAttributeId:
                    pRetVal->vt = VT_I4;
                    pRetVal->lVal = attributes.underlined ? TextDecorationLineStyle_Single : TextDecorationLineStyle_None;
                    break;
                case UIA_StrikethroughStyleAttributeId:
                    pRetVal->vt = VT_I4;
                    pRetVal->lVal = attributes.strikeThrough ? TextDecorationLineStyle_Single : TextDecorationLineStyle_None;
                    break;
                case UIA_ForegroundColorAttributeId:
                    pRetVal->vt = VT_I4;
                    pRetVal->lVal = toColorRef(attributes.foregroundColor);
                    break;
                case UIA_BackgroundColorAttributeId:
                    pRetVal->vt = VT_I4;
                    pRetVal->lVal = toColorRef(attributes.backgroundColor);
                    break;
                case UIA_IsReadOnlyAttributeId:
                    pRetVal->vt = VT_BOOL;
                    pRetVal->boolVal = text->state.readOnly ? VARIANT_TRUE : VARIANT_FALSE;
                    break;
                case UIA_IsHiddenAttributeId:
                case UIA_IsSubscriptAttributeId:
                case UIA_IsSuperscriptAttributeId:
                    pRetVal->vt = VT_BOOL;
                    pRetVal->boolVal = VARIANT_FALSE;
                    break;
                default: // Attributes that we don't know about have to be reported with a special value
                    return getNotSupportedValue(*pRetVal);
            }

            return S_OK;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextRangeProvider::GetBoundingRectangles(SAFEARRAY** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;
            HRESULT result = S_OK;
            const auto text = getText(result);
            if (!text)
                return result;

            clampEndpoints(text->getText().length());

            // Each line of the range gets its own rectangle, but only the part inside the visible area is included
            const FloatRect& visibleArea = text->visibleTextArea;
            std::vector<UiaRect> rects;
            for (const auto& line : text->getTextLines())
            {
                const std::size_t first = std::max(m_start, line.first);
                const std::size_t last = std::min(m_end, line.second);
                if ((first > last) || ((first == last) && (m_start != m_end)))
                    continue;

                // A degenerate range is represented by a rectangle without width at its position
                const FloatRect firstBounds = getCharacterBounds(*text, first);
                const FloatRect lastBounds = (last > first) ? getCharacterBounds(*text, last - 1) : firstBounds;
                const float left = firstBounds.left;
                const float right = (last > first) ? (lastBounds.left + lastBounds.width) : firstBounds.left;
                const FloatRect lineBounds{left, firstBounds.top, std::max(0.f, right - left), firstBounds.height};

                const float clippedLeft = std::max(lineBounds.left, visibleArea.left);
                const float clippedTop = std::max(lineBounds.top, visibleArea.top);
                const float clippedRight = std::min(lineBounds.left + lineBounds.width, visibleArea.left + visibleArea.width);
                const float clippedBottom = std::min(lineBounds.top + lineBounds.height, visibleArea.top + visibleArea.height);
                if ((clippedRight < clippedLeft) || (clippedBottom <= clippedTop))
                    continue;

                rects.push_back(toUiaRect({clippedLeft, clippedTop, clippedRight - clippedLeft, clippedBottom - clippedTop}));

                if (m_start == m_end)
                    break;
            }

            SAFEARRAY* rectArray = SafeArrayCreateVector(VT_R8, 0, static_cast<ULONG>(rects.size() * 4));
            if (!rectArray)
                return E_OUTOFMEMORY;

            for (std::size_t i = 0; i < rects.size(); ++i)
            {
                double values[] = {rects[i].left, rects[i].top, rects[i].width, rects[i].height};
                for (LONG j = 0; j < 4; ++j)
                {
                    LONG index = static_cast<LONG>(i * 4) + j;
                    const HRESULT putResult = SafeArrayPutElement(rectArray, &index, &values[j]);
                    if (FAILED(putResult))
                    {
                        SafeArrayDestroy(rectArray);
                        return putResult;
                    }
                }
            }

            *pRetVal = rectArray;
            return S_OK;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextRangeProvider::GetEnclosingElement(IRawElementProviderSimple** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;

            HRESULT result = S_OK;
            if (!getText(result))
                return result;

            // The text never contains child elements, so the enclosing element is always the element that owns the text
            *pRetVal = static_cast<IRawElementProviderSimple*>(m_element);
            m_element->AddRef();
            return S_OK;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextRangeProvider::GetText(int maxLength, BSTR* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;
            if (maxLength < -1)
                return E_INVALIDARG;

            HRESULT result = S_OK;
            const auto text = getText(result);
            if (!text)
                return result;

            const String& fullText = text->getText();
            clampEndpoints(fullText.length());

            std::wstring wideText = fullText.substr(m_start, m_end - m_start).toWideString();
            if ((maxLength >= 0) && (wideText.length() > static_cast<std::size_t>(maxLength)))
                wideText.resize(static_cast<std::size_t>(maxLength));

            *pRetVal = SysAllocStringLen(wideText.c_str(), static_cast<UINT>(wideText.length()));
            return *pRetVal ? S_OK : E_OUTOFMEMORY;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextRangeProvider::Move(TextUnit unit, int count, int* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = 0;
            HRESULT result = S_OK;
            const auto text = getText(result);
            if (!text)
                return result;

            clampEndpoints(text->getText().length());
            if (count == 0)
                return S_OK;

            // The range is collapsed, moved and then expanded again to the unit if it wasn't empty
            const bool degenerate = (m_start == m_end);
            std::size_t pos = m_start;
            *pRetVal = movePosition(*text, pos, unit, count);

            m_start = pos;
            m_end = degenerate ? pos : findUnit(*text, pos, unit).second;
            return S_OK;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextRangeProvider::MoveEndpointByUnit(TextPatternRangeEndpoint endpoint, TextUnit unit, int count, int* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = 0;
            HRESULT result = S_OK;
            const auto text = getText(result);
            if (!text)
                return result;

            clampEndpoints(text->getText().length());

            // When one endpoint moves past the other, the other endpoint moves along with it
            std::size_t& pos = (endpoint == TextPatternRangeEndpoint_Start) ? m_start : m_end;
            *pRetVal = movePosition(*text, pos, unit, count);
            if (m_start > m_end)
            {
                if (endpoint == TextPatternRangeEndpoint_Start)
                    m_end = m_start;
                else
                    m_start = m_end;
            }

            return S_OK;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextRangeProvider::MoveEndpointByRange(TextPatternRangeEndpoint endpoint,
                                                          ITextRangeProvider* targetRange,
                                                          TextPatternRangeEndpoint targetEndpoint)
        {
            const UiaTextRangeProvider* other = getSameTextRange(targetRange);
            if (!other)
                return E_INVALIDARG;

            const std::size_t targetPos = (targetEndpoint == TextPatternRangeEndpoint_Start) ? other->m_start : other->m_end;
            if (endpoint == TextPatternRangeEndpoint_Start)
            {
                m_start = targetPos;
                if (m_start > m_end)
                    m_end = m_start;
            }
            else
            {
                m_end = targetPos;
                if (m_end < m_start)
                    m_start = m_end;
            }

            return S_OK;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextRangeProvider::Select()
        {
            HRESULT result = S_OK;
            const auto text = getText(result);
            if (!text)
                return result;

            // The selection is changed on the gui thread, the change is reported with a text selection event
            clampEndpoints(text->getText().length());
            m_element->queueAction([start = m_start, end = m_end](AccessibleObject& object) { object.setTextSelection(start, end); });
            return S_OK;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextRangeProvider::AddToSelection()
        {
            // Widgets only support a single selection
            return static_cast<HRESULT>(UIA_E_INVALIDOPERATION);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextRangeProvider::RemoveFromSelection()
        {
            return static_cast<HRESULT>(UIA_E_INVALIDOPERATION);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextRangeProvider::ScrollIntoView(BOOL alignToTop)
        {
            HRESULT result = S_OK;
            const auto text = getText(result);
            if (!text)
                return result;

            clampEndpoints(text->getText().length());
            m_element->queueAction([index = alignToTop ? m_start : m_end](AccessibleObject& object) { object.scrollTextToIndex(index); });
            return S_OK;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaTextRangeProvider::GetChildren(SAFEARRAY** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            // The text never contains embedded objects
            *pRetVal = SafeArrayCreateVector(VT_UNKNOWN, 0, 0);
            return *pRetVal ? S_OK : E_OUTOFMEMORY;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<const UiaElementSnapshot> UiaTextRangeProvider::getText(HRESULT& result) const
        {
            auto text = m_element->getSnapshot();
            if (!text || !text->alive || !text->state.hasText)
            {
                result = static_cast<HRESULT>(UIA_E_ELEMENTNOTAVAILABLE);
                return nullptr;
            }

            result = S_OK;
            return text;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        const UiaTextRangeProvider* UiaTextRangeProvider::getSameTextRange(ITextRangeProvider* range) const
        {
            if (!range)
                return nullptr;

            // Ranges are only comparable when they are our own objects and belong to the same element
            const std::lock_guard<std::mutex> lock{liveTextRangesMutex};
            if (liveTextRanges.find(range) == liveTextRanges.end())
                return nullptr;

            const auto* other = static_cast<const UiaTextRangeProvider*>(range);
            if (other->m_element != m_element)
                return nullptr;

            return other;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::pair<std::size_t, std::size_t> UiaTextRangeProvider::findUnit(const UiaElementSnapshot& text, std::size_t pos, TextUnit unit)
        {
            const std::size_t textLength = text.getText().length();
            if ((unit == TextUnit_Format) || (unit == TextUnit_Page) || (unit == TextUnit_Document))
                return {0, textLength};

            // At the end of the text there is no character or word to expand to
            if ((pos >= textLength) && ((unit == TextUnit_Character) || (unit == TextUnit_Word)))
                return {textLength, textLength};

            const auto starts = findUnitStarts(text, unit);
            auto it = std::upper_bound(starts.begin(), starts.end(), pos);
            const std::size_t unitStart = *(it - 1);
            const std::size_t unitEnd = (it != starts.end()) ? *it : textLength;
            return {unitStart, unitEnd};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        int UiaTextRangeProvider::movePosition(const UiaElementSnapshot& text, std::size_t& pos, TextUnit unit, int count)
        {
            // The positions that can be reached are the starts of the units and the end of the text
            const std::size_t textLength = text.getText().length();
            std::vector<std::size_t> positions = findUnitStarts(text, unit);
            if ((unit == TextUnit_Format) || (unit == TextUnit_Page) || (unit == TextUnit_Document))
                positions = {0};
            if (positions.back() != textLength)
                positions.push_back(textLength);

            int moved = 0;
            while ((count > 0) && (moved < count))
            {
                const auto it = std::upper_bound(positions.begin(), positions.end(), pos);
                if (it == positions.end())
                    break;

                pos = *it;
                ++moved;
            }

            while ((count < 0) && (moved > count))
            {
                const auto it = std::lower_bound(positions.begin(), positions.end(), pos);
                if (it == positions.begin())
                    break;

                pos = *(it - 1);
                --moved;
            }

            return moved;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void UiaTextRangeProvider::clampEndpoints(std::size_t textLength)
        {
            // The text might have changed since the range was created
            m_start = std::min(m_start, textLength);
            m_end = std::min(m_end, textLength);
            if (m_start > m_end)
                m_end = m_start;
        }
    } // namespace priv
} // namespace tgui
