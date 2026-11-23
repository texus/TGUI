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
#include <TGUI/Backend/Window/BackendGui.hpp>

#include <commctrl.h>
#include <tchar.h>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        // How often the snapshots are updated at most while the gui keeps being redrawn, to pick up changes without a signal
        // (e.g. a list box that was scrolled)
        static constexpr std::chrono::milliseconds snapshotUpdateInterval{100};

        // How long to wait before reporting the focus again when the gui was attached to a window that already had focus
        static constexpr std::chrono::milliseconds focusReannounceDelay{1000};

        // How long to wait before reporting the focus on the window again after the gui was removed from it
        static constexpr UINT focusAfterDetachDelay = 600;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Tells clients to look at the focus again after the gui was removed from a window that still exists and has focus.
        // A screen reader may remember for a short while that the window used UI Automation (NVDA does so for 0.5s) and then
        // ignore the event, so it is repeated after a delay. A timer with a callback is used for this, as the gui no longer
        // exists by then. The callback is executed when the window library dispatches the timer message.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void CALLBACK focusAfterDetachTimerProc(HWND window, UINT, UINT_PTR timerId, DWORD)
        {
            KillTimer(window, timerId);

            // Nothing is reported when the window lost the focus in the meantime
            if (GetFocus() == window)
                NotifyWinEvent(EVENT_OBJECT_FOCUS, window, OBJID_CLIENT, CHILDID_SELF);
        }

        static void reportWindowFocusAfterDetach(HWND window)
        {
            NotifyWinEvent(EVENT_OBJECT_FOCUS, window, OBJID_CLIENT, CHILDID_SELF);

            // The address of the callback is used as timer id, so that it doesn't conflict with timers of the application
            SetTimer(window, reinterpret_cast<UINT_PTR>(&focusAfterDetachTimerProc), focusAfterDetachDelay, &focusAfterDetachTimerProc);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Tells UI Automation that a provider will no longer be used. UiaDisconnectProvider only exists since Windows 8,
        // so it is looked up at runtime. Without it, clients keep the provider until they release it, and the provider
        // returns UIA_E_ELEMENTNOTAVAILABLE in the meantime.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void disconnectProvider(IRawElementProviderSimple* provider)
        {
            using UiaDisconnectProviderFunc = HRESULT(WINAPI*)(IRawElementProviderSimple*);
            static const UiaDisconnectProviderFunc disconnectProviderFunc = []
            {
                // The library is already loaded, as we link to it for the functions that exist on all supported Windows versions
                const HMODULE moduleHandle = GetModuleHandle(TEXT("uiautomationcore.dll"));
                if (!moduleHandle)
                    return UiaDisconnectProviderFunc{nullptr};

#if defined(__clang__)
    #pragma clang diagnostic push
    #if defined(__clang_major__) && (__clang_major__ >= 19)
        #pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
    #endif
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-function-type"
#endif
                return reinterpret_cast<UiaDisconnectProviderFunc>(GetProcAddress(moduleHandle, "UiaDisconnectProvider"));
#if defined(__clang__)
    #pragma clang diagnostic pop
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif
            }();

            if (disconnectProviderFunc)
                disconnectProviderFunc(provider);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        UiaActionQueue::UiaActionQueue(HWND window) :
            m_window{window}
        {
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void UiaActionQueue::push(std::function<void()> action)
        {
            {
                const std::lock_guard<std::mutex> lock{m_mutex};
                m_actions.push_back(std::move(action));
            }

            // Backends that wait for events (e.g. GLFW) only update the gui when a message arrives
            PostMessage(m_window, WM_NULL, 0, 0);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<std::function<void()>> UiaActionQueue::takeAll()
        {
            const std::lock_guard<std::mutex> lock{m_mutex};
            return std::move(m_actions);
        }


        std::unique_ptr<AccessibilityBackendUIA> AccessibilityBackendUIA::create(BackendGui& gui, HWND window)
        {
            if (!window || !IsWindow(window))
                return nullptr;

            auto backend = std::make_unique<AccessibilityBackendUIA>(gui, window);

            // The backend is used as the subclass id, so that multiple guis can be attached to the same window
            const auto subclassId = reinterpret_cast<UINT_PTR>(backend.get());
            if (!SetWindowSubclass(window, &AccessibilityBackendUIA::subclassProc, subclassId, reinterpret_cast<DWORD_PTR>(backend.get())))
                return nullptr;

            backend->m_subclassed = true;

            // UI Automation clients that registered for events before the window was shown only receive the events of the
            // window if it had a provider at the time that it was shown. The window is usually shown before the gui is
            // attached to it, so we tell the clients that the window was shown again now that it does have a provider.
            if (IsWindowVisible(window))
                NotifyWinEvent(EVENT_OBJECT_SHOW, window, OBJID_WINDOW, CHILDID_SELF);

            return backend;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibilityBackendUIA::AccessibilityBackendUIA(BackendGui& gui, HWND window) :
            AccessibilityBackend{gui},
            m_window{window},
            m_actionQueue{std::make_shared<UiaActionQueue>(window)}
        {
            // The window might already have focus when the backend is created, later changes arrive via the subclass.
            // A child window (e.g. one that a window library created for text input) can have focus on behalf of our window.
            const HWND focusedWindow = GetFocus();
            m_windowFocused = focusedWindow && ((focusedWindow == window) || IsChild(window, focusedWindow));

            // The window is usually shown before the gui is attached to it. A screen reader that already looked at the window
            // concluded that it has no UI Automation support and may remember this for a short while (NVDA does so for 0.5s),
            // ignoring our focus events. So the focus is reported again once the screen reader will have looked again.
            if (m_windowFocused)
            {
                m_focusReannounceTime = std::chrono::steady_clock::now() + focusReannounceDelay;
                m_focusReannouncePending = true;
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibilityBackendUIA::~AccessibilityBackendUIA()
        {
            // The window is a nullptr here when it was destroyed before the gui
            const HWND window = m_window;
            const bool windowFocused = m_windowFocused;

            disconnectProviders();
            detachFromWindow();

            // When the gui is destroyed while its window remains (e.g. a game that closes its menu), clients still consider
            // the focus to be on an object of the gui. They are told to look at the focus again, which is now the window.
            if (window && windowFocused)
                reportWindowFocusAfterDetach(window);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        UiaElementProvider* AccessibilityBackendUIA::getProvider(const AccessibleObject::Ptr& object)
        {
            if (!object)
                return nullptr;

            UiaElementProvider* provider;
            const auto it = m_providers.find(object.get());
            if (it != m_providers.end())
                provider = it->second;
            else
            {
                // The new provider starts with a reference count of 1, which is owned by the map
                provider = new UiaElementProvider(*this, object);
                m_providers[object.get()] = provider;

                // The snapshot is created after the provider was added to the map, as it requests the providers of the
                // parent and children, which in turn request the provider of this object
                provider->updateSnapshot();
            }

            provider->AddRef();
            return provider;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        UiaRect AccessibilityBackendUIA::mapGuiToScreen(const FloatRect& rect) const
        {
            if (!m_window)
                return {};

            POINT windowOrigin = {0, 0};
            if (!ClientToScreen(m_window, &windowOrigin))
                return {};

            const BackendGui& gui = m_tree.getGui();
            const Vector2f topLeft = gui.mapCoordsToPixel(rect.getPosition());
            const Vector2f bottomRight = gui.mapCoordsToPixel(rect.getPosition() + rect.getSize());

            UiaRect screenRect;
            screenRect.left = static_cast<double>(windowOrigin.x) + std::round(topLeft.x);
            screenRect.top = static_cast<double>(windowOrigin.y) + std::round(topLeft.y);
            screenRect.width = std::max(0.0, static_cast<double>(std::round(bottomRight.x - topLeft.x)));
            screenRect.height = std::max(0.0, static_cast<double>(std::round(bottomRight.y - topLeft.y)));
            return screenRect;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Vector2f AccessibilityBackendUIA::mapScreenToGui(double x, double y) const
        {
            if (!m_window)
                return {};

            POINT windowOrigin = {0, 0};
            if (!ClientToScreen(m_window, &windowOrigin))
                return {};

            const Vector2i pixel{static_cast<int>(std::floor(x)) - windowOrigin.x, static_cast<int>(std::floor(y)) - windowOrigin.y};
            return m_tree.getGui().mapPixelToCoords(pixel);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibilityBackendUIA::hasWindowFocus() const
        {
            return m_window && m_windowFocused;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String AccessibilityBackendUIA::getWindowTitle() const
        {
            if (!m_window)
                return {};

            const int length = GetWindowTextLength(m_window);
            if (length <= 0)
                return {};

            std::vector<TCHAR> buffer(static_cast<std::size_t>(length) + 1);
            const int copied = GetWindowText(m_window, buffer.data(), length + 1);
            return String{std::basic_string<TCHAR>(buffer.data(), static_cast<std::size_t>(std::max(copied, 0)))};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibilityBackendUIA::update()
        {
            // Execute the actions that UI Automation requested since the last update
            const auto actions = m_actionQueue->takeAll();
            for (const auto& action : actions)
                action();

            const auto now = std::chrono::steady_clock::now();
            bool reannounceWindowFocus = false;
            if (m_focusReannouncePending && (now >= m_focusReannounceTime))
            {
                m_focusReannouncePending = false;
                reannounceWindowFocus = true;
                m_tree.invalidateFocus();
            }

            // The objects that the events are about get their snapshots updated by the tree before the events are reported.
            // Changes without a signal (e.g. scrolling) and the effects of actions are picked up by updating all snapshots,
            // which is only done after the gui was redrawn so that an idle gui doesn't use any processing time.
            if (!actions.empty() || (m_snapshotsOutdated && (now - m_lastSnapshotUpdateTime >= snapshotUpdateInterval)))
            {
                m_lastSnapshotUpdateTime = now;
                m_snapshotsOutdated = false;
                updateSnapshots();
            }

            const bool redrawRequested = m_tree.flushEvents(true);

            // A client that connected to the window before it had a provider doesn't receive the focus events that we raise
            // ourselves. The focus is also reported as if the window just received focus, which makes UI Automation ask the
            // root for its focused element, just like when the user switches to another window and back.
            // This is done after flushing the events, so that the root already knows which element has focus.
            if (reannounceWindowFocus && m_window && m_windowFocused)
                NotifyWinEvent(EVENT_OBJECT_FOCUS, m_window, OBJID_CLIENT, CHILDID_SELF);

            return redrawRequested || !actions.empty();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibilityBackendUIA::waitForEvents(Duration timeout)
        {
            // Messages that are send from other threads (e.g. WM_GETOBJECT) are only delivered while the thread is pumping
            // messages, so a plain sleep would make the screen reader wait. MWMO_INPUTAVAILABLE returns immediately when
            // messages are already in the queue, even if they were seen by an earlier PeekMessage call.
            // The timeout is rounded up, as rounding it down to 0 would keep the main loop busy until a timer expires
            const auto milliseconds = std::chrono::ceil<std::chrono::milliseconds>(std::chrono::nanoseconds(timeout));
            MsgWaitForMultipleObjectsEx(0,
                                        nullptr,
                                        static_cast<DWORD>(std::max<long long>(milliseconds.count(), 0)),
                                        QS_ALLINPUT,
                                        MWMO_INPUTAVAILABLE);
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibilityBackendUIA::accessibleObjectsChanged(const std::vector<AccessibleObject::Ptr>& objects)
        {
            // Objects without a provider were never requested by UI Automation, so they have no snapshot to update
            for (const auto& object : objects)
            {
                const auto it = m_providers.find(object.get());
                if (it == m_providers.end())
                    continue;

                // Updating the snapshot can create new providers, which could invalidate the iterator
                UiaElementProvider* provider = it->second;
                provider->AddRef();
                provider->updateSnapshot();
                provider->Release();
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibilityBackendUIA::updateSnapshots()
        {
            // Updating a snapshot can create new providers, so the providers are copied before updating them
            std::vector<UiaElementProvider*> providers;
            providers.reserve(m_providers.size());
            for (const auto& pair : m_providers)
            {
                pair.second->AddRef();
                providers.push_back(pair.second);
            }

            for (UiaElementProvider* provider : providers)
            {
                provider->updateSnapshot();
                provider->Release();
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibilityBackendUIA::accessibleEventOccurred(AccessibleEvent event,
                                                              const AccessibleObject::Ptr& object,
                                                              AccessibleProperty property)
        {
            if (event == AccessibleEvent::ObjectRemoved)
            {
                // Clients that still hold a reference to the provider will get UIA_E_ELEMENTNOTAVAILABLE from now on
                const auto it = m_providers.find(object.get());
                if (it == m_providers.end())
                    return;

                UiaElementProvider* provider = it->second;
                m_providers.erase(it);
                provider->disconnect();
                disconnectProvider(provider);
                provider->Release();
                return;
            }

            if (!m_window)
                return;

            UiaElementProvider* provider = getProvider(object);
            if (!provider)
                return;

            switch (event)
            {
                case AccessibleEvent::FocusChanged:
                    // The focus can only be inside our window when the window itself has focus
                    if (hasWindowFocus())
                        UiaRaiseAutomationEvent(provider, UIA_AutomationFocusChangedEventId);
                    break;
                case AccessibleEvent::PropertyChanged:
                    raisePropertyChangedEvent(*provider, property);
                    break;
                case AccessibleEvent::ChildrenChanged:
                {
                    int runtimeId[] = {UiaAppendRuntimeId, static_cast<int>(object->getUniqueId())};
                    UiaRaiseStructureChangedEvent(provider, StructureChangeType_ChildrenInvalidated, runtimeId, 2);
                    break;
                }
                case AccessibleEvent::TextChanged:
                    UiaRaiseAutomationEvent(provider, UIA_Text_TextChangedEventId);
                    break;
                case AccessibleEvent::TextSelectionChanged:
                    UiaRaiseAutomationEvent(provider, UIA_Text_TextSelectionChangedEventId);
                    break;
                case AccessibleEvent::ElementSelected:
                    UiaRaiseAutomationEvent(provider, UIA_SelectionItem_ElementSelectedEventId);
                    break;
                case AccessibleEvent::WindowOpened:
                    UiaRaiseAutomationEvent(provider, UIA_Window_WindowOpenedEventId);
                    break;
                case AccessibleEvent::WindowClosed:
                    UiaRaiseAutomationEvent(provider, UIA_Window_WindowClosedEventId);
                    break;
                case AccessibleEvent::MenuOpened:
                    UiaRaiseAutomationEvent(provider, UIA_MenuOpenedEventId);
                    break;
                case AccessibleEvent::MenuClosed:
                    UiaRaiseAutomationEvent(provider, UIA_MenuClosedEventId);
                    break;
                case AccessibleEvent::ObjectRemoved: // Already handled above
                    break;
            }

            provider->Release();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        LRESULT CALLBACK AccessibilityBackendUIA::subclassProc(HWND hWnd,
                                                               UINT message,
                                                               WPARAM wParam,
                                                               LPARAM lParam,
                                                               UINT_PTR,
                                                               DWORD_PTR refData)
        {
            auto* backend = reinterpret_cast<AccessibilityBackendUIA*>(refData);

            switch (message)
            {
                case WM_GETOBJECT:
                {
                    // The object id is a DWORD that is send in the lParam, so it has to be truncated to compare it
                    if (static_cast<LONG>(static_cast<DWORD>(lParam)) != UiaRootObjectId)
                        break;

                    // Exceptions must never propagate through the window procedure
                    UiaElementProvider* rootProvider = nullptr;
                    try
                    {
                        rootProvider = backend->getProvider(backend->m_tree.getRoot());
                    }
                    catch (...)
                    {
                        rootProvider = nullptr;
                    }

                    if (!rootProvider)
                        break;

                    const LRESULT result = UiaReturnRawElementProvider(hWnd, wParam, lParam, rootProvider);
                    rootProvider->Release();
                    return result;
                }
                case WM_SETFOCUS:
                case WM_KILLFOCUS:
                {
                    backend->m_windowFocused = (message == WM_SETFOCUS);

                    // The focused widget is reported again when the window gets focus back. Reporting it also updates its
                    // snapshot, which tells whether the widget has keyboard focus and thus depends on the focus of the window.
                    backend->m_tree.invalidateFocus();
                    break;
                }
                case WM_MOVE:
                {
                    // The snapshots contain screen coordinates, which change when the window moves without the gui being redrawn
                    backend->m_snapshotsOutdated = true;
                    break;
                }
                case WM_SETTEXT:
                {
                    // The root reports the window title as its name. Windows tells clients about the new title, so the snapshot
                    // of the root has to contain it before they ask for it.
                    const LRESULT result = DefSubclassProc(hWnd, message, wParam, lParam);
                    backend->accessibleObjectsChanged({backend->m_tree.getRoot()});
                    return result;
                }
                case WM_NCDESTROY:
                {
                    // The window is being destroyed, so the providers can no longer be used
                    backend->disconnectProviders();
                    backend->detachFromWindow();
                    return DefSubclassProc(hWnd, message, wParam, lParam);
                }
                default:
                    break;
            }

            return DefSubclassProc(hWnd, message, wParam, lParam);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibilityBackendUIA::disconnectProviders()
        {
            auto providers = std::move(m_providers);
            m_providers.clear();
            for (auto& pair : providers)
            {
                UiaElementProvider* provider = pair.second;
                provider->disconnect();
                disconnectProvider(provider);
                provider->Release();
            }

            // All providers are already disconnected, so the removal of the objects won't do anything anymore
            m_tree.clear();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibilityBackendUIA::detachFromWindow()
        {
            if (!m_window)
                return;

            // Tell UI Automation that the window no longer provides elements, so that it releases its references
            UiaReturnRawElementProvider(m_window, 0, 0, nullptr);

            if (m_subclassed)
            {
                RemoveWindowSubclass(m_window, &AccessibilityBackendUIA::subclassProc, reinterpret_cast<UINT_PTR>(this));
                m_subclassed = false;
            }

            m_window = nullptr;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibilityBackendUIA::raisePropertyChangedEvent(UiaElementProvider& provider, AccessibleProperty property)
        {
            PROPERTYID propertyId = 0;
            switch (property)
            {
                case AccessibleProperty::Name:
                    propertyId = UIA_NamePropertyId;
                    break;
                case AccessibleProperty::Description:
                    propertyId = UIA_FullDescriptionPropertyId;
                    break;
                case AccessibleProperty::HelpText:
                    propertyId = UIA_HelpTextPropertyId;
                    break;
                case AccessibleProperty::Value:
                    propertyId = UIA_ValueValuePropertyId;
                    break;
                case AccessibleProperty::RangeValue:
                    propertyId = UIA_RangeValueValuePropertyId;
                    break;
                case AccessibleProperty::ToggleState:
                    propertyId = UIA_ToggleToggleStatePropertyId;
                    break;
                case AccessibleProperty::ExpandState:
                    propertyId = UIA_ExpandCollapseExpandCollapseStatePropertyId;
                    break;
                case AccessibleProperty::Selected:
                    propertyId = UIA_SelectionItemIsSelectedPropertyId;
                    break;
                case AccessibleProperty::Enabled:
                    propertyId = UIA_IsEnabledPropertyId;
                    break;
                case AccessibleProperty::Offscreen:
                    propertyId = UIA_IsOffscreenPropertyId;
                    break;
                case AccessibleProperty::ReadOnly:
                    propertyId = UIA_ValueIsReadOnlyPropertyId;
                    break;
                case AccessibleProperty::Bounds:
                    propertyId = UIA_BoundingRectanglePropertyId;
                    break;
            }

            // Clients read the new value from the provider, but we pass it along anyway for clients that do use it
            const auto snapshot = provider.getSnapshot();
            const auto raiseEvent = [&provider, &snapshot](PROPERTYID id)
            {
                VARIANT oldValue;
                VARIANT newValue;
                VariantInit(&oldValue);
                VariantInit(&newValue);
                if (snapshot)
                    UiaElementProvider::getPropertyValue(*snapshot, id, newValue);
                UiaRaiseAutomationPropertyChangedEvent(&provider, id, oldValue, newValue);
                VariantClear(&newValue);
            };

            raiseEvent(propertyId);

            // NVDA reads the full description, but only listens to changes of the help text to know that the description changed
            if (property == AccessibleProperty::Description)
                raiseEvent(UIA_HelpTextPropertyId);

            // Screen readers don't listen to changes of the selected property, items in a list in which multiple items can be
            // selected report the change with an event instead. A single selection is reported with ElementSelected.
            if ((property == AccessibleProperty::Selected) && snapshot && snapshot->parent)
            {
                const auto parentSnapshot = snapshot->parent->getSnapshot();
                if (parentSnapshot && parentSnapshot->state.multipleSelection)
                {
                    UiaRaiseAutomationEvent(&provider,
                                            snapshot->state.selected ? UIA_SelectionItem_ElementAddedToSelectionEventId
                                                                     : UIA_SelectionItem_ElementRemovedFromSelectionEventId);
                }
            }
        }
    } // namespace priv
} // namespace tgui
