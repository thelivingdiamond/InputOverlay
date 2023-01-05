//
// Created by theli on 1/4/2023.
//
#ifndef IMGUI_UTILS
#define IMGUI_UTILS

#pragma once


namespace ImGui {
    ImVec2 StringToImVec2(std::string str);

    ImVec4 StringToImVec4(std::string str);

    std::string ImVec2ToString(ImVec2 vec);

    std::string ImVec4ToString(ImVec4 vec);

    //! the vertical resolution of the game (DPI compensated) will act as the base for deriving the size elements
    float GetDisplayScalar();
}

#endif //IMGUI_UTILS