cmake_minimum_required(VERSION 3.21)
project(ZEngine)

find_package(Vulkan REQUIRED)

set(CMAKE_C_FLAGS  "-march=native -O3 -Wall -Wextra -Wno-maybe-uninitialized -Wno-override-init -std=c99")
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

file(GLOB_RECURSE ALL_C_SOURCES   RELATIVE ${CMAKE_SOURCE_DIR} "src/*.c")

set(SOURCES ${ALL_C_SOURCES})
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
target_link_libraries(main PRIVATE
    gdi32
    Vulkan::Vulkan
)

set_target_properties(main PROPERTIES WIN32_EXECUTABLE FALSE)
