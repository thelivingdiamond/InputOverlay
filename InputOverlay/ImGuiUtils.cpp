//
// Created by theli on 1/4/2023.
//

#include "ImGuiUtils.h"
#include <regex>

ImVec2 ImGui::StringToImVec2(std::string str) {
    // example: "0.000000,0.000000"
    std::regex regex(R"((\d+.\d+),(\d+.\d+))");
    std::smatch match;
    if (std::regex_search(str, match, regex)) {
        return ImVec2(std::stof(match[1]), std::stof(match[2]));
    }
    return {};
}

ImVec4 ImGui::StringToImVec4(std::string str) {
    // example: "0.000000,0.000000,0.000000,0.000000"
    std::regex regex(R"((\d+.\d+),(\d+.\d+),(\d+.\d+),(\d+.\d+))");
    std::smatch match;
    if (std::regex_search(str, match, regex)) {
        return ImVec4(std::stof(match[1]), std::stof(match[2]), std::stof(match[3]), std::stof(match[4]));
    }
    return {};
}

std::string ImGui::ImVec2ToString(ImVec2 vec) {
    return std::to_string(vec.x) + "," + std::to_string(vec.y);
}

std::string ImGui::ImVec4ToString(ImVec4 vec) {
    return std::to_string(vec.x) + "," + std::to_string(vec.y) + "," + std::to_string(vec.z) + "," + std::to_string(vec.w);
}

//! the vertical resolution of the game (DPI compensated) will act as the base for deriving the size elements
float ImGui::GetDisplayScalar(){
    return ImGui::GetIO().DisplaySize.y * ImGui::GetWindowDpiScale();
}
