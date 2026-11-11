cmake_minimum_required(VERSION 3.21)
project(ZEngine LANGUAGES C CXX)

find_package(Vulkan REQUIRED)
find_program(GLSLC glslc REQUIRED)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_C_FLAGS  "-Ofast -Wall -Wextra -Wno-maybe-uninitialized -Wno-override-init -DZENGINE_MAX_SPRITES=10000 -DZENGINE_MAX_TEXTURES=100 -DTRACK_FPS")
#set(CMAKE_CXX_FLAGS  "-Ofast -Wall -Wextra -Wno-maybe-uninitialized -Wno-override-init -DZENGINE_MAX_SPRITES=10000 -DZENGINE_MAX_TEXTURES=100 -DTRACK_FPS")
set(CMAKE_C_FLAGS  "-O0 -Wall -Wextra -Wno-maybe-uninitialized -Wno-override-init -DZENGINE_MAX_SPRITES=10000 -DZENGINE_MAX_TEXTURES=100 -DZENGINE_DEBUG -DZENGINE_ENABLE_VALIDATION_LAYERS -DTRACK_FPS")
set(CMAKE_CXX_FLAGS  "-O0 -Wall -Wextra -Wno-maybe-uninitialized -Wno-override-init -DZENGINE_MAX_SPRITES=10000 -DZENGINE_MAX_TEXTURES=100 -DZENGINE_DEBUG -DZENGINE_ENABLE_VALIDATION_LAYERS -DTRACK_FPS")

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

file(GLOB_RECURSE ALL_C_SOURCES   CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR} "src/*.c")
file(GLOB_RECURSE ALL_CPP_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR} "src/*.cpp")

set(SOURCES ${ALL_C_SOURCES} ${ALL_CPP_SOURCES})
add_executable(main ${SOURCES})

add_custom_command(TARGET main POST_BUILD
    # copy assets and stuffs
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/src/assets
            $<TARGET_FILE_DIR:main>/assets

    # create shaders paths and stuffs
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:main>/shaders

    # compile shaders and stuffs
    COMMAND ${GLSLC} -O --target-env=vulkan1.2
        "${CMAKE_SOURCE_DIR}/src/shaders/z.frag"
        -o "$<TARGET_FILE_DIR:main>/shaders/z.frag.spv"

    COMMAND ${GLSLC} -O --target-env=vulkan1.2
        "${CMAKE_SOURCE_DIR}/src/shaders/z.vert"
        -o "$<TARGET_FILE_DIR:main>/shaders/z.vert.spv"
)

target_include_directories(main PRIVATE "${CMAKE_SOURCE_DIR}/src" )
target_link_libraries(main PRIVATE gdi32 Vulkan::Vulkan )
