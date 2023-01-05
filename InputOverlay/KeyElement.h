//
// Created by theli on 1/3/2023.
//

#ifndef INPUTOVERLAY_KEYELEMENT_H
#define INPUTOVERLAY_KEYELEMENT_H


struct KeyElement {
    static const inline ImVec2 DEFAULT_SIZE = ImVec2(0.05f, 0.05f);
    static inline ImVec4 m_BorderColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    static inline ImVec4 m_FillColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    static inline ImVec4 m_TextColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    static ImColor getStateColor(EInputState state){
        switch (state) {
            case EInputState::eIS_Pressed:
                return {255, 255, 255, 175};
            case EInputState::eIS_Released:
                return ImGui::ColorConvertFloat4ToU32(m_FillColor);
            case EInputState::eIS_Down:
                return {255, 255, 255, 120};
            default:
                return {0, 0, 0, 0};
        }
    }
    EKeyId m_KeyId;

    KeyElement(EKeyId mKeyId);
    ~KeyElement() = default;

    ImVec2 m_Position;

    EInputState m_InputState = EInputState::eIS_Released;
public:
    virtual void draw(ImDrawList *drawList, ImVec2 windowPos);
    void setState(EInputState inputState);
    virtual void SetPosition(ImVec2 position);
    virtual void SetKey(EKeyId keyId);
    virtual std::string GetKeyString() const;

    ImVec2 m_Size = DEFAULT_SIZE;
    
    // operators for sorting and searching
    bool operator==(const KeyElement& other) const { return m_KeyId == other.m_KeyId; }
    bool operator<(const KeyElement& other) const { return m_KeyId < other.m_KeyId; }
    bool operator!=(const KeyElement& other) const { return !(*this == other); }
    bool operator>(const KeyElement& other) const { return other < *this; }
    bool operator<=(const KeyElement& other) const { return !(other < *this); }
    bool operator>=(const KeyElement& other) const { return !(*this < other); }

    // operators for sorting and searching against EKeyId
    bool operator==(const EKeyId& other) const { return m_KeyId == other; }
    bool operator<(const EKeyId& other) const { return m_KeyId < other; }
    bool operator!=(const EKeyId& other) const { return !(*this == other); }
    bool operator>(const EKeyId& other) const { return !(*this < other) && *this != other; }
    bool operator<=(const EKeyId& other) const { return (*this < other) || *this == other; }
    bool operator>=(const EKeyId& other) const { return !(*this < other); }

};


#endif //INPUTOVERLAY_KEYELEMENT_H
