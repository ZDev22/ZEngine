#include "main.hpp"

#include "vulkan/app.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "deps/glfw/glfw3.h"

short scene = 1;

bool compileShader(const std::string& inputFile, const std::string& outputFile, const std::string& stage) {
    std::string command = "glslc -fshader-stage=" + stage + " " + inputFile + " -o " + outputFile;
    int result = std::system(command.c_str());
    if (result == 0) { return true; }
    else { std::cerr << "Failed to compile " << inputFile << " (exit code: " << result << ")" << std::endl; return false; }
}

bool compile() {
    bool success = true;
    success &= compileShader("vulkan/shaders/triangle.vert", "vulkan/shaders/triangle.vert.spv", "vert");
    success &= compileShader("vulkan/shaders/triangle.frag", "vulkan/shaders/triangle.frag.spv", "frag");
    return success;
}

void error_callback(int error, const char* description) { std::cerr << "GLFW Error (" << error << "): " << description << std::endl; }
int main() {
    if (compile()) {

        glfwSetErrorCallback(error_callback);
        std::cout << "Calling glfwInit()..." << std::endl;
        if (!glfwInit()) { std::cout << "GLFW failed to initialize!" << std::endl; return 1; }
        std::cout << "GLFW initialized successfully!" << std::endl;
        if (!glfwVulkanSupported()) { std::cerr << "GLFW says Vulkan is NOT supported!" << std::endl; return -1; }
        std::cout << "GLFW Vulkan is supported." << std::endl;

        App app{};
        app.run();
    }
    return 0;
}