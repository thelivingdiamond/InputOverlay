//
// Created by theli on 1/3/2023.
//

#include "OverlayInputListener.h"
#include "ModMain.h"

OverlayInputListener::~OverlayInputListener() {
    gEnv->pInput->RemoveEventListener(this);

}

bool OverlayInputListener::OnInputEvent(const SInputEvent &event) {
    if(event.keyId == eKI_MouseX || event.keyId == eKI_MouseY){
        gMod->setMouseState(event.keyId, event.value);
    } else {
        gMod->setKeyState(event.keyId, event.state);
    }
    return false;
}

bool OverlayInputListener::OnInputEventUI(const SInputEvent &event) {
    return false;
}

int OverlayInputListener::GetPriority() {
    return 0;
}

void OverlayInputListener::registerListener() {
    gEnv->pInput->AddEventListener(this);
}
