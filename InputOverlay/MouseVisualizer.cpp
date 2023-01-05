//
// Created by theli on 1/4/2023.
//

#include "MouseVisualizer.h"
#include "ImGuiUtils.h"

void MouseVisualizer::draw(ImDrawList *drawList, ImVec2 windowPos) {
    ImVec2 compPos = {m_Position.x * ImGui::GetDisplayScalar(), m_Position.y * ImGui::GetDisplayScalar()};
    ImVec2 compSize = {m_Size.x * ImGui::GetDisplayScalar(), m_Size.y * ImGui::GetDisplayScalar()};
    // draw a circle with an arrow in the middle that points in the direction of the mouse movement, and the size of the line is the magnitude of the mouse movement
    drawList->AddCircleFilled(ImVec2(windowPos.x + compPos.x + compSize.x / 2, windowPos.y + compPos.y + compSize.y / 2), compSize.x / 2, ImGui::ColorConvertFloat4ToU32(m_FillColor), 32);
    drawList->AddCircle(ImVec2(windowPos.x + compPos.x + compSize.x / 2, windowPos.y + compPos.y + compSize.y / 2), compSize.x / 2, ImGui::ColorConvertFloat4ToU32(m_BorderColor), 32, 2.0f);
    ImVec2 arrowStart = ImVec2(windowPos.x + compPos.x + compSize.x / 2, windowPos.y + compPos.y + compSize.y / 2);
    ImVec2 arrowEnd = ImVec2(windowPos.x + compPos.x + compSize.x / 2 + m_MouseDelta.x, windowPos.y + compPos.y + compSize.y / 2 + m_MouseDelta.y);
    float lineWidth = 2.0f;
    // make sure the arrow does not go outside the circle using the pythagorean theorem to find the hypotenuse of the triangle, and then scale the arrow to fit inside the circle
    // if the arrow is longer than the radius of the circle, start scaling the line width up to indicate the magnitude of the mouse movement
    float hypotenuse = sqrt(pow(arrowEnd.x - arrowStart.x, 2) + pow(arrowEnd.y - arrowStart.y, 2));
    if (hypotenuse > compSize.x / 2) {
        float scaleFactor = (compSize.x / 2) / hypotenuse;
        arrowEnd.x = arrowStart.x + (arrowEnd.x - arrowStart.x) * scaleFactor;
        arrowEnd.y = arrowStart.y + (arrowEnd.y - arrowStart.y) * scaleFactor;
        lineWidth = 2.0f + (hypotenuse - compSize.x / 2) / 10;
    }
    drawList->AddLine(arrowStart, arrowEnd, ImGui::ColorConvertFloat4ToU32(m_BorderColor), lineWidth);
    // add some text below the circle to indicate the magnitude of the mouse movement
    std::string text = fmt::format("{:.0f}, {:.0f}", m_MouseDelta.x, m_MouseDelta.y);
    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
    ImVec2 textPos = ImVec2(windowPos.x + compPos.x + (compSize.x / 2) - (textSize.x / 2),  windowPos.y + compPos.y + compSize.y + 5);
    drawList->AddText(textPos, ImGui::ColorConvertFloat4ToU32(m_TextColor), text.c_str());
}

void MouseVisualizer::SetPosition(ImVec2 position) {
    KeyElement::SetPosition(position);
}

std::string MouseVisualizer::GetKeyString() const {
    return KeyElement::GetKeyString();
}

void MouseVisualizer::updateState(EKeyId mouseButton, float val) {
    if(mouseButton == EKeyId::eKI_MouseX){
        m_MouseDelta.x = val;
    } else if(mouseButton == EKeyId::eKI_MouseY){
        m_MouseDelta.y = val;
    }
    resetCounter = RESET_COUNTER_MAX;
}

MouseVisualizer::MouseVisualizer(EKeyId mKeyId) : KeyElement(mKeyId) {
    m_Size = ImVec2(DEFAULT_SIZE.x * 2, DEFAULT_SIZE.y * 2);

}

void MouseVisualizer::Update() {
    if(resetCounter > 0){
        resetCounter--;
    } else {
        m_MouseDelta = ImVec2(0, 0);
    }
}
