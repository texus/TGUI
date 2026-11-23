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

#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    // Creates a BSTR from a string, which has to be freed by the caller (or by UI Automation when it is returned to it)
    [[nodiscard]] BSTR createBstr(const tgui::String& str)
    {
        const std::wstring wideStr = str.toWideString();
        return SysAllocStringLen(wideStr.c_str(), static_cast<UINT>(wideStr.length()));
    }

    // Stores a string in a variant, or leaves the variant empty if the string is empty
    void setStringVariant(VARIANT& value, const tgui::String& str)
    {
        if (str.empty())
            return;

        value.bstrVal = createBstr(str);
        if (value.bstrVal)
            value.vt = VT_BSTR;
    }

    // Stores a boolean in a variant
    void setBoolVariant(VARIANT& value, bool flag)
    {
        value.vt = VT_BOOL;
        value.boolVal = flag ? VARIANT_TRUE : VARIANT_FALSE;
    }

    // Stores an integer in a variant
    void setIntVariant(VARIANT& value, int number)
    {
        value.vt = VT_I4;
        value.lVal = number;
    }

    // Returns the UI Automation control type that best matches the role of the object
    [[nodiscard]] CONTROLTYPEID getControlType(tgui::AccessibleRole role)
    {
        using tgui::AccessibleRole;
        switch (role)
        {
            case AccessibleRole::Window:
                return UIA_WindowControlTypeId;
            case AccessibleRole::Pane:
                return UIA_PaneControlTypeId;
            case AccessibleRole::Group:
                return UIA_GroupControlTypeId;
            case AccessibleRole::Button:
            case AccessibleRole::ToggleButton: // UI Automation has no toggle button, it is a button with a toggle pattern
                return UIA_ButtonControlTypeId;
            case AccessibleRole::CheckBox:
                return UIA_CheckBoxControlTypeId;
            case AccessibleRole::RadioButton:
                return UIA_RadioButtonControlTypeId;
            case AccessibleRole::Edit:
            case AccessibleRole::MultiLineEdit: // Multiple lines are indicated with the ARIA properties
                return UIA_EditControlTypeId;
            case AccessibleRole::Text:
                return UIA_TextControlTypeId;
            case AccessibleRole::Image:
                return UIA_ImageControlTypeId;
            case AccessibleRole::ComboBox:
                return UIA_ComboBoxControlTypeId;
            case AccessibleRole::List:
                return UIA_ListControlTypeId;
            case AccessibleRole::ListItem:
                return UIA_ListItemControlTypeId;
            case AccessibleRole::Tree:
                return UIA_TreeControlTypeId;
            case AccessibleRole::TreeItem:
                return UIA_TreeItemControlTypeId;
            case AccessibleRole::Table:
                return UIA_DataGridControlTypeId;
            case AccessibleRole::TableRow:
                return UIA_DataItemControlTypeId;
            case AccessibleRole::TableCell:
                return UIA_TextControlTypeId;
            case AccessibleRole::Header:
                return UIA_HeaderControlTypeId;
            case AccessibleRole::HeaderItem:
                return UIA_HeaderItemControlTypeId;
            case AccessibleRole::TabList:
                return UIA_TabControlTypeId;
            case AccessibleRole::TabItem:
                return UIA_TabItemControlTypeId;
            case AccessibleRole::Slider:
                return UIA_SliderControlTypeId;
            case AccessibleRole::ScrollBar:
                return UIA_ScrollBarControlTypeId;
            case AccessibleRole::Spinner:
                return UIA_SpinnerControlTypeId;
            case AccessibleRole::ProgressBar:
                return UIA_ProgressBarControlTypeId;
            case AccessibleRole::MenuBar:
                return UIA_MenuBarControlTypeId;
            case AccessibleRole::Menu:
                return UIA_MenuControlTypeId;
            case AccessibleRole::MenuItem:
                return UIA_MenuItemControlTypeId;
            case AccessibleRole::Separator:
                return UIA_SeparatorControlTypeId;
            case AccessibleRole::Default:
            case AccessibleRole::Custom:
                return UIA_CustomControlTypeId;
        }

        return UIA_CustomControlTypeId;
    }

    // Returns whether objects with the role contain information for the user, as opposed to only structuring the gui
    [[nodiscard]] bool isContentRole(tgui::AccessibleRole role)
    {
        using tgui::AccessibleRole;
        return (role != AccessibleRole::Pane) && (role != AccessibleRole::Separator) && (role != AccessibleRole::Header)
               && (role != AccessibleRole::ScrollBar);
    }

    // Converts a rectangle that UI Automation uses into a rectangle that TGUI uses
    [[nodiscard]] tgui::FloatRect toFloatRect(const UiaRect& rect)
    {
        return {static_cast<float>(rect.left), static_cast<float>(rect.top), static_cast<float>(rect.width), static_cast<float>(rect.height)};
    }

    // Returns whether a point in screen coordinates lies inside the rectangle
    [[nodiscard]] bool containsPoint(const UiaRect& rect, double x, double y)
    {
        return (x >= rect.left) && (y >= rect.top) && (x < rect.left + rect.width) && (y < rect.top + rect.height);
    }

    // Returns the provider with an extra reference, which is passed to the caller
    template <typename InterfaceType>
    [[nodiscard]] InterfaceType* addReference(tgui::priv::UiaElementProvider* provider)
    {
        if (!provider)
            return nullptr;

        provider->AddRef();
        return static_cast<InterfaceType*>(provider);
    }
} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        UiaElementSnapshot::~UiaElementSnapshot()
        {
            for (UiaElementProvider* provider : {parent, fragmentRoot, focusedDescendant, labeledBy})
            {
                if (provider)
                    provider->Release();
            }

            for (UiaElementProvider* child : children)
                child->Release();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        UiaElementProvider::UiaElementProvider(AccessibilityBackendUIA& backend, AccessibleObject::Ptr object) :
            m_backend{&backend},
            m_window{backend.getWindowHandle()},
            m_actionQueue{backend.getActionQueue()},
            m_uniqueId{object->getUniqueId()},
            m_object{std::move(object)}
        {
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleObject::Ptr UiaElementProvider::getObject() const
        {
            if (!m_backend || !m_object || !m_object->isAlive())
                return nullptr;

            return m_object;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void UiaElementProvider::disconnect()
        {
            m_backend = nullptr;
            m_object = nullptr;

            // The snapshot holds references to other providers, which have to be released to break reference cycles.
            // The snapshot is destroyed after the lock is released, as releasing providers may destroy them.
            std::shared_ptr<const UiaElementSnapshot> snapshot;
            {
                const std::lock_guard<std::mutex> lock{m_snapshotMutex};
                snapshot = std::move(m_snapshot);
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void UiaElementProvider::updateSnapshot()
        {
            auto snapshot = std::make_shared<UiaElementSnapshot>();
            const auto object = getObject();
            if (object)
            {
                AccessibilityTree& tree = m_backend->getTree();
                const auto root = tree.getRoot();
                const auto focusedObject = tree.getReportedFocusedObject();

                snapshot->alive = true;
                snapshot->root = (object == root);
                snapshot->state = object->getState();
                snapshot->role = object->getRole();
                snapshot->name = object->getName();

                // UI Automation would otherwise ask the window for its title, which only the thread of the window can answer.
                // A game only does that once per frame, so the title is read here, while we are on that thread.
                if (snapshot->root && snapshot->name.empty())
                    snapshot->name = m_backend->getWindowTitle();

                snapshot->description = object->getDescription();
                snapshot->helpText = object->getHelpText();
                snapshot->automationId = object->getAutomationId();
                snapshot->className = object->getClassName();
                snapshot->enabled = object->isEnabled();
                snapshot->offscreen = object->isOffscreen();
                snapshot->focusable = object->isFocusable();
                snapshot->hasKeyboardFocus = m_backend->hasWindowFocus() && (object == focusedObject);

                // The window provides the bounding rectangle of the root. Hidden objects have an empty rectangle.
                if (!snapshot->root && !snapshot->offscreen)
                    snapshot->bounds = m_backend->mapGuiToScreen(object->getBounds());

                if (snapshot->root)
                {
                    if (focusedObject && (focusedObject != object))
                        snapshot->focusedDescendant = m_backend->getProvider(focusedObject);
                }
                else
                    snapshot->parent = m_backend->getProvider(object->getParent());

                snapshot->fragmentRoot = m_backend->getProvider(root);
                snapshot->labeledBy = m_backend->getProvider(object->getLabel());
                for (const auto& child : object->getChildren())
                {
                    if (UiaElementProvider* childProvider = m_backend->getProvider(child))
                        snapshot->children.push_back(childProvider);
                }

                if (snapshot->state.hasText)
                {
                    snapshot->text = object->getText();
                    snapshot->textSelection = object->getTextSelection();
                    snapshot->textLines = object->getTextLines();
                    snapshot->textAttributes = object->getTextAttributes();
                    snapshot->visibleTextArea = toFloatRect(m_backend->mapGuiToScreen(object->getVisibleTextArea()));

                    // Finding the position of every character is expensive, so the positions of the previous snapshot are
                    // reused when the text didn't change and neither the first nor the last character moved
                    const std::size_t textLength = snapshot->text.length();
                    const FloatRect firstBounds = toFloatRect(m_backend->mapGuiToScreen(object->getCharacterBounds(0)));
                    const FloatRect lastBounds = toFloatRect(m_backend->mapGuiToScreen(object->getCharacterBounds(textLength)));
                    const auto previousSnapshot = getSnapshot();
                    if (previousSnapshot && previousSnapshot->characterBounds && (previousSnapshot->text == snapshot->text)
                        && (previousSnapshot->characterBounds->front() == firstBounds)
                        && (previousSnapshot->characterBounds->back() == lastBounds))
                    {
                        snapshot->characterBounds = previousSnapshot->characterBounds;
                    }
                    else
                    {
                        // Only the characters on visible lines are needed exactly, as the text pattern clips everything to
                        // the visible area. Characters on other lines get the position of their line start, which is enough
                        // to know which lines are visible. This keeps long texts from being measured character by character.
                        auto characterBounds = std::make_shared<std::vector<FloatRect>>(textLength + 1, firstBounds);
                        const FloatRect& visibleArea = snapshot->visibleTextArea;
                        for (const auto& line : snapshot->textLines)
                        {
                            const std::size_t lineEnd = std::min(line.second, textLength);
                            if (line.first > lineEnd)
                                continue;

                            const FloatRect lineStartBounds = toFloatRect(m_backend->mapGuiToScreen(object->getCharacterBounds(line.first)));
                            const bool lineVisible = (lineStartBounds.top + lineStartBounds.height > visibleArea.top)
                                                     && (lineStartBounds.top < visibleArea.top + visibleArea.height);

                            (*characterBounds)[line.first] = lineStartBounds;
                            for (std::size_t i = line.first + 1; i <= lineEnd; ++i)
                            {
                                if (lineVisible)
                                    (*characterBounds)[i] = toFloatRect(m_backend->mapGuiToScreen(object->getCharacterBounds(i)));
                                else
                                    (*characterBounds)[i] = {lineStartBounds.left, lineStartBounds.top, 0, lineStartBounds.height};
                            }
                        }

                        (*characterBounds)[textLength] = lastBounds;
                        snapshot->characterBounds = std::move(characterBounds);
                    }
                }
            }

            // The previous snapshot is destroyed after the lock is released, as it may release providers
            std::shared_ptr<const UiaElementSnapshot> previousSnapshot;
            {
                const std::lock_guard<std::mutex> lock{m_snapshotMutex};
                previousSnapshot = std::move(m_snapshot);
                m_snapshot = std::move(snapshot);
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<const UiaElementSnapshot> UiaElementProvider::getSnapshot() const
        {
            const std::lock_guard<std::mutex> lock{m_snapshotMutex};
            return m_snapshot;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename Func>
        HRESULT UiaElementProvider::readSnapshot(Func&& func) const
        {
            HRESULT result = S_OK;
            const auto snapshot = getAliveSnapshot(result);
            if (!snapshot)
                return result;

            return func(*snapshot);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void UiaElementProvider::getPropertyValue(const UiaElementSnapshot& snapshot, PROPERTYID propertyId, VARIANT& value)
        {
            value.vt = VT_EMPTY;
            if (!snapshot.alive)
                return;

            // Most properties of the root are provided by the window that hosts it (e.g. the name is the window title)
            const AccessibleState& state = snapshot.state;
            switch (propertyId)
            {
                case UIA_ControlTypePropertyId:
                    if (!snapshot.root)
                        setIntVariant(value, getControlType(snapshot.role));
                    break;
                case UIA_NamePropertyId:
                    setStringVariant(value, snapshot.name);
                    break;
                case UIA_HelpTextPropertyId:
                    setStringVariant(value, snapshot.helpText);
                    break;
                case UIA_FullDescriptionPropertyId:
                    setStringVariant(value, snapshot.description);
                    break;
                case UIA_AutomationIdPropertyId:
                    setStringVariant(value, snapshot.automationId);
                    break;
                case UIA_ClassNamePropertyId:
                    if (!snapshot.root)
                        setStringVariant(value, snapshot.className);
                    break;
                case UIA_FrameworkIdPropertyId:
                    setStringVariant(value, U"TGUI");
                    break;
                case UIA_IsEnabledPropertyId:
                    setBoolVariant(value, snapshot.enabled);
                    break;
                case UIA_HasKeyboardFocusPropertyId:
                    setBoolVariant(value, snapshot.hasKeyboardFocus);
                    break;
                case UIA_IsKeyboardFocusablePropertyId:
                    setBoolVariant(value, snapshot.focusable);
                    break;
                case UIA_IsOffscreenPropertyId:
                    if (!snapshot.root)
                        setBoolVariant(value, snapshot.offscreen);
                    break;
                case UIA_IsPasswordPropertyId:
                    setBoolVariant(value, state.password);
                    break;
                case UIA_AriaPropertiesPropertyId:
                    // UI Automation has no other way to tell clients that an edit control contains multiple lines
                    if (snapshot.role == AccessibleRole::MultiLineEdit)
                        setStringVariant(value, U"multiline=true");
                    break;
                case UIA_LabeledByPropertyId:
                    if (snapshot.labeledBy)
                    {
                        value.vt = VT_UNKNOWN;
                        value.punkVal = static_cast<IRawElementProviderSimple*>(snapshot.labeledBy);
                        value.punkVal->AddRef();
                    }
                    break;
                case UIA_IsControlElementPropertyId:
                    setBoolVariant(value, true);
                    break;
                case UIA_IsContentElementPropertyId:
                    setBoolVariant(value, isContentRole(snapshot.role));
                    break;
                case UIA_IsDialogPropertyId:
                    if (snapshot.role == AccessibleRole::Window)
                        setBoolVariant(value, state.modal);
                    break;
                case UIA_OrientationPropertyId:
                    if (state.orientation)
                        setIntVariant(value, (*state.orientation == Orientation::Horizontal) ? OrientationType_Horizontal : OrientationType_Vertical);
                    break;
                case UIA_PositionInSetPropertyId:
                    if (state.positionInSet > 0)
                        setIntVariant(value, state.positionInSet);
                    break;
                case UIA_SizeOfSetPropertyId:
                    if (state.sizeOfSet > 0)
                        setIntVariant(value, state.sizeOfSet);
                    break;
                case UIA_LevelPropertyId:
                    if (state.level > 0)
                        setIntVariant(value, state.level);
                    break;
                case UIA_ValueValuePropertyId:
                    if (state.value)
                    {
                        // An empty value is still a value, so setStringVariant can't be used here
                        value.bstrVal = createBstr(*state.value);
                        if (value.bstrVal)
                            value.vt = VT_BSTR;
                    }
                    break;
                case UIA_ValueIsReadOnlyPropertyId:
                    if (state.value)
                        setBoolVariant(value, state.readOnly);
                    break;
                case UIA_RangeValueValuePropertyId:
                    if (state.rangeInfo)
                    {
                        value.vt = VT_R8;
                        value.dblVal = state.rangeInfo->value;
                    }
                    break;
                case UIA_ToggleToggleStatePropertyId:
                    if (state.toggleState)
                    {
                        if (*state.toggleState == AccessibleToggleState::On)
                            setIntVariant(value, ToggleState_On);
                        else if (*state.toggleState == AccessibleToggleState::Indeterminate)
                            setIntVariant(value, ToggleState_Indeterminate);
                        else
                            setIntVariant(value, ToggleState_Off);
                    }
                    break;
                case UIA_ExpandCollapseExpandCollapseStatePropertyId:
                    if (state.expandState)
                    {
                        if (*state.expandState == AccessibleExpandState::Expanded)
                            setIntVariant(value, ExpandCollapseState_Expanded);
                        else if (*state.expandState == AccessibleExpandState::PartiallyExpanded)
                            setIntVariant(value, ExpandCollapseState_PartiallyExpanded);
                        else if (*state.expandState == AccessibleExpandState::LeafNode)
                            setIntVariant(value, ExpandCollapseState_LeafNode);
                        else
                            setIntVariant(value, ExpandCollapseState_Collapsed);
                    }
                    break;
                case UIA_SelectionItemIsSelectedPropertyId:
                    if (state.selectable)
                        setBoolVariant(value, state.selected);
                    break;
                default: // Other properties are either not supported or provided by UI Automation itself
                    break;
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::QueryInterface(REFIID riid, void** ppvObject)
        {
            if (!ppvObject)
                return E_POINTER;

            *ppvObject = nullptr;
            const auto snapshot = getSnapshot();
            if ((riid == __uuidof(IUnknown)) || (riid == __uuidof(IRawElementProviderSimple)))
                *ppvObject = static_cast<IRawElementProviderSimple*>(this);
            else if (riid == __uuidof(IRawElementProviderFragment))
                *ppvObject = static_cast<IRawElementProviderFragment*>(this);
            else if (snapshot && snapshot->alive)
            {
                // Pattern interfaces are only handed out when the object supports the pattern
                const AccessibleState& state = snapshot->state;
                if ((riid == __uuidof(IRawElementProviderFragmentRoot)) && snapshot->root)
                    *ppvObject = static_cast<IRawElementProviderFragmentRoot*>(this);
                else if ((riid == __uuidof(IInvokeProvider)) && state.invokable)
                    *ppvObject = static_cast<IInvokeProvider*>(this);
                else if ((riid == __uuidof(IToggleProvider)) && state.toggleState)
                    *ppvObject = static_cast<IToggleProvider*>(this);
                else if ((riid == __uuidof(IExpandCollapseProvider)) && state.expandState)
                    *ppvObject = static_cast<IExpandCollapseProvider*>(this);
                else if ((riid == __uuidof(IValueProvider)) && state.value)
                    *ppvObject = static_cast<IValueProvider*>(this);
                else if ((riid == __uuidof(IRangeValueProvider)) && state.rangeInfo)
                    *ppvObject = static_cast<IRangeValueProvider*>(this);
                else if ((riid == __uuidof(ISelectionProvider)) && state.selectionContainer)
                    *ppvObject = static_cast<ISelectionProvider*>(this);
                else if ((riid == __uuidof(ISelectionProvider2)) && state.selectionContainer)
                    *ppvObject = static_cast<ISelectionProvider2*>(this);
                else if ((riid == __uuidof(ISelectionItemProvider)) && state.selectable)
                    *ppvObject = static_cast<ISelectionItemProvider*>(this);
                else if ((riid == __uuidof(IScrollItemProvider)) && !snapshot->root)
                    *ppvObject = static_cast<IScrollItemProvider*>(this);
                else if ((riid == __uuidof(IWindowProvider)) && state.closable)
                    *ppvObject = static_cast<IWindowProvider*>(this);
                else if ((riid == __uuidof(IGridProvider)) && state.tableSize)
                    *ppvObject = static_cast<IGridProvider*>(this);
                else if ((riid == __uuidof(ITableProvider)) && state.tableSize)
                    *ppvObject = static_cast<ITableProvider*>(this);
                else if ((riid == __uuidof(IGridItemProvider)) && state.cellPosition)
                    *ppvObject = static_cast<IGridItemProvider*>(this);
                else if ((riid == __uuidof(ITableItemProvider)) && state.cellPosition)
                    *ppvObject = static_cast<ITableItemProvider*>(this);
            }

            if (!*ppvObject)
                return E_NOINTERFACE;

            AddRef();
            return S_OK;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ULONG UiaElementProvider::AddRef()
        {
            return static_cast<ULONG>(InterlockedIncrement(&m_referenceCount));
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ULONG UiaElementProvider::Release()
        {
            const LONG referenceCount = InterlockedDecrement(&m_referenceCount);
            if (referenceCount == 0)
                delete this;

            return static_cast<ULONG>(referenceCount);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_ProviderOptions(ProviderOptions* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            // The provider is free-threaded: UI Automation may call it on any thread, as all queries are answered from snapshots
            *pRetVal = ProviderOptions_ServerSideProvider;
            return S_OK;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::GetPatternProvider(PATTERNID patternId, IUnknown** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    // The text pattern is implemented by a separate object
                    if (patternId == UIA_TextPatternId)
                    {
                        if (snapshot.state.hasText)
                            *pRetVal = static_cast<ITextProvider*>(new UiaTextProvider(this));
                        return S_OK;
                    }

                    const IID* riid = nullptr;
                    switch (patternId)
                    {
                        case UIA_InvokePatternId:
                            riid = &__uuidof(IInvokeProvider);
                            break;
                        case UIA_TogglePatternId:
                            riid = &__uuidof(IToggleProvider);
                            break;
                        case UIA_ExpandCollapsePatternId:
                            riid = &__uuidof(IExpandCollapseProvider);
                            break;
                        case UIA_ValuePatternId:
                            riid = &__uuidof(IValueProvider);
                            break;
                        case UIA_RangeValuePatternId:
                            riid = &__uuidof(IRangeValueProvider);
                            break;
                        case UIA_SelectionPatternId:
                            riid = &__uuidof(ISelectionProvider);
                            break;
                        case UIA_SelectionPattern2Id:
                            riid = &__uuidof(ISelectionProvider2);
                            break;
                        case UIA_SelectionItemPatternId:
                            riid = &__uuidof(ISelectionItemProvider);
                            break;
                        case UIA_ScrollItemPatternId:
                            riid = &__uuidof(IScrollItemProvider);
                            break;
                        case UIA_WindowPatternId:
                            riid = &__uuidof(IWindowProvider);
                            break;
                        case UIA_GridPatternId:
                            riid = &__uuidof(IGridProvider);
                            break;
                        case UIA_GridItemPatternId:
                            riid = &__uuidof(IGridItemProvider);
                            break;
                        case UIA_TablePatternId:
                            riid = &__uuidof(ITableProvider);
                            break;
                        case UIA_TableItemPatternId:
                            riid = &__uuidof(ITableItemProvider);
                            break;
                        default: // The pattern isn't supported by any widget
                            break;
                    }

                    // QueryInterface checks whether the pattern is supported, a nullptr means that it isn't
                    if (riid && FAILED(QueryInterface(*riid, reinterpret_cast<void**>(pRetVal))))
                        *pRetVal = nullptr;

                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::GetPropertyValue(PROPERTYID propertyId, VARIANT* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            pRetVal->vt = VT_EMPTY;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    getPropertyValue(snapshot, propertyId, *pRetVal);
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_HostRawElementProvider(IRawElementProviderSimple** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;

            // Only the root is hosted in the window, the other elements are fragments inside the root
            const auto snapshot = getSnapshot();
            if (!snapshot || !snapshot->root)
                return S_OK;

            return UiaHostProviderFromHwnd(m_window, pRetVal);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::Navigate(NavigateDirection direction, IRawElementProviderFragment** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    switch (direction)
                    {
                        case NavigateDirection_Parent:
                            // The parent of the root is the window, which UI Automation already knows about
                            *pRetVal = addReference<IRawElementProviderFragment>(snapshot.parent);
                            break;
                        case NavigateDirection_FirstChild:
                            if (!snapshot.children.empty())
                                *pRetVal = addReference<IRawElementProviderFragment>(snapshot.children.front());
                            break;
                        case NavigateDirection_LastChild:
                            if (!snapshot.children.empty())
                                *pRetVal = addReference<IRawElementProviderFragment>(snapshot.children.back());
                            break;
                        case NavigateDirection_NextSibling:
                        case NavigateDirection_PreviousSibling:
                        {
                            const auto parentSnapshot = snapshot.parent ? snapshot.parent->getSnapshot() : nullptr;
                            if (!parentSnapshot)
                                break;

                            const auto& siblings = parentSnapshot->children;
                            const auto it = std::find(siblings.begin(), siblings.end(), this);
                            if (it == siblings.end())
                                break;

                            // The sibling must get its reference while the parent snapshot still holds a reference to it
                            if (direction == NavigateDirection_NextSibling)
                            {
                                if (it + 1 != siblings.end())
                                    *pRetVal = addReference<IRawElementProviderFragment>(*(it + 1));
                            }
                            else
                            {
                                if (it != siblings.begin())
                                    *pRetVal = addReference<IRawElementProviderFragment>(*(it - 1));
                            }
                            break;
                        }
                    }

                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::GetRuntimeId(SAFEARRAY** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    // The root uses the runtime id of the window that hosts it
                    if (snapshot.root)
                        return S_OK;

                    SAFEARRAY* runtimeId = SafeArrayCreateVector(VT_I4, 0, 2);
                    if (!runtimeId)
                        return E_OUTOFMEMORY;

                    int values[] = {UiaAppendRuntimeId, static_cast<int>(m_uniqueId)};
                    for (LONG i = 0; i < 2; ++i)
                    {
                        const HRESULT putResult = SafeArrayPutElement(runtimeId, &i, &values[i]);
                        if (FAILED(putResult))
                        {
                            SafeArrayDestroy(runtimeId);
                            return putResult;
                        }
                    }

                    *pRetVal = runtimeId;
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_BoundingRectangle(UiaRect* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = {};
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    *pRetVal = snapshot.bounds;
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::GetEmbeddedFragmentRoots(SAFEARRAY** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            // Widgets never contain other UI Automation trees
            *pRetVal = nullptr;
            return S_OK;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::SetFocus()
        {
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    if (!snapshot.enabled)
                        return static_cast<HRESULT>(UIA_E_ELEMENTNOTENABLED);
                    if (!snapshot.focusable)
                        return static_cast<HRESULT>(UIA_E_INVALIDOPERATION);

                    queueAction([](AccessibleObject& object) { object.setFocus(); });
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_FragmentRoot(IRawElementProviderFragmentRoot** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    *pRetVal = addReference<IRawElementProviderFragmentRoot>(snapshot.fragmentRoot);
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::ElementProviderFromPoint(double x, double y, IRawElementProviderFragment** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;
            return readSnapshot(
                [&](const UiaElementSnapshot& rootSnapshot)
                {
                    // Descend into the deepest visible child that contains the point. Children are ordered from back to front,
                    // so the last child that contains the point is the one on top. A reference is kept to the provider that
                    // was found, as the snapshot that referenced it is released while descending.
                    UiaElementProvider* target = nullptr;
                    std::shared_ptr<const UiaElementSnapshot> targetSnapshot;
                    const UiaElementSnapshot* current = &rootSnapshot;
                    while (current)
                    {
                        UiaElementProvider* hit = nullptr;
                        std::shared_ptr<const UiaElementSnapshot> hitSnapshot;
                        for (auto it = current->children.rbegin(); it != current->children.rend(); ++it)
                        {
                            auto childSnapshot = (*it)->getSnapshot();
                            if (childSnapshot && childSnapshot->alive && !childSnapshot->offscreen
                                && containsPoint(childSnapshot->bounds, x, y))
                            {
                                hit = addReference<UiaElementProvider>(*it);
                                hitSnapshot = std::move(childSnapshot);
                                break;
                            }
                        }

                        if (!hit)
                            break;

                        if (target)
                            target->Release();

                        target = hit;
                        targetSnapshot = std::move(hitSnapshot);
                        current = targetSnapshot.get();
                    }

                    // When the point is on the root itself then we return a nullptr and UI Automation will use the root.
                    // The reference that we hold is passed to the caller.
                    *pRetVal = static_cast<IRawElementProviderFragment*>(target);
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::GetFocus(IRawElementProviderFragment** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            // When the root itself has focus then focusedDescendant is a nullptr, which is what we must return in that case
            *pRetVal = nullptr;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    *pRetVal = addReference<IRawElementProviderFragment>(snapshot.focusedDescendant);
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::Invoke()
        {
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    if (!snapshot.enabled)
                        return static_cast<HRESULT>(UIA_E_ELEMENTNOTENABLED);

                    queueAction([](AccessibleObject& object) { object.performAction(AccessibleAction::Invoke); });
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::Toggle()
        {
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    if (!snapshot.enabled)
                        return static_cast<HRESULT>(UIA_E_ELEMENTNOTENABLED);

                    queueAction([](AccessibleObject& object) { object.performAction(AccessibleAction::Toggle); });
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_ToggleState(ToggleState* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = ToggleState_Off;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    const auto& toggleState = snapshot.state.toggleState;
                    if (toggleState && (*toggleState == AccessibleToggleState::On))
                        *pRetVal = ToggleState_On;
                    else if (toggleState && (*toggleState == AccessibleToggleState::Indeterminate))
                        *pRetVal = ToggleState_Indeterminate;

                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::Expand()
        {
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    if (!snapshot.enabled)
                        return static_cast<HRESULT>(UIA_E_ELEMENTNOTENABLED);

                    // Leaf nodes can't be expanded
                    const auto& expandState = snapshot.state.expandState;
                    if (!expandState || (*expandState == AccessibleExpandState::LeafNode))
                        return static_cast<HRESULT>(UIA_E_INVALIDOPERATION);

                    queueAction([](AccessibleObject& object) { object.performAction(AccessibleAction::Expand); });
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::Collapse()
        {
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    if (!snapshot.enabled)
                        return static_cast<HRESULT>(UIA_E_ELEMENTNOTENABLED);

                    const auto& expandState = snapshot.state.expandState;
                    if (!expandState || (*expandState == AccessibleExpandState::LeafNode))
                        return static_cast<HRESULT>(UIA_E_INVALIDOPERATION);

                    queueAction([](AccessibleObject& object) { object.performAction(AccessibleAction::Collapse); });
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_ExpandCollapseState(ExpandCollapseState* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = ExpandCollapseState_LeafNode;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    VARIANT value;
                    VariantInit(&value);
                    getPropertyValue(snapshot, UIA_ExpandCollapseExpandCollapseStatePropertyId, value);
                    if (value.vt == VT_I4)
                        *pRetVal = static_cast<ExpandCollapseState>(value.lVal);

                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::SetValue(LPCWSTR val)
        {
            if (!val)
                return E_INVALIDARG;

            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    if (!snapshot.enabled || snapshot.state.readOnly)
                        return static_cast<HRESULT>(UIA_E_ELEMENTNOTENABLED);

                    queueAction([value = String{val}](AccessibleObject& object) { object.setValue(value); });
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_Value(BSTR* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    *pRetVal = createBstr(snapshot.state.value ? *snapshot.state.value : String{});
                    return *pRetVal ? S_OK : E_OUTOFMEMORY;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_IsReadOnly(BOOL* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = FALSE;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    // This function is shared between the value pattern and the range value pattern
                    if (snapshot.state.value)
                        *pRetVal = snapshot.state.readOnly ? TRUE : FALSE;
                    else if (snapshot.state.rangeInfo)
                        *pRetVal = snapshot.state.rangeInfo->readOnly ? TRUE : FALSE;

                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::SetValue(double val)
        {
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    const auto& rangeInfo = snapshot.state.rangeInfo;
                    if (!rangeInfo)
                        return static_cast<HRESULT>(UIA_E_INVALIDOPERATION);
                    if (!snapshot.enabled || rangeInfo->readOnly)
                        return static_cast<HRESULT>(UIA_E_ELEMENTNOTENABLED);
                    if ((val < rangeInfo->minimum) || (val > rangeInfo->maximum))
                        return E_INVALIDARG;

                    queueAction([val](AccessibleObject& object) { object.setRangeValue(val); });
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_Value(double* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = 0;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    if (snapshot.state.rangeInfo)
                        *pRetVal = snapshot.state.rangeInfo->value;
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_Maximum(double* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = 0;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    if (snapshot.state.rangeInfo)
                        *pRetVal = snapshot.state.rangeInfo->maximum;
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_Minimum(double* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = 0;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    if (snapshot.state.rangeInfo)
                        *pRetVal = snapshot.state.rangeInfo->minimum;
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_LargeChange(double* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = 0;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    if (snapshot.state.rangeInfo)
                        *pRetVal = snapshot.state.rangeInfo->largeChange;
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_SmallChange(double* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = 0;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    if (snapshot.state.rangeInfo)
                        *pRetVal = snapshot.state.rangeInfo->smallChange;
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::GetSelection(SAFEARRAY** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    // The selection array may be empty, but it must always exist
                    std::vector<UiaElementProvider*> selected;
                    addSelectedDescendants(snapshot, selected);
                    *pRetVal = createProviderArray(selected);
                    for (UiaElementProvider* provider : selected)
                        provider->Release();

                    return *pRetVal ? S_OK : E_OUTOFMEMORY;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_CanSelectMultiple(BOOL* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = FALSE;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    *pRetVal = snapshot.state.multipleSelection ? TRUE : FALSE;
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_IsSelectionRequired(BOOL* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = FALSE;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    *pRetVal = snapshot.state.selectionRequired ? TRUE : FALSE;
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_FirstSelectedItem(IRawElementProviderSimple** retVal)
        {
            if (!retVal)
                return E_POINTER;

            *retVal = nullptr;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    std::vector<UiaElementProvider*> selected;
                    addSelectedDescendants(snapshot, selected);
                    for (std::size_t i = 0; i < selected.size(); ++i)
                    {
                        // The reference of the first item is passed to the caller
                        if (i == 0)
                            *retVal = static_cast<IRawElementProviderSimple*>(selected[i]);
                        else
                            selected[i]->Release();
                    }

                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_LastSelectedItem(IRawElementProviderSimple** retVal)
        {
            if (!retVal)
                return E_POINTER;

            *retVal = nullptr;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    std::vector<UiaElementProvider*> selected;
                    addSelectedDescendants(snapshot, selected);
                    for (std::size_t i = 0; i < selected.size(); ++i)
                    {
                        // The reference of the last item is passed to the caller
                        if (i + 1 == selected.size())
                            *retVal = static_cast<IRawElementProviderSimple*>(selected[i]);
                        else
                            selected[i]->Release();
                    }

                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_CurrentSelectedItem(IRawElementProviderSimple** retVal)
        {
            // The focus follows the selection in all widgets, so the current item is the first selected item
            return get_FirstSelectedItem(retVal);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_ItemCount(int* retVal)
        {
            if (!retVal)
                return E_POINTER;

            *retVal = 0;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    std::vector<UiaElementProvider*> selected;
                    addSelectedDescendants(snapshot, selected);
                    *retVal = static_cast<int>(selected.size());
                    for (UiaElementProvider* provider : selected)
                        provider->Release();

                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::Select()
        {
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    if (!snapshot.enabled)
                        return static_cast<HRESULT>(UIA_E_ELEMENTNOTENABLED);

                    queueAction([](AccessibleObject& object) { object.performAction(AccessibleAction::Select); });
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::AddToSelection()
        {
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    if (!snapshot.enabled)
                        return static_cast<HRESULT>(UIA_E_ELEMENTNOTENABLED);

                    queueAction([](AccessibleObject& object) { object.performAction(AccessibleAction::AddToSelection); });
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::RemoveFromSelection()
        {
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    if (!snapshot.enabled)
                        return static_cast<HRESULT>(UIA_E_ELEMENTNOTENABLED);

                    // None of the widgets allow removing the selected item without selecting another one
                    if (snapshot.state.selected && !snapshot.state.multipleSelection)
                        return static_cast<HRESULT>(UIA_E_INVALIDOPERATION);

                    queueAction([](AccessibleObject& object) { object.performAction(AccessibleAction::RemoveFromSelection); });
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_IsSelected(BOOL* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = FALSE;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    *pRetVal = snapshot.state.selected ? TRUE : FALSE;
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_SelectionContainer(IRawElementProviderSimple** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    // The closest ancestor that manages the selection, or the parent if no ancestor does (e.g. radio buttons).
                    // The snapshot of each ancestor is kept until the next ancestor has a reference of its own.
                    UiaElementProvider* container = addReference<UiaElementProvider>(snapshot.parent);
                    UiaElementProvider* ancestor = addReference<UiaElementProvider>(snapshot.parent);
                    while (ancestor)
                    {
                        const auto ancestorSnapshot = ancestor->getSnapshot();
                        if (ancestorSnapshot && ancestorSnapshot->state.selectionContainer)
                        {
                            container->Release();
                            container = ancestor;
                            break;
                        }

                        UiaElementProvider* nextAncestor =
                            ancestorSnapshot ? addReference<UiaElementProvider>(ancestorSnapshot->parent) : nullptr;
                        ancestor->Release();
                        ancestor = nextAncestor;
                    }

                    *pRetVal = static_cast<IRawElementProviderSimple*>(container);
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::ScrollIntoView()
        {
            return readSnapshot(
                [&](const UiaElementSnapshot&)
                {
                    queueAction([](AccessibleObject& object) { object.performAction(AccessibleAction::ScrollIntoView); });
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::SetVisualState(WindowVisualState state)
        {
            // Child windows can't be minimized or maximized via code, so only the normal state is supported
            return readSnapshot([state](const UiaElementSnapshot&)
                                { return (state == WindowVisualState_Normal) ? S_OK : static_cast<HRESULT>(UIA_E_INVALIDOPERATION); });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::Close()
        {
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    if (!snapshot.state.closable)
                        return static_cast<HRESULT>(UIA_E_INVALIDOPERATION);

                    queueAction([](AccessibleObject& object) { object.performAction(AccessibleAction::Close); });
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::WaitForInputIdle(int, BOOL* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = TRUE;
            return readSnapshot([](const UiaElementSnapshot&) { return S_OK; });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_CanMaximize(BOOL* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = FALSE;
            return readSnapshot([](const UiaElementSnapshot&) { return S_OK; });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_CanMinimize(BOOL* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = FALSE;
            return readSnapshot([](const UiaElementSnapshot&) { return S_OK; });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_IsModal(BOOL* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = FALSE;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    *pRetVal = snapshot.state.modal ? TRUE : FALSE;
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_WindowVisualState(WindowVisualState* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = WindowVisualState_Normal;
            return readSnapshot([](const UiaElementSnapshot&) { return S_OK; });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_WindowInteractionState(WindowInteractionState* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = WindowInteractionState_ReadyForUserInteraction;
            return readSnapshot([](const UiaElementSnapshot&) { return S_OK; });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_IsTopmost(BOOL* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = FALSE;
            return readSnapshot([](const UiaElementSnapshot&) { return S_OK; });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::GetItem(int row, int column, IRawElementProviderSimple** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    const auto& tableSize = snapshot.state.tableSize;
                    if (!tableSize || (row < 0) || (column < 0) || (static_cast<std::size_t>(row) >= tableSize->x)
                        || (static_cast<std::size_t>(column) >= tableSize->y))
                    {
                        return E_INVALIDARG;
                    }

                    // The rows of a table are its children with the TableRow role, the cells are the children of the rows
                    int rowIndex = 0;
                    for (UiaElementProvider* child : snapshot.children)
                    {
                        const auto childSnapshot = child->getSnapshot();
                        if (!childSnapshot || (childSnapshot->role != AccessibleRole::TableRow))
                            continue;

                        if (rowIndex++ != row)
                            continue;

                        if (static_cast<std::size_t>(column) < childSnapshot->children.size())
                            *pRetVal = addReference<IRawElementProviderSimple>(childSnapshot->children[static_cast<std::size_t>(column)]);
                        break;
                    }

                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_RowCount(int* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = 0;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    if (snapshot.state.tableSize)
                        *pRetVal = static_cast<int>(snapshot.state.tableSize->x);
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_ColumnCount(int* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = 0;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    if (snapshot.state.tableSize)
                        *pRetVal = static_cast<int>(snapshot.state.tableSize->y);
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_Row(int* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = 0;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    if (snapshot.state.cellPosition)
                        *pRetVal = static_cast<int>(snapshot.state.cellPosition->x);
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_Column(int* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = 0;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    if (snapshot.state.cellPosition)
                        *pRetVal = static_cast<int>(snapshot.state.cellPosition->y);
                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_RowSpan(int* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            // Cells never span multiple rows or columns
            *pRetVal = 1;
            return readSnapshot([](const UiaElementSnapshot&) { return S_OK; });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_ColumnSpan(int* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = 1;
            return readSnapshot([](const UiaElementSnapshot&) { return S_OK; });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_ContainingGrid(IRawElementProviderSimple** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    // A reference is held to each ancestor while its snapshot is being looked at
                    UiaElementProvider* ancestor = addReference<UiaElementProvider>(snapshot.parent);
                    while (ancestor)
                    {
                        const auto ancestorSnapshot = ancestor->getSnapshot();
                        if (ancestorSnapshot && (ancestorSnapshot->role == AccessibleRole::Table))
                        {
                            *pRetVal = static_cast<IRawElementProviderSimple*>(ancestor);
                            break;
                        }

                        UiaElementProvider* nextAncestor =
                            ancestorSnapshot ? addReference<UiaElementProvider>(ancestorSnapshot->parent) : nullptr;
                        ancestor->Release();
                        ancestor = nextAncestor;
                    }

                    return S_OK;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::GetRowHeaders(SAFEARRAY** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            // Tables in TGUI only have column headers
            *pRetVal = nullptr;
            return readSnapshot(
                [&](const UiaElementSnapshot&)
                {
                    *pRetVal = createProviderArray({});
                    return *pRetVal ? S_OK : E_OUTOFMEMORY;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::GetColumnHeaders(SAFEARRAY** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    // The column headers are the children of the child with the Header role
                    for (UiaElementProvider* child : snapshot.children)
                    {
                        const auto childSnapshot = child->getSnapshot();
                        if (childSnapshot && (childSnapshot->role == AccessibleRole::Header))
                        {
                            *pRetVal = createProviderArray(childSnapshot->children);
                            return *pRetVal ? S_OK : E_OUTOFMEMORY;
                        }
                    }

                    *pRetVal = createProviderArray({});
                    return *pRetVal ? S_OK : E_OUTOFMEMORY;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::get_RowOrColumnMajor(RowOrColumnMajor* pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = RowOrColumnMajor_RowMajor;
            return readSnapshot([](const UiaElementSnapshot&) { return S_OK; });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::GetRowHeaderItems(SAFEARRAY** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;
            return readSnapshot(
                [&](const UiaElementSnapshot&)
                {
                    *pRetVal = createProviderArray({});
                    return *pRetVal ? S_OK : E_OUTOFMEMORY;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        HRESULT UiaElementProvider::GetColumnHeaderItems(SAFEARRAY** pRetVal)
        {
            if (!pRetVal)
                return E_POINTER;

            *pRetVal = nullptr;
            return readSnapshot(
                [&](const UiaElementSnapshot& snapshot)
                {
                    if (!snapshot.state.cellPosition)
                    {
                        *pRetVal = createProviderArray({});
                        return *pRetVal ? S_OK : E_OUTOFMEMORY;
                    }

                    // Find the table that contains this cell and return the header of the same column
                    IRawElementProviderSimple* table = nullptr;
                    SAFEARRAY* columnHeaders = nullptr;
                    if (SUCCEEDED(get_ContainingGrid(&table)) && table)
                    {
                        static_cast<void>(static_cast<UiaElementProvider*>(table)->GetColumnHeaders(&columnHeaders));
                        table->Release();
                    }

                    std::vector<UiaElementProvider*> headers;
                    IUnknown* header = nullptr;
                    LONG index = static_cast<LONG>(snapshot.state.cellPosition->y);
                    if (columnHeaders && SUCCEEDED(SafeArrayGetElement(columnHeaders, &index, &header)) && header)
                        headers.push_back(static_cast<UiaElementProvider*>(static_cast<IRawElementProviderSimple*>(header)));

                    *pRetVal = createProviderArray(headers);
                    if (header)
                        header->Release();
                    if (columnHeaders)
                        SafeArrayDestroy(columnHeaders);

                    return *pRetVal ? S_OK : E_OUTOFMEMORY;
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<const UiaElementSnapshot> UiaElementProvider::getAliveSnapshot(HRESULT& result) const
        {
            auto snapshot = getSnapshot();
            if (!snapshot || !snapshot->alive)
            {
                result = static_cast<HRESULT>(UIA_E_ELEMENTNOTAVAILABLE);
                return nullptr;
            }

            result = S_OK;
            return snapshot;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleObject::Ptr UiaElementProvider::getAliveObject(HRESULT& result) const
        {
            auto object = getObject();
            result = object ? S_OK : static_cast<HRESULT>(UIA_E_ELEMENTNOTAVAILABLE);
            return object;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void UiaElementProvider::queueAction(std::function<void(AccessibleObject&)> action)
        {
            // The action is executed on the gui thread. The provider is kept alive until then, at which point the action is
            // only executed if the provider is still connected to its object.
            AddRef();
            m_actionQueue->push(
                [this, action = std::move(action)]
                {
                    HRESULT result = S_OK;
                    if (const auto object = getAliveObject(result))
                    {
                        action(*object);

                        // The action may have disconnected the provider (e.g. when it closed the window that it represents)
                        if (m_backend)
                            m_backend->getTree().requestRedraw();
                    }

                    Release();
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SAFEARRAY* UiaElementProvider::createProviderArray(const std::vector<UiaElementProvider*>& providers)
        {
            SAFEARRAY* array = SafeArrayCreateVector(VT_UNKNOWN, 0, static_cast<ULONG>(providers.size()));
            if (!array)
                return nullptr;

            // SafeArrayPutElement adds its own reference to each provider
            for (LONG i = 0; i < static_cast<LONG>(providers.size()); ++i)
            {
                IUnknown* provider = static_cast<IRawElementProviderSimple*>(providers[static_cast<std::size_t>(i)]);
                if (FAILED(SafeArrayPutElement(array, &i, provider)))
                {
                    SafeArrayDestroy(array);
                    return nullptr;
                }
            }

            return array;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void UiaElementProvider::addSelectedDescendants(const UiaElementSnapshot& snapshot, std::vector<UiaElementProvider*>& selected)
        {
            // Selected items can be nested (e.g. in a tree view), but selection containers inside them manage their own
            // selection. Each provider that is added to the list gets a reference, which the caller has to release.
            for (UiaElementProvider* child : snapshot.children)
            {
                const auto childSnapshot = child->getSnapshot();
                if (!childSnapshot || !childSnapshot->alive)
                    continue;

                if (childSnapshot->state.selectable && childSnapshot->state.selected)
                    selected.push_back(addReference<UiaElementProvider>(child));
                if (!childSnapshot->state.selectionContainer)
                    addSelectedDescendants(*childSnapshot, selected);
            }
        }
    } // namespace priv
} // namespace tgui
