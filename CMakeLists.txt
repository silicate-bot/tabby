cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
    tabby
    VERSION 1.0.0
    DESCRIPTION "C++23 ImGUI-based UI framework"
    LANGUAGES CXX
)

option(TABBY_BUILD_TESTS "Whether to build Tabby tests" OFF)

option(TABBY_NO_IMGUI "If ImGUI is already provided by the user" OFF)

option(TABBY_USE_GLFW "Use GLFW for platform code in Tabby" OFF)
option(TABBY_USE_WIN32 "Use raw Windows bindings for platform code in Tabby" OFF)

option(TABBY_USE_OPENGL3 "Use OpenGL 3 for graphics code in Tabby" OFF)

# Install CPM
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

# Sources & includes
set(SOURCES
    src/widgets/text.cpp
    src/widgets/window.cpp
    src/widgets/font.cpp
    src/widgets/button.cpp
    src/widgets/layout.cpp
    src/widgets/drag.cpp
    src/widgets/input.cpp
    src/widgets/checkbox.cpp
    src/widgets/radio.cpp
    src/widgets/dropdown.cpp
    src/widgets/color.cpp
    src/widgets/overlay.cpp

    src/context/context.cpp

        # lib/glad/src/gl.c
)

add_library(tabby STATIC ${SOURCES})
add_library(tabby::tabby ALIAS tabby)

target_include_directories(tabby PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(tabby PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glad/include)
target_include_directories(tabby PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui_lib)

# ImGUI
if (NOT TABBY_NO_IMGUI)
    # CPMAddPackage("gh:ocornut/imgui#master")
    set(imgui_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui_lib)

    # add_compile_definitions(IMGUI_ENABLE_FREETYPE)
    # add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_CUSTOM)

    set(IM_SOURCES
 ${imgui_SOURCE_DIR}/imgui.cpp
 ${imgui_SOURCE_DIR}/imgui_draw.cpp
 ${imgui_SOURCE_DIR}/imgui_widgets.cpp
 ${imgui_SOURCE_DIR}/imgui_tables.cpp
 ${imgui_SOURCE_DIR}/imgui_demo.cpp
 ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
  # ${imgui_SOURCE_DIR}/misc/freetype/imgui_freetype.cpp
    )


    if (TABBY_USE_GLFW)
        CPMAddPackage("gh:glfw/glfw#master")

        add_compile_definitions(TABBY_USE_GLFW)
        target_link_libraries(tabby PRIVATE glfw)
        list(APPEND IM_SOURCES
     ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
 )
    endif()

    target_include_directories(tabby PRIVATE ${imgui_SOURCE_DIR})
    target_sources(${PROJECT_NAME} PRIVATE ${IM_SOURCES})
endif()

# Tests
if (TABBY_BUILD_TESTS)
    set(TEST_SOURCES
 tests/main.cpp
    )

    add_executable(tabbytest ${TEST_SOURCES})

    CPMAddPackage("gh:glfw/glfw#master")
    find_package(OpenGL REQUIRED)

    target_link_libraries(tabbytest PRIVATE tabby glfw OpenGL::GL)

    target_include_directories(tabbytest PRIVATE ${CMAKE_SOURCE_DIR}/include)
endif()
