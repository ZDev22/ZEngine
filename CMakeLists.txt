cmake_minimum_required(VERSION 3.21)
project(VulkanProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -DNDEBUG")

set(VULKAN_BASE_DIR ${CMAKE_SOURCE_DIR}/src/deps/vulkan)

if(WIN32)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/windows/Debug)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/windows/Release)

   set(VULKAN_INCLUDE_DIR ${VULKAN_BASE_DIR}/includewindows)
   set(VULKAN_LIBRARY_PATH ${VULKAN_BASE_DIR}/includewindows/vulkan-1.lib)
elseif(UNIX AND NOT APPLE)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/linux/Debug)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/linux/Release)

   set(VULKAN_INCLUDE_DIR ${VULKAN_BASE_DIR}/includelinux)
   set(VULKAN_LIBRARY_PATH ${VULKAN_BASE_DIR}/includelinux/libvulkan.so.1.4.321)
else()
   message(FATAL_ERROR "Unsupported platform!")
endif()

add_library(Vulkan STATIC IMPORTED GLOBAL)
set_target_properties(Vulkan PROPERTIES
   IMPORTED_LOCATION "${VULKAN_LIBRARY_PATH}"
   INTERFACE_INCLUDE_DIRECTORIES "${VULKAN_INCLUDE_DIR}"
)

file(GLOB_RECURSE ALL_CPP_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")
file(GLOB_RECURSE ALL_C_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.c")

set(SOURCES "")
list(APPEND SOURCES "src/deps/rgfw.cpp")
foreach(F ${ALL_CPP_SOURCES} ${ALL_C_SOURCES})
   if(NOT F MATCHES "^src/deps/")
       list(APPEND SOURCES "${F}")
   endif()
endforeach()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
add_executable(main ${SOURCES})

target_include_directories(main PRIVATE
   "${VULKAN_INCLUDE_DIR}"
   "${CMAKE_SOURCE_DIR}/src"
   "${CMAKE_SOURCE_DIR}/src/include"
)

target_link_libraries(main PRIVATE
    Vulkan
)

if(UNIX AND NOT APPLE)
find_package(X11 REQUIRED)
    target_link_libraries(main PRIVATE
        ${X11_LIBRARIES}
        dl
        Xrandr
        wayland-client
        wayland-egl
        wayland-client
        wayland-cursor
        xkbcommon
        EGL
    )
endif()

set_target_properties(main PROPERTIES WIN32_EXECUTABLE FALSE)