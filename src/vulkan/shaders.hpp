#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <string>
#include <cstdlib>

inline bool compileShader(const std::string& inputFile, const std::string& outputFile, const std::string& stage) {
    std::string command = "glslc -fshader-stage=" + stage + " " + inputFile + " -o " + outputFile;
    int result = std::system(command.c_str());
    if (result == 0) {
        std::cout << "Compiled: " << inputFile << std::endl;
        return true;
    } else {
        std::cerr << "Failed to compile " << inputFile << " (exit code: " << result << ")" << std::endl;
        return false;
    }
}

inline bool compile() {
    bool success = true;
    std::string shaderDir = "vulkan/shaders";

    const std::unordered_map<std::string, std::string> stageMap = {
        {".vert", "vert"},
        {".frag", "frag"},
        {".comp", "comp"},
        {".geom", "geom"},
        {".tesc", "tesc"},
        {".tese", "tese"},
        {".rgen", "rgen"},
        {".rchit", "rchit"},
        {".rmiss", "rmiss"}
    };

    for (const auto& file : std::filesystem::directory_iterator(shaderDir)) {
        if (!std::filesystem::is_regular_file(file)) continue;

        std::filesystem::path path = file.path();
        std::string ext = path.extension().string();

        auto it = stageMap.find(ext);
        if (it == stageMap.end()) continue;

        std::string inputFile = path.string();
        std::string outputFile = inputFile + ".spv";

        success &= compileShader(inputFile, outputFile, it->second);
    }

    return success;
}