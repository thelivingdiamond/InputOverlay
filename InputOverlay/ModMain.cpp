#include "ModMain.h"
#include "KeyElement.h"
#include "MouseVisualizer.h"
#include "InputNameMap.h"
#include "OverlayInputListener.h"
#include "ImGuiUtils.h"

ModMain* gMod = nullptr;

#ifdef EXAMPLE
// You can define PreyDll.dll function using PreyFunction
// auto fnFuncName = PreyFunction<void(T funcArgs)>(functionOffset);
// Many functions are defined as static members with F prefix:
//  ArkPlayer.h
//     static inline auto FHasAbility = PreyFunction<bool(ArkPlayer* _this, uint64_t _abilityID)>(0x1550330);
static auto fn_ArkPlayer_HasAbility = PreyFunction<bool(ArkPlayer* _this, uint64_t _abilityID)>(0x1550330);

// Use MakeHook() method of PreyFunction to create a hook.
// See ModMain::InitHooks for more.
static auto s_hook_ArkPlayer_HasAbility = fn_ArkPlayer_HasAbility.MakeHook();

// This function will be called in place of ArkPlayer::HasAbility.
// See ModMain::InitHooks for more.
static bool ArkPlayer_HasAbility_Hook(ArkPlayer* _this, uint64_t _abilityID)
{
	// NOTE: This particular method of ability unlocking is unreliable.
	// NOTE: It's only used as an example of function hooking.

	if (_abilityID % 2 == 0)
	{
		// Player always has abilities whose ID is even.
		return true;
	}

	// Use InvokeOrig of PreyFunctionHook to call the original (non-hooked) function.
	return s_hook_ArkPlayer_HasAbility.InvokeOrig(_this, _abilityID);
}

#endif

void ModMain::FillModInfo(ModDllInfoEx& info)
{
	info.modName = "thelivingdiamond.InputOverlay"; // CHANGE ME
	info.logTag = "InputOverlay"; // CHANGE ME
	info.supportsHotReload = true; // TODO: Add comment/wiki link
}

void ModMain::InitHooks()
{
#ifdef EXAMPLE
	// Functions hooks are intalled early into mod loading process,
	// before any engine subsystem is initialized.
	// But for hook to succeed PreyFunctionHook needs to know the hook function
	// (that will be called in place of original one)
	// That's why ModMain::InitHooks exists.
	// Call SetHookFunc from here.
	s_hook_ArkPlayer_HasAbility.SetHookFunc(&ArkPlayer_HasAbility_Hook);
#endif
}

void ModMain::InitSystem(const ModInitInfo& initInfo, ModDllInfo& dllInfo)
{
	BaseClass::InitSystem(initInfo, dllInfo);
    LoadKeyMaps();
	// Your code goes here
}

void ModMain::InitGame(bool isHotReloading)
{
	BaseClass::InitGame(isHotReloading);
    m_pOverlayInputListener = std::make_unique<OverlayInputListener>();
    m_pOverlayInputListener->registerListener();

	// Your code goes here
    auto lastUsed = getLastUsedKeyMapName();
    if (lastUsed.empty())
    {
        lastUsed = "Default";
    }
    LoadKeyMap(getKeyMapNode(lastUsed));
}

void ModMain::ShutdownGame(bool isHotUnloading)
{
	// Your code goes here
	BaseClass::ShutdownGame(isHotUnloading);
    m_pOverlayInputListener.reset();
}

void ModMain::ShutdownSystem(bool isHotUnloading)
{
	// Your code goes here
	BaseClass::ShutdownSystem(isHotUnloading);
}

void ModMain::DrawMenuBar() {
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Input Overlay"))
        {
            ImGui::MenuItem("Show", NULL, &m_bDrawOverlay);
            ImGui::MenuItem("Settings", NULL, &m_bDrawSettings);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

ImGuiWindowFlags SettingsWindowFlags = ImGuiWindowFlags_MenuBar;

void ModMain::DrawSettings() {
    if(!m_bDrawSettings) return;
    ImGui::SetNextWindowBgAlpha(1.0f);
    if(ImGui::Begin("Settings", &m_bDrawSettings, SettingsWindowFlags)) {
        ImGui::Text("Key Map: %s", m_CurrentKeyMap.c_str());
        if(ImGui::Button("Save Key Map")) {
            openSaveKeyMapDialog(m_CurrentKeyMap);
        }
        ImGui::SameLine();
        if(ImGui::Button("Load Key Map")) {
            openLoadKeyMapDialog();
        }
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Load")) {
                    openLoadKeyMapDialog();
                }
                if (ImGui::MenuItem("Save")) {
                    openSaveKeyMapDialog(m_CurrentKeyMap);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::Text("Position");
        ImGui::SameLine();
        ImVec2 pos = {m_vOverlayPos.x * ImGui::GetDisplayScalar(), m_vOverlayPos.y * ImGui::GetDisplayScalar()};
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f - ImGui::GetStyle().ItemSpacing.x * 0.5f);
        if(ImGui::DragFloat("##OverlayPosX", &pos.x, 1.0f, 0.0f, ImGui::GetIO().DisplaySize.x, "%.1f")){
            m_vOverlayPos.x = pos.x / ImGui::GetDisplayScalar();
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if(ImGui::DragFloat("##OverlayPosY", &pos.y, 1.0f, 0.0f, ImGui::GetIO().DisplaySize.y, "%.1f")){
            m_vOverlayPos.y = pos.y / ImGui::GetDisplayScalar();
        }

        ImGui::Text("Size");
        ImGui::SameLine();
        ImVec2 size = {m_vOverlaySize.x * ImGui::GetDisplayScalar(), m_vOverlaySize.y * ImGui::GetDisplayScalar()};
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f - ImGui::GetStyle().ItemSpacing.x * 0.5f);
        if(ImGui::DragFloat("##SizeX", &size.x, 1.0f, 0.0f, ImGui::GetIO().DisplaySize.x, "%.1f")){
            m_vOverlaySize.x = size.x / ImGui::GetDisplayScalar();
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if(ImGui::DragFloat("##SizeY", &size.y, 1.0f, 0.0f, ImGui::GetIO().DisplaySize.y, "%.1f")){
            m_vOverlaySize.y = size.y / ImGui::GetDisplayScalar();
        }

        ImGui::NewLine();
        ImGui::ColorEdit4("Outline Color", &KeyElement::m_BorderColor.x);
        ImGui::ColorEdit4("Background Color", &KeyElement::m_FillColor.x);
        ImGui::ColorEdit4("Text Color", &KeyElement::m_TextColor.x);
        ImGui::Separator();
        static bool m_bEditingPosition = true;
        if(ImGui::RadioButton("Edit Position", m_bEditingPosition)){
            m_bEditingPosition = true;
        }
        ImGui::SameLine();
        if(ImGui::RadioButton("Edit Size", !m_bEditingPosition)){
            m_bEditingPosition = false;
        }
        if(ImGui::BeginTable("Key Elements", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_BordersInner | ImGuiTableFlags_ScrollY, ImGui::GetContentRegionAvail())) {
            ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed);
            if(m_bEditingPosition) {
                ImGui::TableSetupColumn("Position (X, Y)");
            } else {
                ImGui::TableSetupColumn("Size (X, Y)");
            }
            ImGui::TableSetupColumn("##Delete", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Mouse\nVisualizer");
            ImGui::TableNextColumn();
            if(!m_MouseVisualizer){
                if(ImGui::Button("Add")){
                    addMouseVisualizer();
                }
            } else {
                if(m_bEditingPosition) {
                    ImVec2 elementPos = {m_MouseVisualizer->m_Position.x * ImGui::GetDisplayScalar(), m_MouseVisualizer->m_Position.y * ImGui::GetDisplayScalar()};
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f - ImGui::GetStyle().ItemSpacing.x * 0.5f);
                    if (ImGui::DragFloat("##PositionXMouseVisualizer", &elementPos.x, 1.0f, 0.0f, m_vOverlaySize.x * ImGui::GetDisplayScalar(), "%.1f")) {
                        m_MouseVisualizer->m_Position.x = elementPos.x / ImGui::GetDisplayScalar();
                    }
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    if (ImGui::DragFloat("##PositionYMouseVisualizer", &elementPos.y, 1.0f, 0.0f, m_vOverlaySize.y * ImGui::GetDisplayScalar(), "%.1f")) {
                        m_MouseVisualizer->m_Position.y = elementPos.y / ImGui::GetDisplayScalar();
                    }
                } else {
                    float elementSize = m_MouseVisualizer->m_Size.x * ImGui::GetDisplayScalar();
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    if (ImGui::DragFloat("##SizeMouseVisualizer", &elementSize, 1.0f, 0.0f, m_vOverlaySize.x * ImGui::GetDisplayScalar(), "%.1f")) {
                        m_MouseVisualizer->m_Size.x = elementSize / ImGui::GetDisplayScalar();
                        m_MouseVisualizer->m_Size.y = elementSize / ImGui::GetDisplayScalar();
                    }
                }
                ImGui::TableNextColumn();
                if(ImGui::SmallButton("X##MouseVisualizer")){
                    removeMouseVisualizer();
                }
            }
            for(auto& element : m_KeyElements) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%s", EKeyIdToString.at(element.first).c_str());
                ImGui::TableNextColumn();
                if(m_bEditingPosition) {
                    ImVec2 elementPos = {element.second.m_Position.x * ImGui::GetDisplayScalar(), element.second.m_Position.y * ImGui::GetDisplayScalar()};
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f - ImGui::GetStyle().ItemSpacing.x * 0.5f);
                    if (ImGui::DragFloat(("##PositionX" + std::to_string(element.first)).c_str(), &elementPos.x, 1.0f, 0.0f, m_vOverlaySize.x * ImGui::GetDisplayScalar(), "%.1f")) {
                        element.second.m_Position.x = elementPos.x / ImGui::GetDisplayScalar();
                    }
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    if (ImGui::DragFloat(("##PositionY" + std::to_string(element.first)).c_str(), &elementPos.y, 1.0f, 0.0f, m_vOverlaySize.y * ImGui::GetDisplayScalar(), "%.1f")) {
                        element.second.m_Position.y = elementPos.y / ImGui::GetDisplayScalar();
                    }
                } else {
                    ImVec2 elementSize = {element.second.m_Size.x * ImGui::GetDisplayScalar(), element.second.m_Size.y * ImGui::GetDisplayScalar()};
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f - ImGui::GetStyle().ItemSpacing.x * 0.5f);
                    if (ImGui::DragFloat(("##SizeX" + std::to_string(element.first)).c_str(), &elementSize.x, 1.0f, 0.0f, m_vOverlaySize.x * ImGui::GetDisplayScalar(), "%.1f")) {
                        element.second.m_Size.x = elementSize.x / ImGui::GetDisplayScalar();
                    }
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    if (ImGui::DragFloat(("##SizeY" + std::to_string(element.first)).c_str(), &elementSize.y, 1.0f, 0.0f, m_vOverlaySize.y * ImGui::GetDisplayScalar(), "%.1f")) {
                        element.second.m_Size.y = elementSize.y / ImGui::GetDisplayScalar();
                    }
                }
                ImGui::TableNextColumn();
                if(ImGui::SmallButton(("X##" + std::to_string(element.first)).c_str())){
                    RemoveKeyFromMap(element.first);
                }
            }
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if(ImGui::Selectable("Add Key")) {
                openAddKeyDialog();
            }
            ImGui::EndTable();
        }
    }
    ImGui::End();

}

const ImGuiWindowFlags Overlay_Flags = ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoBringToFrontOnFocus;


void ModMain::DrawOverlay() {
    if(!m_bDrawOverlay && !m_bDrawSettings) return;

    ImGui::SetNextWindowPos(ImVec2{m_vOverlayPos.x * ImGui::GetDisplayScalar(), m_vOverlayPos.y * ImGui::GetDisplayScalar()});
    ImGui::SetNextWindowSize(ImVec2{m_vOverlaySize.x * ImGui::GetDisplayScalar(), m_vOverlaySize.y * ImGui::GetDisplayScalar()});
    if(ImGui::Begin("Input Overlay", &m_bDrawOverlay, Overlay_Flags)) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        // draw a rectangle around the window when the settings window is open
        ImVec2 min = ImGui::GetWindowContentRegionMin();
        ImVec2 max = ImGui::GetWindowContentRegionMax();
        ImVec2 pos = ImGui::GetWindowPos();
        if(m_bDrawSettings) {
            min.x += pos.x;
            min.y += pos.y;
            max.x += pos.x;
            max.y += pos.y;
            draw_list->AddRect(min, max, IM_COL32(255, 255, 255, 255), 0.0f, 0, 0.5f);
        }
        std::scoped_lock lock(m_KeyElementsMutex);
        for(auto& element : m_KeyElements) {
            element.second.draw(draw_list, pos);
        }
        if(m_MouseVisualizer){
            m_MouseVisualizer->draw(draw_list, pos);
        }
    }
    ImGui::End();
    //TODO: Draw overlay

}


void ModMain::Draw()
{
    DrawMenuBar();
    DrawSettings();
    DrawSavePopup();
    DrawLoadPopup();
    DrawKeyAddPopup();

}

void ModMain::MainUpdate(unsigned updateFlags)
{
	// Your code goes here
    DrawOverlay();
    // hope for the best
}



void *ModMain::QueryInterface(const char *ifaceName) {
#ifdef EXAMPLE
    // this is used to return an interface for your mod, if available.
    // Your mod class should inherit from the interface class. i.e: class ModMain : public ChairloaderModBase, public IExampleMod {
    // Then you can return the interface pointer here.
    if (!strcmp(ifaceName, "ExampleMod"))
        return static_cast<IExampleMod*>(this);
    // If you have multiple interfaces, you can return as many as you want for even potentially different objects.
    // if you don't have an interface, just return nullptr.
#endif
    return nullptr;
}

void ModMain::Connect(const std::vector<IChairloaderMod *> &mods) {
#ifdef EXAMPLE
    // Example of how to get a mod interface from the list of mods
    IOtherMod* otherMod = nullptr;
    for (auto & mod: mods) {
        otherMod = mod->QueryInterface("IOtherMod001"); // the interface name is defined in the other mod
        if (otherMod) {
            break;
        }
    }

    // do something with otherMod
#endif
}

void ModMain::setKeyState(EKeyId key, EInputState state) {
    std::scoped_lock lock(m_KeyElementsMutex);
    if(m_KeyElements.find(key) != m_KeyElements.end()) {
        m_KeyElements.at(key).setState(state);
    }
}

void ModMain::LoadKeyMaps() {
    // double check the directory exists
    if(!fs::exists(m_KeyMapDocPath.parent_path())) {
        try {
            fs::create_directories(m_KeyMapDocPath.parent_path());
        } catch (fs::filesystem_error &e) {
            CryError("Failed to create keymap directory: {}", e.what());
            return;
        }
    }
    if(!fs::exists(m_KeyMapDocPath)) {
        try {
            fs::copy(m_DefaultKeyMapDocPath, m_KeyMapDocPath);
        } catch (fs::filesystem_error &e) {
            CryError("Failed to copy default keymap file: {}", e.what());
        }
    }
    auto success = m_KeyMapDoc.load_file(m_KeyMapDocPath.wstring().c_str(), pugi::parse_default | pugi::parse_comments);
    if(!success) {
        CryError("Failed to load keymap file: {}", success.description());
        return;
    }
    m_KeyMapRoot = m_KeyMapDoc.first_child();
    if(!m_KeyMapRoot) {
        CryError("Failed to load keymap file: Root node is null");
        return;
    }
}

void ModMain::SaveKeyMaps() {
    // double check the directory exists
    if(!fs::exists(m_KeyMapDocPath.parent_path())) {
        try {
            fs::create_directories(m_KeyMapDocPath.parent_path());
        } catch (fs::filesystem_error &e) {
            CryError("Failed to create keymap directory: {}", e.what());
            return;
        }
    }
    auto success = m_KeyMapDoc.save_file(m_KeyMapDocPath.wstring().c_str());
    if(!success) {
        CryError("Failed to save keymap file: {}", m_KeyMapDocPath.u8string());
    }
}

static const auto LOAD_POPUP_SIZE = ImVec2(0.3f, 0.6f);

void ModMain::DrawLoadPopup() {
    auto popupSize = ImVec2(LOAD_POPUP_SIZE.x * ImGui::GetDisplayScalar(), LOAD_POPUP_SIZE.y * ImGui::GetDisplayScalar());
    if(m_bDrawLoadPopup) {
        ImGui::OpenPopup("Load Keymap");
        m_bDrawLoadPopup = false;
    }
    static bool m_bDrawNewKeymapPopup = false;
    bool keepOpen = true;

    ImGui::SetNextWindowBgAlpha(1.0f);
    ImGui::SetNextWindowSize(popupSize, ImGuiCond_Appearing);
    // center the popup
    ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - popupSize.x) / 2, (ImGui::GetIO().DisplaySize.y - popupSize.y) / 2), ImGuiCond_Appearing);
    if(ImGui::BeginPopupModal("Load Keymap", &keepOpen)) {
        if(ImGui::Button("Reload Key Maps File")){
            LoadKeyMaps();
        }
        if(ImGui::BeginTable("Keymap List", 1, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImGui::GetContentRegionAvail())) {
            ImGui::TableSetupColumn("Name");
            ImGui::TableHeadersRow();
            for(auto& keymap : m_KeyMapRoot.children("KeyMap")) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Bullet();
                ImGui::SameLine();
                if(ImGui::Selectable(keymap.attribute("name").value(), false, ImGuiSelectableFlags_SpanAllColumns)) {
                    LoadKeyMap(keymap);
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if(ImGui::Button("New Keymap")) {
                m_bDrawNewKeymapPopup = true;
            }
            ImGui::EndTable();
        }
        if(m_bDrawNewKeymapPopup){
            ImGui::OpenPopup("Add New Keymap");
            m_bDrawNewKeymapPopup = false;
        }
        if(ImGui::BeginPopupContextItem("Add New Keymap")) {
            ImGui::InputText("Name", &m_newKeymapName);
            if(ImGui::Button("Add")){
                getKeyMapNode(m_newKeymapName);
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if(ImGui::Button("Cancel")){
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        if(!keepOpen) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

}

static const auto SAVE_POPUP_SIZE = ImVec2(0.3f, 0.3f);

void ModMain::DrawSavePopup() {
    auto popupSize = ImVec2(SAVE_POPUP_SIZE.x * ImGui::GetDisplayScalar(), SAVE_POPUP_SIZE.y * ImGui::GetDisplayScalar());
    if(m_bDrawSavePopup) {
        ImGui::OpenPopup("Save Keymap");
        m_bDrawSavePopup = false;
    }
    bool keepOpen = true;
    static bool m_bDrawConfirmOverwritePopup = false;
    ImGui::SetNextWindowBgAlpha(1.0f);
    ImGui::SetNextWindowSize(popupSize, ImGuiCond_Appearing);
    // center the popup
    ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - popupSize.x) / 2, (ImGui::GetIO().DisplaySize.y - popupSize.y) / 2), ImGuiCond_Appearing);
    if(ImGui::BeginPopupModal("Save Keymap", &keepOpen)) {
        ImGui::InputText("Name", &m_newKeymapName);
        if(ImGui::Button("Save")){
            if(!doesKeyMapExist(m_newKeymapName)) {
                SaveKeyMap(getKeyMapNode(m_newKeymapName));
                ImGui::CloseCurrentPopup();
            } else {
                m_bDrawConfirmOverwritePopup = true;
            }
        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel")){
            ImGui::CloseCurrentPopup();
        }
        if(m_bDrawConfirmOverwritePopup) {
            ImGui::OpenPopup("Confirm Overwrite");
            m_bDrawConfirmOverwritePopup = false;
        }
        if(ImGui::BeginPopupModal("Confirm Overwrite", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("A keymap with that name already exists. Overwrite?");
            if(ImGui::Button("Yes")) {
                SaveKeyMap(getKeyMapNode(m_newKeymapName));
                ImGui::CloseCurrentPopup();
                keepOpen = false;
            }
            ImGui::SameLine();
            if(ImGui::Button("No")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        if(!keepOpen) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}


void ModMain::LoadKeyMap(const pugi::xml_node keyMapNode) {
    {
        std::scoped_lock lock(m_KeyElementsMutex);
        m_KeyElements.clear();
        if (keyMapNode.attribute("overlayPos")) {
            m_vOverlayPos = ImGui::StringToImVec2(keyMapNode.attribute("overlayPos").value());
        }
        if (keyMapNode.attribute("overlaySize")) {
            m_vOverlaySize = ImGui::StringToImVec2(keyMapNode.attribute("overlaySize").value());
        }
        if (keyMapNode.attribute("outlineColor")) {
            KeyElement::m_BorderColor = ImGui::StringToImVec4(keyMapNode.attribute("outlineColor").value());
        }
        if (keyMapNode.attribute("fillColor")) {
            KeyElement::m_FillColor = ImGui::StringToImVec4(keyMapNode.attribute("fillColor").value());
        }
        if (keyMapNode.attribute("textColor")) {
            KeyElement::m_TextColor = ImGui::StringToImVec4(keyMapNode.attribute("textColor").value());
        }
        for (auto &keyNode: keyMapNode.children("Key")) {
            auto key = static_cast<EKeyId>(keyNode.attribute("id").as_int());
            auto keyElement = KeyElement(key);
            keyElement.m_Position = ImGui::StringToImVec2(keyNode.attribute("pos").value());
            keyElement.m_Size = ImGui::StringToImVec2(keyNode.attribute("size").value());
            m_KeyElements.emplace(key, keyElement);
        }
        if(keyMapNode.child("MouseVisualizer")){
            addMouseVisualizer();
            m_MouseVisualizer->m_Position = ImGui::StringToImVec2(keyMapNode.child("MouseVisualizer").attribute("pos").value());
            m_MouseVisualizer->m_Size = ImGui::StringToImVec2(keyMapNode.child("MouseVisualizer").attribute("size").value());
        }
    }
    m_CurrentKeyMap = keyMapNode.attribute("name").value();
    setLastUsedKeyMapName(m_CurrentKeyMap);
}

void ModMain::SaveKeyMap(pugi::xml_node keyMapNode) {
    if(!keyMapNode) {
        return;
    }
    {
        std::scoped_lock lock(m_KeyElementsMutex);
        if (!keyMapNode.attribute("overlayPos")) {
            keyMapNode.append_attribute("overlayPos");
        }
        keyMapNode.attribute("overlayPos").set_value(ImGui::ImVec2ToString(m_vOverlayPos).c_str());
        if (!keyMapNode.attribute("overlaySize")) {
            keyMapNode.append_attribute("overlaySize");
        }
        keyMapNode.attribute("overlaySize").set_value(ImGui::ImVec2ToString(m_vOverlaySize).c_str());
        if (!keyMapNode.attribute("outlineColor")) {
            keyMapNode.append_attribute("outlineColor");
        }
        keyMapNode.attribute("outlineColor").set_value(ImGui::ImVec4ToString(KeyElement::m_BorderColor).c_str());
        if (!keyMapNode.attribute("fillColor")) {
            keyMapNode.append_attribute("fillColor");
        }
        keyMapNode.attribute("fillColor").set_value(ImGui::ImVec4ToString(KeyElement::m_FillColor).c_str());
        if (!keyMapNode.attribute("textColor")) {
            keyMapNode.append_attribute("textColor");
        }
        keyMapNode.attribute("textColor").set_value(ImGui::ImVec4ToString(KeyElement::m_TextColor).c_str());
        // remove the children
        keyMapNode.remove_children();
        for (auto &keyElement: m_KeyElements) {
            auto keyNode = keyMapNode.append_child("Key");
            keyNode.append_attribute("id").set_value(fmt::format("0x{:X}", keyElement.first).c_str());
            keyNode.append_attribute("pos").set_value(ImGui::ImVec2ToString(keyElement.second.m_Position).c_str());
            keyNode.append_attribute("size").set_value(ImGui::ImVec2ToString(keyElement.second.m_Size).c_str());
        }
        if(m_MouseVisualizer) {
            auto MouseVisualizerNode = keyMapNode.append_child("MouseVisualizer");
            MouseVisualizerNode.append_attribute("pos").set_value(ImGui::ImVec2ToString(m_MouseVisualizer->m_Position).c_str());
            MouseVisualizerNode.append_attribute("size").set_value(ImGui::ImVec2ToString(m_MouseVisualizer->m_Size).c_str());
        }
    }
    SaveKeyMaps();
    LoadKeyMap(keyMapNode);
}

void ModMain::AddKeyToMap(EKeyId key) {
    std::scoped_lock lock(m_KeyElementsMutex);
    if (m_KeyElements.find(key) == m_KeyElements.end()) {
        m_KeyElements.emplace(key, KeyElement(key));
    }
}

void ModMain::RemoveKeyFromMap(EKeyId key) {
    std::scoped_lock lock(m_KeyElementsMutex);
    m_KeyElements.erase(key);
}

pugi::xml_node ModMain::getKeyMapNode(std::string &keyMapName) {
    for(auto &keymap : m_KeyMapRoot.children("KeyMap")) {
        if(keymap.attribute("name").value() == keyMapName) {
            return keymap;
        }
    }
    return createKeyMapNode(keyMapName);
}

pugi::xml_node ModMain::createKeyMapNode(std::string &keyMapName) {
    auto node = m_KeyMapRoot.append_child("KeyMap");
    node.append_attribute("name").set_value(keyMapName.c_str());
    return node;
}

bool ModMain::doesKeyMapExist(std::string &keyMapName) {
    for(auto &keymap : m_KeyMapRoot.children("KeyMap")) {
        if(keymap.attribute("name").value() == keyMapName) {
            return true;
        }
    }
    return false;
}

std::string ModMain::getLastUsedKeyMapName() {
    if(m_KeyMapRoot.attribute("lastUsed")) {
        return m_KeyMapRoot.attribute("lastUsed").value();
    }
    return "";
}

void ModMain::setLastUsedKeyMapName(std::string &keyMapName) {
    if(!m_KeyMapRoot.attribute("lastUsed")) {
        m_KeyMapRoot.append_attribute("lastUsed");
    }
    m_KeyMapRoot.attribute("lastUsed").set_value(keyMapName.c_str());
    SaveKeyMaps();
}

void ModMain::openSaveKeyMapDialog(std::string keyMapName) {
    m_bDrawSavePopup = true;
    m_newKeymapName = keyMapName;

}

void ModMain::openLoadKeyMapDialog() {
    m_bDrawLoadPopup = true;
}

static const auto KEY_ADD_POPUP_SIZE = ImVec2(0.4, 0.8f);
void ModMain::DrawKeyAddPopup() {
    auto popupSize = ImVec2(KEY_ADD_POPUP_SIZE.x * ImGui::GetDisplayScalar(), KEY_ADD_POPUP_SIZE.y * ImGui::GetDisplayScalar());
    if(m_bDrawKeyAddPopup) {
        ImGui::OpenPopup("Add Key");
        m_bDrawKeyAddPopup = false;
    }
    bool keepOpen = true;
    static bool m_bDrawConfirmOverwritePopup = false;
    ImGui::SetNextWindowBgAlpha(1.0f);
    ImGui::SetNextWindowSize(popupSize, ImGuiCond_Appearing);
    // center the popup
    ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - popupSize.x) / 2, (ImGui::GetIO().DisplaySize.y - popupSize.y) / 2), ImGuiCond_Appearing);
    if(ImGui::BeginPopupContextWindow("Add Key")){
        static ImGuiTextFilter filter;
        filter.Draw("Filter Keys");
        if(ImGui::BeginTable("Key Table", 1, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn("Key");
            ImGui::TableHeadersRow();
            for(auto&key : EKeyIdToString){
                if(m_KeyElements.find(key.first) == m_KeyElements.end() && filter.PassFilter(key.second.c_str())) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if(ImGui::Selectable(key.second.c_str())) {
                        AddKeyToMap(key.first);
                    }
                }
            }
            ImGui::EndTable();
        }
        ImGui::EndPopup();
    }

}

void ModMain::openAddKeyDialog() {
    m_bDrawKeyAddPopup = true;
}

void ModMain::addMouseVisualizer() {
    m_MouseVisualizer = std::make_unique<MouseVisualizer>();
}

void ModMain::removeMouseVisualizer() {
    m_MouseVisualizer.reset();
}

void ModMain::setMouseState(EKeyId key, float val) {
    if(m_MouseVisualizer) {
        m_MouseVisualizer->updateState(key, val);
    }
}

void ModMain::LateUpdate(unsigned int updateFlags) {
    if(m_MouseVisualizer) {
        m_MouseVisualizer->Update();
    }
}


extern "C" DLL_EXPORT IChairloaderMod* ClMod_Initialize()
{
	CRY_ASSERT(!gMod);
	gMod = new ModMain();
	return gMod;
}

extern "C" DLL_EXPORT void ClMod_Shutdown()
{
	CRY_ASSERT(gMod);
	delete gMod;
	gMod = nullptr;
}

// Validate that declarations haven't changed
static_assert(std::is_same_v<decltype(ClMod_Initialize), IChairloaderMod::ProcInitialize>);
static_assert(std::is_same_v<decltype(ClMod_Shutdown), IChairloaderMod::ProcShutdown>);
