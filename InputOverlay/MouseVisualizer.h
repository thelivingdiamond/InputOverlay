//
// Created by theli on 1/4/2023.
//

#ifndef INPUTOVERLAY_MOUSEVISUALIZER_H
#define INPUTOVERLAY_MOUSEVISUALIZER_H

#include "KeyElement.h"

struct MouseVisualizer : public KeyElement {
public:
    MouseVisualizer(EKeyId mKeyId = EKeyId::eKI_Unknown);
    void draw(ImDrawList *drawList, ImVec2 windowPos) override;
    void SetPosition(ImVec2 position) override;
    std::string GetKeyString() const override;
    void Update();

    void updateState(EKeyId mouseButton, float val);
    int resetCounter = 0;
    const int RESET_COUNTER_MAX = 30;
    ImVec2 m_MouseDelta = ImVec2(0, 0);
};


#endif //INPUTOVERLAY_MOUSEVISUALIZER_H
