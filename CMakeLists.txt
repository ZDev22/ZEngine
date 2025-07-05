cmake_minimum_required(VERSION 3.10)
project(VulkanProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directory based on platform and build type
if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/windows/Debug)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/windows/Release)
elseif(UNIX AND NOT APPLE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/linux/Debug)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/linux/Release)
else()
    message(FATAL_ERROR "Unsupported platform for Vulkan or GLFW")
endif()

# Define paths to local dependencies
set(GLFW_BASE_DIR ${CMAKE_SOURCE_DIR}/src/deps/glfw)
set(GLFW_INCLUDE_DIR ${GLFW_BASE_DIR})

# Select GLFW library based on platform
if(WIN32)
    set(GLFW_LIBRARY ${GLFW_BASE_DIR}/windows.a)
elseif(UNIX AND NOT APPLE)
    set(GLFW_LIBRARY ${GLFW_BASE_DIR}/linux.a)
else()
    message(FATAL_ERROR "Unsupported platform for Vulkan or GLFW")
endif()

set(GLM_DIR ${CMAKE_SOURCE_DIR}/src/deps/glm)

# Select Vulkan include directory based on platform
if(UNIX AND NOT APPLE)
    set(VULKAN_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/deps/vulkan/includelinux)
elseif(WIN32)
    set(VULKAN_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/deps/vulkan/includewindows)
else()
    message(FATAL_ERROR "Unsupported platform for Vulkan or GLFW")
endif()

# GLM is header-only
set(GLM_INCLUDE_DIR ${GLM_DIR})

# Collect all source files under src/, EXCLUDING src/deps/
file(GLOB_RECURSE ALL_CPP_SOURCES "src/*.cpp")
file(GLOB_RECURSE ALL_C_SOURCES "src/*.c")

set(EXCLUDED_DIRS "src/deps/")

set(SOURCES "")
foreach(F ${ALL_CPP_SOURCES} ${ALL_C_SOURCES})
    set(EXCLUDE_THIS_FILE FALSE)
    foreach(EXCLUDED_DIR ${EXCLUDED_DIRS})
        string(FIND "${F}" "${EXCLUDED_DIR}" EXCLUDED_FOUND)
        if(NOT EXCLUDED_FOUND EQUAL -1)
            set(EXCLUDE_THIS_FILE TRUE)
        endif()
    endforeach()
    if(NOT EXCLUDE_THIS_FILE)
        list(APPEND SOURCES "${F}")
    endif()
endforeach()

# Ensure global.cpp is included (if needed)
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

    # Optional: Copy Vulkan lib (but probably you wanted the DLL, let me know)
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