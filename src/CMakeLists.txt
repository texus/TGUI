####################################################################################################
# TGUI - Texus' Graphical User Interface
# Copyright (C) 2012-2025 Bruno Van de Velde (vdv_b@tgui.eu)
#
# This software is provided 'as-is', without any express or implied warranty.
# In no event will the authors be held liable for any damages arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it freely,
# subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented;
#    you must not claim that you wrote the original software.
#    If you use this software in a product, an acknowledgment
#    in the product documentation would be appreciated but is not required.
#
# 2. Altered source versions must be plainly marked as such,
#    and must not be misrepresented as being the original software.
#
# 3. This notice may not be removed or altered from any source distribution.
####################################################################################################

set(TGUI_SRC
    Animation.cpp
    Base64.cpp
    Color.cpp
    Components.cpp
    Container.cpp
    Cursor.cpp
    CustomWidgetForBindings.cpp
    FileDialogIconLoader.cpp
    Filesystem.cpp
    Font.cpp
    Global.cpp
    Layout.cpp
    ObjectConverter.cpp
    Sprite.cpp
    Signal.cpp
    String.cpp
    SignalManager.cpp
    SubwidgetContainer.cpp
    SvgImage.cpp
    TextStyle.cpp
    Text.cpp
    Texture.cpp
    TextureManager.cpp
    Timer.cpp
    ToolTip.cpp
    Transform.cpp
    TwoFingerScrollDetect.cpp
    Widget.cpp
    WindowsIMM.cpp
    Backend/Font/BackendFont.cpp
    Backend/Renderer/BackendRenderTarget.cpp
    Backend/Renderer/BackendText.cpp
    Backend/Renderer/BackendTexture.cpp
    Backend/Window/Backend.cpp
    Backend/Window/BackendGui.cpp
    Loading/DataIO.cpp
    Loading/Deserializer.cpp
    Loading/ImageLoader.cpp
    Loading/Serializer.cpp
    Loading/Theme.cpp
    Loading/ThemeLoader.cpp
    Loading/WidgetFactory.cpp
    Renderers/BoxLayoutRenderer.cpp
    Renderers/ButtonRenderer.cpp
    Renderers/ChatBoxRenderer.cpp
    Renderers/ChildWindowRenderer.cpp
    Renderers/ColorPickerRenderer.cpp
    Renderers/ComboBoxRenderer.cpp
    Renderers/EditBoxRenderer.cpp
    Renderers/FileDialogRenderer.cpp
    Renderers/GroupRenderer.cpp
    Renderers/KnobRenderer.cpp
    Renderers/LabelRenderer.cpp
    Renderers/ListBoxRenderer.cpp
    Renderers/ListViewRenderer.cpp
    Renderers/MenuBarRenderer.cpp
    Renderers/MenuWidgetBaseRenderer.cpp
    Renderers/MessageBoxRenderer.cpp
    Renderers/PanelRenderer.cpp
    Renderers/PanelListBoxRenderer.cpp
    Renderers/PictureRenderer.cpp
    Renderers/ProgressBarRenderer.cpp
    Renderers/RadioButtonRenderer.cpp
    Renderers/RangeSliderRenderer.cpp
    Renderers/ScrollablePanelRenderer.cpp
    Renderers/ScrollbarRenderer.cpp
    Renderers/SeparatorLineRenderer.cpp
    Renderers/SliderRenderer.cpp
    Renderers/SpinButtonRenderer.cpp
    Renderers/SplitContainerRenderer.cpp
    Renderers/TabsRenderer.cpp
    Renderers/TextAreaRenderer.cpp
    Renderers/TreeViewRenderer.cpp
    Renderers/WidgetRenderer.cpp
    Widgets/BitmapButton.cpp
    Widgets/BoxLayout.cpp
    Widgets/BoxLayoutRatios.cpp
    Widgets/Button.cpp
    Widgets/ButtonBase.cpp
    Widgets/CanvasBase.cpp
    Widgets/ChatBox.cpp
    Widgets/CheckBox.cpp
    Widgets/ChildWindow.cpp
    Widgets/ClickableWidget.cpp
    Widgets/ColorPicker.cpp
    Widgets/ComboBox.cpp
    Widgets/ContextMenu.cpp
    Widgets/EditBox.cpp
    Widgets/EditBoxSlider.cpp
    Widgets/FileDialog.cpp
    Widgets/Group.cpp
    Widgets/Grid.cpp
    Widgets/GrowHorizontalLayout.cpp
    Widgets/GrowVerticalLayout.cpp
    Widgets/HorizontalLayout.cpp
    Widgets/HorizontalWrap.cpp
    Widgets/Knob.cpp
    Widgets/Label.cpp
    Widgets/ListBox.cpp
    Widgets/ListView.cpp
    Widgets/MenuBar.cpp
    Widgets/MenuWidgetBase.cpp
    Widgets/MessageBox.cpp
    Widgets/Panel.cpp
    Widgets/PanelListBox.cpp
    Widgets/Picture.cpp
    Widgets/ProgressBar.cpp
    Widgets/RadioButton.cpp
    Widgets/RadioButtonGroup.cpp
    Widgets/RangeSlider.cpp
    Widgets/RichTextLabel.cpp
    Widgets/ScrollablePanel.cpp
    Widgets/Scrollbar.cpp
    Widgets/SeparatorLine.cpp
    Widgets/Slider.cpp
    Widgets/SpinButton.cpp
    Widgets/SpinControl.cpp
    Widgets/SplitContainer.cpp
    Widgets/Tabs.cpp
    Widgets/TabContainer.cpp
    Widgets/TextArea.cpp
    Widgets/ToggleButton.cpp
    Widgets/TreeView.cpp
    Widgets/VerticalLayout.cpp
)

set(TGUI_HEADERS
    "${PROJECT_SOURCE_DIR}/include/TGUI/AbsoluteOrRelativeValue.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/AllWidgets.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Animation.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Any.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Base64.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Color.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Components.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Config.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Container.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/CopiedSharedPtr.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Core.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Cursor.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/CustomWidgetForBindings.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/DefaultBackendWindow.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/DefaultFont.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Duration.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Event.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Exception.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/FileDialogIconLoader.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Filesystem.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Font.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Global.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Keyboard.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Layout.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/ObjectConverter.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Optional.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Outline.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Rect.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/RelFloatRect.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/RendererDefines.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/RenderStates.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Signal.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/SignalManager.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Sprite.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/String.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/StringView.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/SubwidgetContainer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/SvgImage.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Text.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/TextStyle.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/TextureData.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Texture.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/TextureManager.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/TGUI.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Timer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/ToolTip.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Transform.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/TwoFingerScrollDetect.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Utf.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Variant.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Vector2.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Vertex.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widget.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/WindowsIMM.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/raylib.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/SFML-OpenGL3.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/SDL-TTF-OpenGL3.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/SDL-GPU.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/SDL-Renderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/GLFW-OpenGL3.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/GLFW-GLES2.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/SDL-GLES2.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/SDL-TTF-GLES2.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/SDL-OpenGL3.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/SFML-Graphics.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/BackendTexture.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/BackendText.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/BackendRenderTarget.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/BackendRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/OpenGL.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/GLES2/CanvasGLES2.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/GLES2/BackendTextureGLES2.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/GLES2/BackendRendererGLES2.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/GLES2/BackendRenderTargetGLES2.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/SDL_GPU/BackendTextureSDLGPU.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/SDL_GPU/CanvasSDLGPU.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/SDL_GPU/BackendRendererSDLGPU.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/SDL_GPU/BackendRenderTargetSDLGPU.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/SDL_Renderer/BackendTextureSDL.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/SDL_Renderer/CanvasSDL.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/SDL_Renderer/BackendRendererSDL.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/SDL_Renderer/BackendRenderTargetSDL.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/SFML-Graphics/BackendRenderTargetSFML.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/SFML-Graphics/BackendRendererSFML.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/SFML-Graphics/BackendTextureSFML.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/SFML-Graphics/CanvasSFML.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/Raylib/BackendTextureRaylib.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/Raylib/BackendRendererRaylib.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/Raylib/CanvasRaylib.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/Raylib/BackendRenderTargetRaylib.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/OpenGL3/BackendRendererOpenGL3.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/OpenGL3/CanvasOpenGL3.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/OpenGL3/BackendTextureOpenGL3.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Renderer/OpenGL3/BackendRenderTargetOpenGL3.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Window/BackendGui.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Window/Backend.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Window/Raylib/BackendRaylib.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Window/Raylib/BackendGuiRaylib.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Window/SDL/BackendSDL.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Window/SDL/BackendGuiSDL.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Window/SFML/BackendGuiSFML.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Window/SFML/BackendSFML.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Window/GLFW/BackendGLFW.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Window/GLFW/BackendGuiGLFW.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Font/BackendFontFactory.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Font/BackendFont.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Font/SDL_ttf/BackendFontSDLttf.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Font/SFML-Graphics/BackendFontSFML.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Font/Raylib/BackendFontRaylib.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Backend/Font/FreeType/BackendFontFreeType.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Loading/DataIO.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Loading/Deserializer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Loading/ImageLoader.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Loading/Serializer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Loading/Theme.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Loading/ThemeLoader.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Loading/WidgetFactory.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/BoxLayoutRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/ButtonRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/ChatBoxRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/CheckBoxRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/ChildWindowRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/ColorPickerRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/ComboBoxRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/ContextMenuRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/EditBoxRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/FileDialogRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/GroupRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/KnobRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/LabelRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/ListBoxRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/ListViewRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/MenuBarRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/MenuWidgetBaseRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/MessageBoxRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/PanelListBoxRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/PanelRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/PictureRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/ProgressBarRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/RadioButtonRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/RangeSliderRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/ScrollablePanelRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/ScrollbarRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/SeparatorLineRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/SliderRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/SpinButtonRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/SplitContainerRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/TabsRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/TextAreaRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/TextBoxRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/TreeViewRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Renderers/WidgetRenderer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/BitmapButton.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/BoxLayout.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/BoxLayoutRatios.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/ButtonBase.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/Button.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/CanvasBase.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/ChatBox.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/CheckBox.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/ChildWindow.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/ClickableWidget.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/ColorPicker.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/ComboBox.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/ContextMenu.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/EditBox.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/EditBoxSlider.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/FileDialog.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/Grid.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/Group.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/GrowHorizontalLayout.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/GrowVerticalLayout.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/HorizontalLayout.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/HorizontalWrap.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/Knob.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/Label.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/ListBox.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/ListView.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/MenuBar.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/MenuWidgetBase.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/MessageBox.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/Panel.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/PanelListBox.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/Picture.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/ProgressBar.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/RadioButtonGroup.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/RadioButton.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/RangeSlider.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/RichTextLabel.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/ScrollablePanel.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/Scrollbar.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/SeparatorLine.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/Slider.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/SpinButton.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/SpinControl.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/SplitContainer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/TabContainer.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/Tabs.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/TextArea.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/ToggleButton.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/TreeView.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/Widgets/VerticalLayout.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/extlibs/IncludeNanoSVG.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/extlibs/IncludeSDL.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/extlibs/IncludeStbImage.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/extlibs/IncludeStbImageWrite.hpp"
    "${PROJECT_SOURCE_DIR}/include/TGUI/extlibs/IncludeWindows.hpp"

)

set(TGUI_SRC_WITHOUT_UNITY "")
if(TGUI_OS_WINDOWS)
    list(APPEND TGUI_SRC_WITHOUT_UNITY FileDialogIconLoaderWindows.cpp)
elseif(TGUI_OS_LINUX)
    list(APPEND TGUI_SRC_WITHOUT_UNITY FileDialogIconLoaderLinux.cpp)
endif()

# Bundle multiple files together when performing a unity build (useful for CI to speed up compilation).
# Note that this creates new source files which causes CMake to no longer detect changed to the original source files,
# so this should only be enabled in places where the TGUI is build only once.
# For CMake 3.18 and newer, TGUI supports the CMAKE_UNITY_BUILD option and this code should no longer be used.
if(TGUI_OPTIMIZE_SINGLE_BUILD)
    if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.18")
        # The only reason this code still exists is so that the CI can continue to do unity builds with an
        # older CMake version. Once the CMake version is updated, we should fail here to prevent keeping the old code.
        message(FATAL_ERROR "TGUI_OPTIMIZE_SINGLE_BUILD has been replaced with CMAKE_UNITY_BUILD")
    endif()
    list(LENGTH TGUI_SRC fileCount)
    if(TGUI_OPTIMIZE_SINGLE_BUILD_THREADS)
        set(threads ${TGUI_OPTIMIZE_SINGLE_BUILD_THREADS})
    else()
        include(ProcessorCount)
        ProcessorCount(threads)
    endif()

    if(${threads} LESS ${fileCount})
        math(EXPR fileCountMinusOne "${fileCount} - 1")
        math(EXPR threadsMinusOne "${threads} - 1")

        set(NEW_TGUI_SRC "")
        foreach(i RANGE 0 ${threadsMinusOne})
            set(OutputFile "${CMAKE_CURRENT_SOURCE_DIR}/CombinedSources-${i}.cpp")
            file(WRITE "${OutputFile}" "")
            foreach(j RANGE ${i} ${fileCountMinusOne} ${threads})
                list(GET TGUI_SRC ${j} inputFile)
                file(READ ${inputFile} CONTENTS)
                file(APPEND "${OutputFile}" "${CONTENTS}")
                list(APPEND NEW_TGUI_SRC "${OutputFile}")
            endforeach()
        endforeach()

        set(TGUI_SRC ${NEW_TGUI_SRC})
    endif()
endif()

if (TGUI_BUILD_AS_CXX_MODULE)
    file(READ "TGUI-Main-Module.cppm.in" file_contents)
    foreach(source_file ${TGUI_SRC})
        string(APPEND file_contents "#include \"${CMAKE_CURRENT_SOURCE_DIR}/${source_file}\"\n")
    endforeach()
    foreach(source_file ${TGUI_SRC_WITHOUT_UNITY})
        string(APPEND file_contents "#include \"${CMAKE_CURRENT_SOURCE_DIR}/${source_file}\"\n")
    endforeach()
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/module_tgui.cppm" "${file_contents}")
    set(TGUI_SRC "${CMAKE_CURRENT_BINARY_DIR}/module_tgui.cppm")
    set(TGUI_SRC_WITHOUT_UNITY "")
endif()

if (TGUI_BUILD_AS_CXX_MODULE)
    tgui_create_module_from_sources(DefaultBackendWindow.cpp "tgui.default_backend_window")
    list(APPEND TGUI_SRC_WITHOUT_UNITY "${module_source}")
else()
    list(APPEND TGUI_SRC_WITHOUT_UNITY DefaultBackendWindow.cpp)
endif()

# Determine library suffixes depending on static/shared configuration
if(TGUI_SHARED_LIBS)
    add_library(tgui SHARED)
    set_target_properties(tgui PROPERTIES DEFINE_SYMBOL TGUI_EXPORTS)
    set_target_properties(tgui PROPERTIES DEBUG_POSTFIX -d)

    # Set the version and soversion of the target (for compatible systems -- mostly Linuxes)
    # Except for Android which strips soversion suffixes
    if(NOT TGUI_OS_ANDROID)
        set_target_properties(tgui PROPERTIES SOVERSION ${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR}.${TGUI_VERSION_PATCH})
        set_target_properties(tgui PROPERTIES VERSION ${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR}.${TGUI_VERSION_PATCH})
    endif()

    # on Windows/gcc get rid of "lib" prefix for shared libraries,
    # and transform the ".dll.a" suffix into ".a" for import libraries
    if(TGUI_OS_WINDOWS AND TGUI_COMPILER_GCC)
        set_target_properties(tgui PROPERTIES PREFIX "")
        set_target_properties(tgui PROPERTIES IMPORT_SUFFIX ".a")
    endif()

else()
    add_library(tgui STATIC)
    set_target_properties(tgui PROPERTIES DEBUG_POSTFIX -s-d)
    set_target_properties(tgui PROPERTIES RELEASE_POSTFIX -s)
    set_target_properties(tgui PROPERTIES MINSIZEREL_POSTFIX -s)
    set_target_properties(tgui PROPERTIES RELWITHDEBINFO_POSTFIX -s)
endif()

if (TGUI_BUILD_AS_CXX_MODULE)
    target_sources(tgui
                   PUBLIC
                   FILE_SET tgui_cxx_module_files
                   TYPE CXX_MODULES
                   BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}"
                   FILES ${TGUI_SRC} ${TGUI_SRC_WITHOUT_UNITY})
else()
    target_sources(tgui PRIVATE ${TGUI_SRC} ${TGUI_SRC_WITHOUT_UNITY})
endif()
target_sources(tgui PRIVATE ${TGUI_HEADERS})

if (${CMAKE_VERSION} VERSION_LESS "3.18")
    # We don't support using CMAKE_UNITY_BUILD in CMake < 3.18 (because we need UNITY_BUILD_MODE)
    set_target_properties(tgui PROPERTIES UNITY_BUILD OFF)
else()
    # We manually group files together since we need to keep the different backends separate from each other.
    # Since grouping some files requires manually grouping ALL files, we have to manually create batches here.
    set_target_properties(tgui PROPERTIES UNITY_BUILD_MODE GROUP)

    if(NOT TGUI_BUILD_AS_CXX_MODULE)
        # Enable unity build by default
        if (NOT DEFINED CMAKE_UNITY_BUILD AND NOT DEFINED TGUI_OPTIMIZE_SINGLE_BUILD)
            set_target_properties(tgui PROPERTIES UNITY_BUILD ON)
        endif()
    endif()

    if(UNITY_BUILD_BATCH_SIZE)
        set(batch_size ${UNITY_BUILD_BATCH_SIZE})
    else()
        set(batch_size 8)
    endif()

    # Reduce size of batches if we have sufficient cores
    include(ProcessorCount)
    ProcessorCount(nr_threads)
    if (NOT nr_threads EQUAL 0)
        list(LENGTH TGUI_SRC file_count)
        math(EXPR max_batch_size "${file_count} / ${nr_threads}")
        if (max_batch_size EQUAL 1)
            # Disable unity build if we have more threads than files
            set_target_properties(tgui PROPERTIES UNITY_BUILD OFF)
        endif()
        if (max_batch_size LESS batch_size)
            set(batch_size ${max_batch_size})
        endif()
    endif()

    set(batch_id 1)
    set(files_in_bucket 0)
    math(EXPR file_count_minus_one "${file_count} - 1")
    foreach(i RANGE 0 ${file_count_minus_one})
        list(GET TGUI_SRC ${i} input_file)
        set_source_files_properties(${input_file} PROPERTIES UNITY_GROUP "batch-${batch_id}")
        math(EXPR files_in_bucket "${files_in_bucket} + 1")
        if (files_in_bucket GREATER_EQUAL batch_size)
            set(files_in_bucket 0)
            math(EXPR batch_id "${batch_id} + 1")
        endif()
    endforeach()
endif()

if(TGUI_OS_LINUX)
    # For the FileDialog we need to link to pthreads and dl on Linux and BSD (to load system icons in the background)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(tgui PRIVATE Threads::Threads)
    target_link_libraries(tgui PRIVATE ${CMAKE_DL_LIBS})

    # The TGUI_USE_X11 variable must always exist on linux to generate a valid TGUIConfig.cmake file
    set(TGUI_USE_X11 OFF)
endif()

add_library(TGUI::TGUI ALIAS tgui)
set_target_properties(tgui PROPERTIES EXPORT_NAME TGUI)

# Add the backends to the library.
# Without the ability to call target_link_libraries in a subdirectory (which was only added in CMake 3.13 in policy CMP0079),
# the code would be more complicated when using add_subdirectory. So we just avoid creating a new scope by using include.
# TODO: We now require a newer CMake version, so we should be able to find a solution that uses add_subdirectory instead of include
include(Backend/CMakeLists.txt)

# Define either TGUI_STATIC or TGUI_DYNAMIC
if(TGUI_SHARED_LIBS)
    # The TGUI_DYNAMIC define is practically never needed. It would only be required in a project that dynamically
    # links to TGUI without an SFML backend, while linking to a static SFML library in the project itself.
    target_compile_definitions(tgui PUBLIC TGUI_DYNAMIC)
else()
    target_compile_definitions(tgui PUBLIC TGUI_STATIC)
endif()

tgui_set_global_compile_flags(tgui)
tgui_set_stdlib(tgui)

if((TGUI_OPTIMIZE_SINGLE_BUILD OR TGUI_BUILD_AS_CXX_MODULE) AND TGUI_COMPILER_MSVC)
    # We don't need this for the new UNITY_BUILD, only for the deprecated TGUI_OPTIMIZE_SINGLE_BUILD option,
    # because the new batching doesn't result in such huge object files.
    # Building TGUI as a module currently concatenates ALL source files, which is why /bigobj is required again.
    target_compile_options(tgui PRIVATE /bigobj)
endif()

# Add <project>/include as public include directory
target_include_directories(tgui PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
if (TGUI_BUILD_FRAMEWORK)
    target_include_directories(tgui INTERFACE $<INSTALL_INTERFACE:TGUI.framework>)
else()
    target_include_directories(tgui INTERFACE $<INSTALL_INTERFACE:include>)
endif()

if(TGUI_OS_ANDROID)
    # We need to link to an extra library on android (to use the asset manager)
    target_link_libraries(tgui PRIVATE android)

    # Always use position-independent code on Android, even when linking statically.
    # This is needed because all c++ code is placed in a shared library on Android.
    set_target_properties(tgui PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Enable automatic reference counting on iOS
if(TGUI_OS_IOS)
    set_target_properties(tgui PROPERTIES XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)
endif()

# Hide public symbols by default (exported ones are explicitly marked)
if(NOT TGUI_OS_WINDOWS AND TGUI_SHARED_LIBS)
    set_target_properties(tgui PROPERTIES
                          CXX_VISIBILITY_PRESET hidden
                          VISIBILITY_INLINES_HIDDEN YES)
endif()

# For Visual Studio on Windows, export debug symbols (PDB files) to lib directory (in Debug and RelWithDebInfo configurations)
if(TGUI_GENERATE_PDB)
    # Linker PDB files seem to be generated on the correct location and with the correct name in VS2017,
    # so we only have deal with the compiler PDBs for static libraries.
    # In VS2019 compiler PDBs are also correctly generated by default, but in VS2017 the file is located at the wrong
    # location and lacks the "-d" postfix. The behavior depends on the toolset version, not the IDE version itself.
    # When using NMake Makefiles (even with VS2019), the same behavior as the v141 toolset (VS2017) is observed.
    # MSVC_IDE is 1 when using a generator with "Visual Studio" in its name.
    if(NOT TGUI_SHARED_LIBS AND (NOT MSVC_IDE OR MSVC_VERSION LESS 1920))  # 1920 = VS2019 (v142 toolset)
        # In multi-configuration builds the CMAKE_BUILD_TYPE shouldn't be used and the postfix can be wrong.
        # A generator expression for COMPILE_PDB_NAME to solve this didn't seem to work though.
        # RelWithDebInfo is less likely to be used, so we will default to "-d" at the end.
        if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
            set(TGUI_PDB_POSTFIX "")
        else()
            set(TGUI_PDB_POSTFIX "-d")
        endif()

        set_target_properties(tgui PROPERTIES
                              COMPILE_PDB_NAME "tgui-s${TGUI_PDB_POSTFIX}"
                              COMPILE_PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
    endif()
endif()

# Build frameworks or dylibs on Apple platforms
if((TGUI_OS_MACOS OR TGUI_OS_IOS) AND TGUI_SHARED_LIBS)
    if(TGUI_BUILD_FRAMEWORK)
        # Adapt target to build frameworks instead of dylibs
        set_target_properties(tgui PROPERTIES
                              FRAMEWORK TRUE
                              FRAMEWORK_VERSION ${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR}.${TGUI_VERSION_PATCH}
                              MACOSX_FRAMEWORK_IDENTIFIER org.tgui.tgui
                              MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR}.${TGUI_VERSION_PATCH}
                              MACOSX_FRAMEWORK_BUNDLE_VERSION ${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR}.${TGUI_VERSION_PATCH})

        # Install the header files to the framework
        add_custom_command(TARGET tgui
                           POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:tgui>/Headers
                           COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/include/TGUI $<TARGET_FILE_DIR:tgui>/Headers
                           COMMAND ${CMAKE_COMMAND} -E create_symlink Versions/${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR}.${TGUI_VERSION_PATCH}/Headers $<TARGET_FILE_DIR:tgui>/../../Headers)

        # The framework has to be with a capital letter (because it includes the header files which must be found in a "TGUI" directory)
        set_target_properties(tgui PROPERTIES OUTPUT_NAME TGUI)
    endif()

    # Adapt install directory to allow distributing dylibs/frameworks in user’s frameworks/application bundle but only if cmake rpath options aren't set
    if(NOT CMAKE_SKIP_RPATH AND NOT CMAKE_SKIP_INSTALL_RPATH AND NOT CMAKE_INSTALL_RPATH AND NOT CMAKE_INSTALL_RPATH_USE_LINK_PATH AND NOT CMAKE_INSTALL_NAME_DIR)
        set_target_properties(tgui PROPERTIES INSTALL_NAME_DIR "@rpath")
        if(NOT CMAKE_SKIP_BUILD_RPATH)
            set_target_properties(tgui PROPERTIES BUILD_WITH_INSTALL_NAME_DIR TRUE)
        endif()
    endif()
endif()

# Generate the Config.hpp header
configure_file(${PROJECT_SOURCE_DIR}/include/TGUI/Config.hpp.in ${PROJECT_SOURCE_DIR}/include/TGUI/Config.hpp @ONLY)

if(NOT TGUI_BUILD_AS_CXX_MODULE AND NOT TGUI_DISABLE_PRECOMPILED_HEADER)
    # Use a precompiled header to speed up compilation
    target_precompile_headers(tgui PRIVATE "${PROJECT_SOURCE_DIR}/include/TGUI/Widget.hpp")
endif()

if (TGUI_BUILD_FRAMEWORK)
    set(config_package_location "TGUI.framework/Resources/CMake")
else()
    set(config_package_location ${CMAKE_INSTALL_LIBDIR}/cmake/TGUI)
endif()

# Generate the TGUIConfig.cmake file
# This had to happen here instead of in the root CMakeLists.txt because otherwise it might try to write to the macOS framework before the framework is installed.
# TODO: CMake 3.13 fixed order of installation, so check if the issue still exists. Since the config files relies on variables that are only defined in this scope (via the backend include), caution is required when moving this.
include(CMakePackageConfigHelpers)
configure_package_config_file("${PROJECT_SOURCE_DIR}/cmake/TGUIConfig.cmake.in"
                              "${PROJECT_BINARY_DIR}/TGUIConfig.cmake"
                              INSTALL_DESTINATION "${config_package_location}")

write_basic_package_version_file("${PROJECT_BINARY_DIR}/TGUIConfigVersion.cmake"
                                 VERSION ${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR}.${TGUI_VERSION_PATCH}
                                 COMPATIBILITY SameMajorVersion)

if (TGUI_SHARED_LIBS)
    set(targets_config_filename TGUISharedTargets.cmake)
else()
    set(targets_config_filename TGUIStaticTargets.cmake)
endif()

if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    # When building TGUI alongside its dependencies, the export is only possible when all dependencies also do the export.
    # To keep the code simple, we will never create the config file in our build folder when TGUI is build as a subdirectory.
    # At the time of writing, the following dependencies have or lack the export call:
    # - Have: SFML 2, SDL 3
    # - Lack: SDL_ttf, FreeType, GLFW, SFML 3 (https://github.com/SFML/SFML/pull/2619)
    export(TARGETS tgui
           NAMESPACE TGUI::
           FILE "${PROJECT_BINARY_DIR}/${targets_config_filename}")
endif()

if (TGUI_INSTALL)
    # Install library
    if (TGUI_BUILD_AS_CXX_MODULE)
        install(TARGETS tgui EXPORT TGUIConfigExport
                RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin
                ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
                FILE_SET tgui_cxx_module_files DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT include
                FRAMEWORK DESTINATION "." COMPONENT bin)
    else()
        install(TARGETS tgui EXPORT TGUIConfigExport
                RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin
                ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
                FRAMEWORK DESTINATION "." COMPONENT bin)
    endif()

    install(EXPORT TGUIConfigExport
            NAMESPACE TGUI::
            FILE ${targets_config_filename}
            DESTINATION ${config_package_location})

    install(FILES "${PROJECT_BINARY_DIR}/TGUIConfig.cmake"
                  "${PROJECT_BINARY_DIR}/TGUIConfigVersion.cmake"
            DESTINATION ${config_package_location}
            COMPONENT devel)

    # Install the find modules when they are needed to find our dependencies
    if((TGUI_HAS_WINDOW_BACKEND_RAYLIB OR TGUI_HAS_RENDERER_BACKEND_RAYLIB OR TGUI_HAS_FONT_BACKEND_RAYLIB) AND NOT TGUI_FOUND_RAYLIB_CONFIG)
        install(FILES "${PROJECT_SOURCE_DIR}/cmake/Modules/Findraylib.cmake" DESTINATION ${config_package_location} COMPONENT devel)
        add_custom_command(TARGET tgui POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/cmake/Modules/Findraylib.cmake" "${PROJECT_BINARY_DIR}/" VERBATIM)
    endif()
    if(TGUI_HAS_WINDOW_BACKEND_GLFW AND NOT TGUI_FOUND_GLFW_CONFIG)
        install(FILES "${PROJECT_SOURCE_DIR}/cmake/Modules/Findglfw3.cmake" DESTINATION ${config_package_location} COMPONENT devel)
        add_custom_command(TARGET tgui POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/cmake/Modules/Findglfw3.cmake" "${PROJECT_BINARY_DIR}/" VERBATIM)
    endif()
    if((TGUI_HAS_WINDOW_BACKEND_SDL OR TGUI_HAS_RENDERER_BACKEND_SDL_RENDERER OR TGUI_HAS_FONT_BACKEND_SDL_TTF) AND NOT TGUI_FOUND_SDL2_CONFIG AND NOT TGUI_USE_SDL3)
        install(FILES "${PROJECT_SOURCE_DIR}/cmake/Modules/FindSDL2.cmake" DESTINATION ${config_package_location} COMPONENT devel)
        add_custom_command(TARGET tgui POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/cmake/Modules/FindSDL2.cmake" "${PROJECT_BINARY_DIR}/" VERBATIM)
    endif()
    if(TGUI_HAS_FONT_BACKEND_SDL_TTF AND NOT TGUI_FOUND_SDL2_TTF_CONFIG AND NOT TGUI_USE_SDL3)
        install(FILES "${PROJECT_SOURCE_DIR}/cmake/Modules/FindSDL2_ttf.cmake" DESTINATION ${config_package_location} COMPONENT devel)
        add_custom_command(TARGET tgui POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/cmake/Modules/FindSDL2_ttf.cmake" "${PROJECT_BINARY_DIR}/" VERBATIM)
    endif()
endif()
