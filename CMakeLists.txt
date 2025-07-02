cmake_minimum_required(VERSION 3.21)
project(VulkanProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directory based on platform
if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/windows)
elseif(UNIX AND NOT APPLE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/linux)
else()
    message(FATAL_ERROR "Unsupported platform for Vulkan or GLFW")
endif()

# Define paths to local dependencies
set(GLFW_BASE_DIR ${CMAKE_SOURCE_DIR}/src/deps/glfw)
set(GLM_DIR ${CMAKE_SOURCE_DIR}/src/deps/glm)

# Select Vulkan include directory based on platform
if(UNIX AND NOT APPLE)
    set(VULKAN_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/deps/vulkan/includelinux)
    set(GLFW_INCLUDE_DIR ${GLFW_BASE_DIR}/glfwlinux/include)
    set(GLFW_LIBRARY ${GLFW_BASE_DIR}/glfwlinux/lib/libglfw3.a)
elseif(WIN32)
    set(VULKAN_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/deps/vulkan/includewindows)
    set(GLFW_INCLUDE_DIR ${GLFW_BASE_DIR}/glfwwindows/include)
    set(GLFW_LIBRARY ${GLFW_BASE_DIR}/glfwwindows/lib-mingw/libglfw3.a)
else()
    message(FATAL_ERROR "Unsupported platform for Vulkan or GLFW")
endif()

# GLM is header-only
set(GLM_INCLUDE_DIR ${GLM_DIR})

# Collect your source files
file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")
file(GLOB_RECURSE C_SOURCES "src/*.c")
list(APPEND SOURCES ${CPP_SOURCES} ${C_SOURCES})

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

# Define the executable
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
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
        opengl32
        gdi32
        user32
        shell32
        ${CMAKE_SOURCE_DIR}/src/deps/vulkan/includewindows/vulkan-1.lib
    )

    # Copy Vulkan DLL if needed (dynamic loading)
    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/src/deps/vulkan/includewindows/vulkan-1.lib"
        $<TARGET_FILE_DIR:main>/vulkan-1.lib
    )
elseif(UNIX AND NOT APPLE)
    find_package(Vulkan REQUIRED)
    target_link_libraries(main PRIVATE ${GLFW_LIBRARY} Vulkan::Vulkan dl pthread)
endif()

# Set the executable properties
set_target_properties(main PROPERTIES WIN32_EXECUTABLE FALSE)