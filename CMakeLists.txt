cmake_minimum_required(VERSION 3.21)
project(VulkanProject)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
elseif(APPLE)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/mac/Debug)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/mac/linux/Release)
   file(GLOB VULKAN_SDK_CANDIDATES "$ENV{HOME}/VulkanSDK/*/macOS")
    list(SORT VULKAN_SDK_CANDIDATES ORDER DESCENDING)
    list(GET VULKAN_SDK_CANDIDATES 0 VULKAN_SDK_DIR)

    if(NOT VULKAN_SDK_DIR)
        message(FATAL_ERROR "No Vulkan SDK found in $HOME/VulkanSDK")
    endif()

    set(VULKAN_LIB "${VULKAN_SDK_DIR}/lib/libvulkan.1.dylib")
    set(VULKAN_INCLUDE "${VULKAN_SDK_DIR}/include")
else()
   message(FATAL_ERROR "Unsupported platform!")
endif()

if(NOT APPLE)
   add_library(Vulkan STATIC IMPORTED GLOBAL)
   set_target_properties(Vulkan PROPERTIES
      IMPORTED_LOCATION "${VULKAN_LIBRARY_PATH}"
      INTERFACE_INCLUDE_DIRECTORIES "${VULKAN_INCLUDE_DIR}"
   )
endif()

file(GLOB_RECURSE ALL_CPP_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")
file(GLOB_RECURSE ALL_C_SOURCES   RELATIVE ${CMAKE_SOURCE_DIR} "src/*.c")

set(SOURCES "")

if(APPLE)
   list(APPEND SOURCES "src/deps/macRGFW.m")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

foreach(F ${ALL_CPP_SOURCES} ${ALL_C_SOURCES})
   if(NOT F MATCHES "^src/deps/")
      list(APPEND SOURCES "${F}")
   endif()
endforeach()

add_executable(main ${SOURCES})

target_include_directories(main PRIVATE
   "${VULKAN_INCLUDE_DIR}"
   "${CMAKE_SOURCE_DIR}/src"
)

if(UNIX)
   if(APPLE)
      target_include_directories(main PRIVATE ${VULKAN_INCLUDE})

      target_link_libraries(main PRIVATE
         "${VULKAN_LIB}"
         "-framework Cocoa"
         "-framework Metal"
         "-framework QuartzCore"
         "-framework IOKit"
         "-framework CoreFoundation"
         "-framework CoreGraphics"
         "-framework CoreVideo"
         objc
      )
   elseif(WAYLAND_ENABLED)
      target_link_libraries(main PRIVATE
         wayland-cursor
         wayland-client
         wayland-egl
         xkbcommon
         EGL
      )
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DRGFW_WAYLAND")
   else()
      find_package(X11 REQUIRED)
      target_link_libraries(main PRIVATE
         ${X11_LIBRARIES}
         dl
         Xrandr
         xkbcommon
         pthread
         Vulkan
      )
   endif()
else()
   target_link_libraries(main PRIVATE
	  gdi32
     Vulkan
   )
endif()

set_target_properties(main PROPERTIES WIN32_EXECUTABLE FALSE)