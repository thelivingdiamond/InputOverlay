//
// Created by theli on 1/3/2023.
//

#ifndef INPUTOVERLAY_OVERLAYINPUTLISTENER_H
#define INPUTOVERLAY_OVERLAYINPUTLISTENER_H

#include <Prey/CryInput/IInput.h>

class OverlayInputListener : public IInputEventListener {
public:
    ~OverlayInputListener() override;

    bool OnInputEvent(const SInputEvent &event) override;

    bool OnInputEventUI(const SInputEvent &event) override;

    int GetPriority() override;

    void registerListener();
};


#endif //INPUTOVERLAY_OVERLAYINPUTLISTENER_H
