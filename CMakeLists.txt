cmake_minimum_required(VERSION 3.21)
project(VulkanProject)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)

if(UNIX AND NOT APPLE)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/linux/Debug)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/linux/Release)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_SMOL ${CMAKE_SOURCE_DIR}/build/linux/Smol)

elseif(APPLE)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/mac/Debug)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/mac/Release)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_SMOL ${CMAKE_SOURCE_DIR}/build/mac/Smol)

   file(GLOB VULKAN_SDK_CANDIDATES "$ENV{HOME}/VulkanSDK/*/macOS")
   list(SORT VULKAN_SDK_CANDIDATES ORDER DESCENDING)
   list(GET VULKAN_SDK_CANDIDATES 0 VULKAN_SDK_DIR)

   if(NOT VULKAN_SDK_DIR)
      message(FATAL_ERROR "No Vulkan SDK found in $HOME/VulkanSDK")
   endif()

   set(VULKAN_LIB "${VULKAN_SDK_DIR}/lib/libvulkan.1.dylib")
   set(VULKAN_INCLUDE "${VULKAN_SDK_DIR}/include")

elseif(WIN32)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/windows/Debug)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/windows/Release)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_SMOL ${CMAKE_SOURCE_DIR}/build/windows/Smol)

else()
   message(FATAL_ERROR "Unsupported platform!")
endif()

file(GLOB_RECURSE ALL_CPP_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")
file(GLOB_RECURSE ALL_C_SOURCES   RELATIVE ${CMAKE_SOURCE_DIR} "src/*.c")

set(SOURCES "")
list(APPEND SOURCES ${ALL_CPP_SOURCES} ${ALL_C_SOURCES})

add_executable(main ${SOURCES})

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/src/shaders
            $<TARGET_FILE_DIR:main>/shaders
)
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/src/assets
            $<TARGET_FILE_DIR:main>/assets
)

target_include_directories(main PRIVATE
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
   else()
      find_package(X11 REQUIRED)
      target_link_libraries(main PRIVATE
         ${X11_LIBRARIES}
         dl
         Xrandr
         xkbcommon
         pthread
         Vulkan::Vulkan
      )
   endif()
else()
   target_link_libraries(main PRIVATE
	 gdi32
     Vulkan::Vulkan
   )
endif()

set_target_properties(main PROPERTIES WIN32_EXECUTABLE FALSE)