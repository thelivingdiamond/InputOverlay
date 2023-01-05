//
// Created by theli on 1/3/2023.
//

#include "KeyElement.h"
#include "InputNameMap.h"

#include <utility>
#include "ImGuiUtils.h"


void KeyElement::SetPosition(ImVec2 position) {
    m_Position = position;
}


void KeyElement::SetKey(EKeyId keyId) {
    m_KeyId = keyId;
}

void KeyElement::draw(ImDrawList *drawList, ImVec2 windowPos) {
    ImVec2 compPos = {m_Position.x * ImGui::GetDisplayScalar(), m_Position.y * ImGui::GetDisplayScalar()};
    ImVec2 compSize = {m_Size.x * ImGui::GetDisplayScalar(), m_Size.y * ImGui::GetDisplayScalar()};
    auto color = getStateColor(m_InputState);
    drawList->AddRectFilled(ImVec2(windowPos.x + compPos.x, windowPos.y + compPos.y), ImVec2(windowPos.x + compPos.x + compSize.x, windowPos.y + compPos.y + compSize.y), color, 4.0f);
    drawList->AddRect(ImVec2(windowPos.x + compPos.x, windowPos.y + compPos.y), ImVec2(windowPos.x + compPos.x + compSize.x, windowPos.y + compPos.y + compSize.y), ImGui::ColorConvertFloat4ToU32(m_BorderColor), 4.0f, 0, 2.0f);
    // Add Text to the center of the rectangle
    ImVec2 textSize = ImGui::CalcTextSize(EKeyIdToString.at(m_KeyId).c_str());
    ImVec2 textPos = ImVec2(windowPos.x + compPos.x + (compSize.x / 2) - (textSize.x / 2),  windowPos.y + compPos.y + (compSize.y / 2) - (textSize.y / 2));
    drawList->AddText(textPos, ImGui::ColorConvertFloat4ToU32(m_TextColor), EKeyIdToString.at(m_KeyId).c_str());
}

void KeyElement::setState(EInputState inputState) {
    m_InputState = inputState;
}

KeyElement::KeyElement(EKeyId mKeyId) : m_KeyId(mKeyId) {
}

std::string KeyElement::GetKeyString() const {
    return EKeyIdToString.at(m_KeyId);
}
