cmake_minimum_required(VERSION 3.21)
project(VulkanProject)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -DNDEBUG")

set(GLFW_BASE_DIR ${CMAKE_SOURCE_DIR}/src/deps/glfw)
set(GLFW_INCLUDE_DIR ${GLFW_BASE_DIR})
set(GLM_DIR ${CMAKE_SOURCE_DIR}/src/deps/glm)
set(GLM_INCLUDE_DIR ${GLM_DIR})
set(VULKAN_BASE_DIR ${CMAKE_SOURCE_DIR}/src/deps/vulkan)

if(WIN32)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/windows/Debug)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/windows/Release)

   set(GLFW_LIBRARY ${GLFW_BASE_DIR}/windows.a)

   set(VULKAN_INCLUDE_DIR ${VULKAN_BASE_DIR}/includewindows)
   set(VULKAN_LIBRARY_PATH ${VULKAN_BASE_DIR}/includewindows/vulkan-1.lib)
elseif(UNIX AND NOT APPLE)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/linux/Debug)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/linux/Release)

   set(GLFW_LIBRARY ${GLFW_BASE_DIR}/linux.a)

   set(VULKAN_INCLUDE_DIR ${VULKAN_BASE_DIR}/includelinux)
   set(VULKAN_LIBRARY_PATH ${VULKAN_BASE_DIR}/includelinux/libvulkan.so.1.4.321)
else()
   message(FATAL_ERROR "Unsupported platform!")
endif()

add_library(VulkanLocal STATIC IMPORTED GLOBAL)
set_target_properties(VulkanLocal PROPERTIES
   IMPORTED_LOCATION "${VULKAN_LIBRARY_PATH}"
   INTERFACE_INCLUDE_DIRECTORIES "${VULKAN_INCLUDE_DIR}"
)

file(GLOB_RECURSE ALL_CPP_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")
file(GLOB_RECURSE ALL_C_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.c")

set(SOURCES "")
foreach(F ${ALL_CPP_SOURCES} ${ALL_C_SOURCES})
   if(NOT F MATCHES "^src/deps/")
       list(APPEND SOURCES "${F}")
   endif()
endforeach()

list(APPEND SOURCES src/vulkan/global.cpp)
message(STATUS "Sources: ${SOURCES}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
add_executable(main ${SOURCES})

target_include_directories(main PRIVATE
   "${VULKAN_INCLUDE_DIR}"
   "${GLM_INCLUDE_DIR}"
   "${GLFW_INCLUDE_DIR}"
   "${CMAKE_SOURCE_DIR}/src"
   "${CMAKE_SOURCE_DIR}/src/include"
)

target_link_libraries(main PRIVATE
   "${GLFW_LIBRARY}"
   VulkanLocal
   dl
   pthread
)

if(UNIX AND NOT APPLE)
   add_custom_command(TARGET main POST_BUILD
       COMMAND ${CMAKE_COMMAND} -E copy_if_different
       "${VULKAN_LIBRARY_PATH}"
       $<TARGET_FILE_DIR:main>/libvulkan.so.1.4.321
   )
endif()
set_target_properties(main PROPERTIES WIN32_EXECUTABLE FALSE)