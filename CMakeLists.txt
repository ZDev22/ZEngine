cmake_minimum_required(VERSION 3.21)
project(VulkanProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform-specific output directory
if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/windows)
elseif(UNIX AND NOT APPLE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/linux)
else()
    message(FATAL_ERROR "Unsupported platform for Vulkan or GLFW")
endif()

# Dependency paths
set(GLFW_BASE_DIR ${CMAKE_SOURCE_DIR}/src/deps/glfw)
set(GLM_DIR ${CMAKE_SOURCE_DIR}/src/deps/glm)

# Platform-specific includes and libraries
if(WIN32)
    set(VULKAN_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/deps/vulkan/includewindows)
    set(VULKAN_LIB ${CMAKE_SOURCE_DIR}/src/deps/vulkan/includewindows/vulkan-1.lib)

    set(GLFW_LIBRARY ${GLFW_BASE_DIR}/glfwwindows/glfw3.lib)
elseif(UNIX AND NOT APPLE)
    set(VULKAN_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/deps/vulkan/includelinux)
    set(GLFW_LIBRARY ${GLFW_BASE_DIR}/glfwlinux/lib/libglfw3.a)
else()
    message(FATAL_ERROR "Unsupported platform for Vulkan or GLFW")
endif()

set(GLM_INCLUDE_DIR ${GLM_DIR})

# Gather sources
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")

# Filter out unwanted files
list(FILTER SOURCES EXCLUDE REGEX "CMakeCXXCompilerId\\.cpp")
list(FILTER SOURCES EXCLUDE REGEX "CMakeCCompilerId\\.c")
list(FILTER SOURCES EXCLUDE REGEX "src/deps/glm/.*")
list(FILTER SOURCES EXCLUDE REGEX "src/deps/vulkan/.*")
list(FILTER SOURCES EXCLUDE REGEX "src/deps/glfw/.*")

# Ensure global.cpp is included
list(APPEND SOURCES src/vulkan/global.cpp)
message(STATUS "Sources: ${SOURCES}")

if(NOT SOURCES)
    message(FATAL_ERROR "No source files found in src/ directory")
endif()

# Define executable
add_executable(main ${SOURCES})

# Include directories
target_include_directories(main PRIVATE
    ${VULKAN_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/include
)

# Link libraries
if(WIN32)
    target_link_libraries(main PRIVATE
        ${GLFW_LIBRARY}
        ${VULKAN_LIB}
        user32
        gdi32
        shell32
    )

    # Enable bigobj for large header files like exprtk
    target_compile_options(main PRIVATE /bigobj)
elseif(UNIX AND NOT APPLE)
    find_package(Vulkan REQUIRED)
    target_link_libraries(main PRIVATE ${GLFW_LIBRARY} Vulkan::Vulkan dl pthread)
endif()

# Set executable properties
set_target_properties(main PROPERTIES WIN32_EXECUTABLE FALSE)