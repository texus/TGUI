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

#ifndef TGUI_UIA_PROVIDERS_HPP
#define TGUI_UIA_PROVIDERS_HPP

#include <TGUI/Accessibility/AccessibleObject.hpp>
#include <TGUI/extlibs/IncludeWindows.hpp>

#include <ole2.h>
#include <uiautomation.h>

#include <functional>
#include <memory>
#include <mutex>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        class AccessibilityBackendUIA;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Actions that UI Automation requested, waiting to be executed on the gui thread.
        //
        // UI Automation calls the providers on its own threads, which aren't allowed to access the widgets. Like AccessKit,
        // actions are therefore executed asynchronously: the provider adds them to this queue and returns immediately, and
        // the gui thread executes them during its next update. The results are reported later via events.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class UiaActionQueue
        {
        public:
            // Constructor
            explicit UiaActionQueue(HWND window);

            // Adds an action to the queue and wakes up the gui thread if it was waiting for events. Can be called on any thread.
            void push(std::function<void()> action);

            // Removes all actions from the queue and returns them. Only called on the gui thread.
            [[nodiscard]] std::vector<std::function<void()>> takeAll();

        private:
            HWND m_window;
            std::mutex m_mutex;
            std::vector<std::function<void()>> m_actions;
        };

        class UiaElementProvider;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // State of an accessible object as it was when the gui thread last updated it.
        //
        // UI Automation calls the providers on its own threads, which aren't allowed to access the widgets. The providers
        // answer most requests from this snapshot, so that they don't have to wait for the gui thread. A snapshot never
        // changes once it has been created, the gui thread replaces it with a new one instead.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct UiaElementSnapshot
        {
            UiaElementSnapshot() = default;
            ~UiaElementSnapshot(); // Releases the providers

            UiaElementSnapshot(const UiaElementSnapshot&) = delete;
            UiaElementSnapshot& operator=(const UiaElementSnapshot&) = delete;

            // Accessors with the same names as in AccessibleObject, which are used by the text range functions
            [[nodiscard]] const String& getText() const
            {
                return text;
            }

            [[nodiscard]] const std::vector<std::pair<std::size_t, std::size_t>>& getTextLines() const
            {
                return textLines;
            }

            bool alive = false;
            bool root = false;
            AccessibleState state;
            AccessibleRole role = AccessibleRole::Custom;
            String name;
            String description;
            String helpText;
            String automationId;
            String className;
            bool enabled = false;
            bool offscreen = true;
            bool focusable = false;
            bool hasKeyboardFocus = false;
            UiaRect bounds = {};
            UiaElementProvider* parent = nullptr;            // Holds a reference
            UiaElementProvider* fragmentRoot = nullptr;      // Holds a reference
            UiaElementProvider* focusedDescendant = nullptr; // Holds a reference, only set for the root
            UiaElementProvider* labeledBy = nullptr;         // Holds a reference, the label that names the element
            std::vector<UiaElementProvider*> children;       // Holds a reference to each child

            // Only filled in when the state has text
            String text;
            std::pair<std::size_t, std::size_t> textSelection;
            std::vector<std::pair<std::size_t, std::size_t>> textLines;
            AccessibleTextAttributes textAttributes;
            std::shared_ptr<const std::vector<FloatRect>> characterBounds; // Screen coordinates, one more than characters
            FloatRect visibleTextArea;                                        // Screen coordinates
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // UI Automation provider for a single accessible object.
        // The text pattern is implemented by a separate UiaTextProvider, because ITextProvider and ISelectionProvider both
        // have a GetSelection function with the same signature.
        //
        // The provider implements all control patterns, but QueryInterface and GetPatternProvider only hand out the patterns
        // that the accessible object supports. Requests that only read information are answered from the snapshot on the
        // thread of UI Automation, while actions are executed on the gui thread. Once the object is removed from the tree,
        // the provider is disconnected and all its functions return UIA_E_ELEMENTNOTAVAILABLE.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class UiaElementProvider : public IRawElementProviderSimple,
                                   public IRawElementProviderFragment,
                                   public IRawElementProviderFragmentRoot,
                                   public IInvokeProvider,
                                   public IToggleProvider,
                                   public IExpandCollapseProvider,
                                   public IValueProvider,
                                   public IRangeValueProvider,
                                   public ISelectionProvider2,
                                   public ISelectionItemProvider,
                                   public IScrollItemProvider,
                                   public IWindowProvider,
                                   public IGridProvider,
                                   public IGridItemProvider,
                                   public ITableProvider,
                                   public ITableItemProvider
        {
        public:
            // Constructor. The reference count starts at 1, which is owned by the backend.
            UiaElementProvider(AccessibilityBackendUIA& backend, AccessibleObject::Ptr object);

            UiaElementProvider(const UiaElementProvider&) = delete;
            UiaElementProvider& operator=(const UiaElementProvider&) = delete;

            // Returns the accessible object, or a nullptr if the provider was disconnected or the object no longer exists
            [[nodiscard]] AccessibleObject::Ptr getObject() const;

            // Returns the window that contains the element, which stays valid after the provider is disconnected
            [[nodiscard]] HWND getWindowHandle() const
            {
                return m_window;
            }

            // Returns the backend, or a nullptr if the provider was disconnected
            [[nodiscard]] AccessibilityBackendUIA* getBackend() const
            {
                return m_backend;
            }

            // Detaches the provider from the backend and the object, after which all requests fail
            void disconnect();

            // Updates the snapshot with the current state of the object. Only called on the gui thread.
            void updateSnapshot();

            // Returns the latest snapshot, or a nullptr when the provider is disconnected. Can be called on any thread.
            [[nodiscard]] std::shared_ptr<const UiaElementSnapshot> getSnapshot() const;

            // Fills the variant with the value of a property in the snapshot. The variant is left empty if it isn't supported.
            static void getPropertyValue(const UiaElementSnapshot& snapshot, PROPERTYID propertyId, VARIANT& value);

            // Queues an action that will be executed on the gui thread, if the object still exists at that moment.
            // Returns immediately, the changes caused by the action are reported via events.
            void queueAction(std::function<void(AccessibleObject&)> action);

            // IUnknown
            HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;
            ULONG STDMETHODCALLTYPE AddRef() override;
            ULONG STDMETHODCALLTYPE Release() override;

            // IRawElementProviderSimple
            HRESULT STDMETHODCALLTYPE get_ProviderOptions(ProviderOptions* pRetVal) override;
            HRESULT STDMETHODCALLTYPE GetPatternProvider(PATTERNID patternId, IUnknown** pRetVal) override;
            HRESULT STDMETHODCALLTYPE GetPropertyValue(PROPERTYID propertyId, VARIANT* pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_HostRawElementProvider(IRawElementProviderSimple** pRetVal) override;

            // IRawElementProviderFragment
            HRESULT STDMETHODCALLTYPE Navigate(NavigateDirection direction, IRawElementProviderFragment** pRetVal) override;
            HRESULT STDMETHODCALLTYPE GetRuntimeId(SAFEARRAY** pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_BoundingRectangle(UiaRect* pRetVal) override;
            HRESULT STDMETHODCALLTYPE GetEmbeddedFragmentRoots(SAFEARRAY** pRetVal) override;
            HRESULT STDMETHODCALLTYPE SetFocus() override;
            HRESULT STDMETHODCALLTYPE get_FragmentRoot(IRawElementProviderFragmentRoot** pRetVal) override;

            // IRawElementProviderFragmentRoot
            HRESULT STDMETHODCALLTYPE ElementProviderFromPoint(double x, double y, IRawElementProviderFragment** pRetVal) override;
            HRESULT STDMETHODCALLTYPE GetFocus(IRawElementProviderFragment** pRetVal) override;

            // IInvokeProvider
            HRESULT STDMETHODCALLTYPE Invoke() override;

            // IToggleProvider
            HRESULT STDMETHODCALLTYPE Toggle() override;
            HRESULT STDMETHODCALLTYPE get_ToggleState(ToggleState* pRetVal) override;

            // IExpandCollapseProvider
            HRESULT STDMETHODCALLTYPE Expand() override;
            HRESULT STDMETHODCALLTYPE Collapse() override;
            HRESULT STDMETHODCALLTYPE get_ExpandCollapseState(ExpandCollapseState* pRetVal) override;

            // IValueProvider
            HRESULT STDMETHODCALLTYPE SetValue(LPCWSTR val) override;
            HRESULT STDMETHODCALLTYPE get_Value(BSTR* pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_IsReadOnly(BOOL* pRetVal) override;

            // IRangeValueProvider (get_IsReadOnly is shared with IValueProvider)
            HRESULT STDMETHODCALLTYPE SetValue(double val) override;
            HRESULT STDMETHODCALLTYPE get_Value(double* pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_Maximum(double* pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_Minimum(double* pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_LargeChange(double* pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_SmallChange(double* pRetVal) override;

            // ISelectionProvider
            HRESULT STDMETHODCALLTYPE GetSelection(SAFEARRAY** pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_CanSelectMultiple(BOOL* pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_IsSelectionRequired(BOOL* pRetVal) override;

            // ISelectionProvider2
            HRESULT STDMETHODCALLTYPE get_FirstSelectedItem(IRawElementProviderSimple** retVal) override;
            HRESULT STDMETHODCALLTYPE get_LastSelectedItem(IRawElementProviderSimple** retVal) override;
            HRESULT STDMETHODCALLTYPE get_CurrentSelectedItem(IRawElementProviderSimple** retVal) override;
            HRESULT STDMETHODCALLTYPE get_ItemCount(int* retVal) override;

            // ISelectionItemProvider
            HRESULT STDMETHODCALLTYPE Select() override;
            HRESULT STDMETHODCALLTYPE AddToSelection() override;
            HRESULT STDMETHODCALLTYPE RemoveFromSelection() override;
            HRESULT STDMETHODCALLTYPE get_IsSelected(BOOL* pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_SelectionContainer(IRawElementProviderSimple** pRetVal) override;

            // IScrollItemProvider
            HRESULT STDMETHODCALLTYPE ScrollIntoView() override;

            // IWindowProvider
            HRESULT STDMETHODCALLTYPE SetVisualState(WindowVisualState state) override;
            HRESULT STDMETHODCALLTYPE Close() override;
            HRESULT STDMETHODCALLTYPE WaitForInputIdle(int milliseconds, BOOL* pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_CanMaximize(BOOL* pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_CanMinimize(BOOL* pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_IsModal(BOOL* pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_WindowVisualState(WindowVisualState* pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_WindowInteractionState(WindowInteractionState* pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_IsTopmost(BOOL* pRetVal) override;

            // IGridProvider
            HRESULT STDMETHODCALLTYPE GetItem(int row, int column, IRawElementProviderSimple** pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_RowCount(int* pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_ColumnCount(int* pRetVal) override;

            // IGridItemProvider
            HRESULT STDMETHODCALLTYPE get_Row(int* pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_Column(int* pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_RowSpan(int* pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_ColumnSpan(int* pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_ContainingGrid(IRawElementProviderSimple** pRetVal) override;

            // ITableProvider
            HRESULT STDMETHODCALLTYPE GetRowHeaders(SAFEARRAY** pRetVal) override;
            HRESULT STDMETHODCALLTYPE GetColumnHeaders(SAFEARRAY** pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_RowOrColumnMajor(RowOrColumnMajor* pRetVal) override;

            // ITableItemProvider
            HRESULT STDMETHODCALLTYPE GetRowHeaderItems(SAFEARRAY** pRetVal) override;
            HRESULT STDMETHODCALLTYPE GetColumnHeaderItems(SAFEARRAY** pRetVal) override;

        private:
            // Destructor is private, the object is destroyed when its reference count reaches 0
            virtual ~UiaElementProvider() = default;

            // Returns the snapshot when the object still existed when it was taken, or fills the error code otherwise
            [[nodiscard]] std::shared_ptr<const UiaElementSnapshot> getAliveSnapshot(HRESULT& result) const;

            // Returns the object when it is still alive, or fills the error code otherwise. Only used on the gui thread.
            [[nodiscard]] AccessibleObject::Ptr getAliveObject(HRESULT& result) const;

            // Answers a request from the snapshot
            template <typename Func>
            [[nodiscard]] HRESULT readSnapshot(Func&& func) const;

            // Creates a SAFEARRAY with the given providers
            [[nodiscard]] static SAFEARRAY* createProviderArray(const std::vector<UiaElementProvider*>& providers);

            // Adds the selected items inside the snapshot (and inside its children) to the list
            static void addSelectedDescendants(const UiaElementSnapshot& snapshot, std::vector<UiaElementProvider*>& selected);

        private:
            LONG m_referenceCount = 1;
            AccessibilityBackendUIA* m_backend = nullptr; // Only used on the gui thread
            HWND m_window = nullptr;                      // Never changes, so it can be used from any thread
            std::shared_ptr<UiaActionQueue> m_actionQueue; // Shared with the backend, can be used from any thread
            unsigned int m_uniqueId = 0;                  // Never changes, so it can be used from any thread
            AccessibleObject::Ptr m_object;               // Only used on the gui thread

            mutable std::mutex m_snapshotMutex;
            std::shared_ptr<const UiaElementSnapshot> m_snapshot;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // UI Automation text pattern of an element that contains text (when hasText is set in its accessible state)
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class UiaTextProvider : public ITextProvider
        {
        public:
            // Constructor. The reference count starts at 1, which is owned by the caller.
            explicit UiaTextProvider(UiaElementProvider* element);

            UiaTextProvider(const UiaTextProvider&) = delete;
            UiaTextProvider& operator=(const UiaTextProvider&) = delete;

            // IUnknown
            HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;
            ULONG STDMETHODCALLTYPE AddRef() override;
            ULONG STDMETHODCALLTYPE Release() override;

            // ITextProvider
            HRESULT STDMETHODCALLTYPE GetSelection(SAFEARRAY** pRetVal) override;
            HRESULT STDMETHODCALLTYPE GetVisibleRanges(SAFEARRAY** pRetVal) override;
            HRESULT STDMETHODCALLTYPE RangeFromChild(IRawElementProviderSimple* childElement, ITextRangeProvider** pRetVal) override;
            HRESULT STDMETHODCALLTYPE RangeFromPoint(UiaPoint point, ITextRangeProvider** pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_DocumentRange(ITextRangeProvider** pRetVal) override;
            HRESULT STDMETHODCALLTYPE get_SupportedTextSelection(SupportedTextSelection* pRetVal) override;

        private:
            // Destructor is private, the object is destroyed when its reference count reaches 0
            virtual ~UiaTextProvider();

            // Returns the text interface when the object still exists, or fills the error code otherwise
            [[nodiscard]] std::shared_ptr<const UiaElementSnapshot> getText(HRESULT& result) const;

            // Creates a SAFEARRAY that contains a single text range
            [[nodiscard]] SAFEARRAY* createRangeArray(std::size_t start, std::size_t end) const;

        private:
            LONG m_referenceCount = 1;
            UiaElementProvider* m_element; // Holds a reference
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // UI Automation provider for a range of text inside an element that contains text.
        // The start and end of the range are indices in the UTF-32 string of the object.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class UiaTextRangeProvider : public ITextRangeProvider
        {
        public:
            // Constructor. The reference count starts at 1, which is owned by the caller.
            UiaTextRangeProvider(UiaElementProvider* element, std::size_t start, std::size_t end);

            UiaTextRangeProvider(const UiaTextRangeProvider&) = delete;
            UiaTextRangeProvider& operator=(const UiaTextRangeProvider&) = delete;

            // IUnknown
            HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;
            ULONG STDMETHODCALLTYPE AddRef() override;
            ULONG STDMETHODCALLTYPE Release() override;

            // ITextRangeProvider
            HRESULT STDMETHODCALLTYPE Clone(ITextRangeProvider** pRetVal) override;
            HRESULT STDMETHODCALLTYPE Compare(ITextRangeProvider* range, BOOL* pRetVal) override;
            HRESULT STDMETHODCALLTYPE CompareEndpoints(TextPatternRangeEndpoint endpoint,
                                                       ITextRangeProvider* targetRange,
                                                       TextPatternRangeEndpoint targetEndpoint,
                                                       int* pRetVal) override;
            HRESULT STDMETHODCALLTYPE ExpandToEnclosingUnit(TextUnit unit) override;
            HRESULT STDMETHODCALLTYPE FindAttribute(TEXTATTRIBUTEID attributeId,
                                                    VARIANT val,
                                                    BOOL backward,
                                                    ITextRangeProvider** pRetVal) override;
            HRESULT STDMETHODCALLTYPE FindText(BSTR text, BOOL backward, BOOL ignoreCase, ITextRangeProvider** pRetVal) override;
            HRESULT STDMETHODCALLTYPE GetAttributeValue(TEXTATTRIBUTEID attributeId, VARIANT* pRetVal) override;
            HRESULT STDMETHODCALLTYPE GetBoundingRectangles(SAFEARRAY** pRetVal) override;
            HRESULT STDMETHODCALLTYPE GetEnclosingElement(IRawElementProviderSimple** pRetVal) override;
            HRESULT STDMETHODCALLTYPE GetText(int maxLength, BSTR* pRetVal) override;
            HRESULT STDMETHODCALLTYPE Move(TextUnit unit, int count, int* pRetVal) override;
            HRESULT STDMETHODCALLTYPE MoveEndpointByUnit(TextPatternRangeEndpoint endpoint, TextUnit unit, int count, int* pRetVal) override;
            HRESULT STDMETHODCALLTYPE MoveEndpointByRange(TextPatternRangeEndpoint endpoint,
                                                          ITextRangeProvider* targetRange,
                                                          TextPatternRangeEndpoint targetEndpoint) override;
            HRESULT STDMETHODCALLTYPE Select() override;
            HRESULT STDMETHODCALLTYPE AddToSelection() override;
            HRESULT STDMETHODCALLTYPE RemoveFromSelection() override;
            HRESULT STDMETHODCALLTYPE ScrollIntoView(BOOL alignToTop) override;
            HRESULT STDMETHODCALLTYPE GetChildren(SAFEARRAY** pRetVal) override;

        private:
            // Destructor is private, the object is destroyed when its reference count reaches 0
            virtual ~UiaTextRangeProvider();

            // Returns the text interface when the object still exists, or fills the error code otherwise
            [[nodiscard]] std::shared_ptr<const UiaElementSnapshot> getText(HRESULT& result) const;

            // Returns the range when the given provider is a range in the same text, or a nullptr otherwise
            [[nodiscard]] const UiaTextRangeProvider* getSameTextRange(ITextRangeProvider* range) const;

            // Finds the start of the unit that contains the position, and the start of the next unit
            [[nodiscard]] static std::pair<std::size_t, std::size_t> findUnit(const UiaElementSnapshot& text, std::size_t pos, TextUnit unit);

            // Moves the position by the amount of units, returns how many units it actually moved
            [[nodiscard]] static int movePosition(const UiaElementSnapshot& text, std::size_t& pos, TextUnit unit, int count);

            // Makes certain that start and end are within the text and that start is not behind end
            void clampEndpoints(std::size_t textLength);

        private:
            LONG m_referenceCount = 1;
            UiaElementProvider* m_element; // Holds a reference
            std::size_t m_start;
            std::size_t m_end;
        };
    } // namespace priv
} // namespace tgui

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_UIA_PROVIDERS_HPP
