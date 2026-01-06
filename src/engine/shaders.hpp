#include <iostream>
#include <filesystem>
#include <string>
#include <cstdlib>

inline bool compileShader(const char* inputFile, const char* outputFile, const char* stage) {
    if (std::filesystem::exists(outputFile)  && std::filesystem::last_write_time(outputFile) >= std::filesystem::last_write_time(inputFile)) {
        std::cout << "Shader " << inputFile << " is up to date.\n";
        return true;
    }

    std::cout << "Compiling " << inputFile << "...\n";
    int result = std::system(("glslc -fshader-stage=" + std::string(stage) + " " + inputFile + " -o " + outputFile).c_str());
    if (result != 0) {
        std::cerr << "Failed to compile " << inputFile << " (error: " << result << ")\n";
        return false;
    }
    return true;
}

inline bool compileShaders() {
    const char* extensions[4] = { ".vert", ".frag", ".comp", ".geom" };
    const char* stages[4]     = { "vert",  "frag",  "comp",  "geom" };

    bool success = true;
    for (const auto& file : std::filesystem::directory_iterator("engine/shaders")) {
        if (!file.is_regular_file()) { continue; }
        for (int i = 0; i < 4; ++i) {
            if (file.path().extension().string() == extensions[i]) {
                success &= compileShader(file.path().string().c_str(), (file.path().string() + ".spv").c_str(), stages[i]);
                break;
            }
        }
    }
    return success;
}