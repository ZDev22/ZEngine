#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <string>
#include <cstdlib>
#include <future>
#include <vector>

inline bool compileShader(const std::string& inputFile, const std::string& outputFile, const std::string& stage) {
    if (std::filesystem::exists(outputFile)) {
        auto inputTime = std::filesystem::last_write_time(inputFile);
        auto outputTime = std::filesystem::last_write_time(outputFile);
        if (outputTime >= inputTime) {
            std::cout << "Shader " << inputFile << " is up to date." << std::endl;
            return true;
        }
    }

    std::cout << "Compiling " << inputFile << "..." << std::endl;
    std::string command = "glslc -fshader-stage=" + stage + " " + inputFile + " -o " + outputFile;
    int result = std::system(command.c_str());
    if (result == 0) {
        std::cout << "Compiled " << inputFile << " successfully." << std::endl;
        return true;
    } else {
        std::cerr << "Failed to compile " << inputFile << " (exit code: " << result << ")" << std::endl;
        return false;
    }
}

inline bool compile() {
    std::string shaderDir = "vulkan/shaders";
    const std::unordered_map<std::string, std::string> stageMap = {
        {".vert", "vert"}, {".frag", "frag"}, {".comp", "comp"},
        {".geom", "geom"}, {".tesc", "tesc"}, {".tese", "tese"},
        {".rgen", "rgen"}, {".rchit", "rchit"}, {".rmiss", "rmiss"}
    };

    std::vector<std::future<bool>> futures;

    for (const auto& file : std::filesystem::directory_iterator(shaderDir)) {
        if (!std::filesystem::is_regular_file(file)) continue;

        std::filesystem::path path = file.path();
        std::string ext = path.extension().string();
        auto it = stageMap.find(ext);
        if (it == stageMap.end()) continue;

        std::string inputFile = path.string();
        std::string outputFile = inputFile + ".spv";

        futures.push_back(std::async(std::launch::async, compileShader, inputFile, outputFile, it->second));
    }

    bool success = true;
    for (auto& fut : futures) { success &= fut.get(); }
    return success;
}