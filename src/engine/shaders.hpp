#include <iostream>
#include <filesystem>
#include <string>
#include <cstdlib>

inline bool compileShader(const std::string& inputFile, const std::string& outputFile, const char* stage) {
    if (std::filesystem::exists(outputFile)) {
        auto inputTime = std::filesystem::last_write_time(inputFile);
        auto outputTime = std::filesystem::last_write_time(outputFile);
        if (outputTime >= inputTime) {
            std::cout << "Shader " << inputFile << " is up to date." << std::endl;
            return true;
        }
    }

    std::cout << "Compiling " << inputFile << "..." << std::endl;
    std::string command = "glslc -fshader-stage=" + std::string(stage) + " " + inputFile + " -o " + outputFile;
    int result = std::system(command.c_str());
    if (result == 0) {
        std::cout << "Compiled " << inputFile << " successfully." << std::endl;
        return true;
    }
    else {
        std::cerr << "Failed to compile " << inputFile << " (error code " << result << ")" << std::endl;
        return false;
    }
}

inline bool compileShaders() {
    const char* extensions[] = { ".vert", ".frag", ".comp", ".geom", ".tesc", ".tese", ".rgen", ".rchit", ".rmiss" };
    const char* stages[]     = { "vert",  "frag",  "comp",  "geom",  "tesc",  "tese",  "rgen",  "rchit",  "rmiss" };

    bool success = true;
    for (const auto& file : std::filesystem::directory_iterator("engine/shaders")) {
        if (!file.is_regular_file()) continue;

        std::string ext = file.path().extension().string();
        for (int i = 0; i < 9; ++i) {
            if (ext == extensions[i]) {
                std::string inputFile = file.path().string();
                std::string outputFile = inputFile + ".spv";
                success &= compileShader(inputFile, outputFile, stages[i]);
                break;
            }
        }
    }
    return success;
}