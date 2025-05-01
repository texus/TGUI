# Define a library for raylib
add_library(raylib STATIC
    rcore.c
    rmodels.c
    rshapes.c
    rtext.c
    rtextures.c
    raudio.c
    utils.c
    )

# Include headers directory for android_native_app_glue.c
include_directories(${ANDROID_NDK}/sources/android/native_app_glue/)

# Add android_native_app_glue.c to the source files
list(APPEND SOURCES ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

# Define compiler macros for raylib
target_compile_definitions(raylib PUBLIC PLATFORM_ANDROID __ANDROID__)

# Add specific compilation options based on target Android architecture
if(CMAKE_ANDROID_ARCH_ABI STREQUAL "armeabi-v7a")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=softfp -mfpu=vfpv3-d16")
elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfix-cortex-a53-835769")
endif()

# Link required libraries to raylib
target_link_libraries(raylib log android EGL OpenSLES dl m c)

# Additional configuration depending on the desired OpenGL version
if(GL_VERSION STREQUAL "ES20")
    target_compile_definitions(raylib PUBLIC GRAPHICS_API_OPENGL_ES2)
    target_link_libraries(raylib GLESv2)
elseif(GL_VERSION STREQUAL "ES30" OR GL_VERSION STREQUAL "ES31" OR GL_VERSION STREQUAL "ES32")
    target_compile_definitions(raylib PUBLIC GRAPHICS_API_OPENGL_ES3)
    target_link_libraries(raylib GLESv3)
endif()

# Add library-specific binding option
target_link_options(raylib PRIVATE "-u ANativeActivity_onCreate")