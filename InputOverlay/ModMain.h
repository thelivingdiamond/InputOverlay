#pragma once
#include <ChairLoader/ModSDK/ChairloaderModBase.h>
#include <pugixml.hpp>
struct KeyElement;
struct MouseVisualizer;
class OverlayInputListener;

class ModMain : public ChairloaderModBase
{
	using BaseClass = ChairloaderModBase;

	//! Fills in the DLL info during initialization.
	virtual void FillModInfo(ModDllInfoEx& info) override;

    //! Retrieves an interface for the mod.
    void* QueryInterface(const char *ifaceName) override;

	//! Initializes function hooks before they are installed.
	virtual void InitHooks() override;

	//! Called during CSystem::Init, before any engine modules.
	//! Call order: TODO
	virtual void InitSystem(const ModInitInfo& initInfo, ModDllInfo& dllInfo) override;

    //! Called after CSystem::Init, after all engine modules and mods have been initialized. Allows your mod to get interfaces from other mods.
    void Connect(const std::vector<IChairloaderMod *> &mods) override;
public:
    void setKeyState(EKeyId key, EInputState state);
    void setMouseState(EKeyId key, float val);

private:
    //! Called after CGame::Init
	//! Call order: TODO
	virtual void InitGame(bool isHotReloading) override;

	//! Called before CGame::Shutdown.
	//! Call order: TODO
	virtual void ShutdownGame(bool isHotUnloading) override;

	//! Called before CSystem::Shutdown.
	//! Call order: TODO
	virtual void ShutdownSystem(bool isHotUnloading) override;

    void DrawMenuBar();
    void DrawSettings();
    void DrawOverlay();

	//! Called just before MainUpdate to draw GUI. Only called when GUI is visible.
	virtual void Draw() override;

	//! Earliest point of update in a frame, before CGame::Update. The timer still tracks time for the previous frame.
	// virtual void UpdateBeforeSystem(unsigned updateFlags) override;

	//! Called before physics is updated for the new frame, best point for queuing physics jobs.
	//! This is like FixedUpdate() in Unity (but not FPS-independent). Use gEnv->pTimer->GetFrameTime() for time delta.
	// virtual void UpdateBeforePhysics(unsigned updateFlags) override;

	//! Called after entities have been updated but before FlowSystem and FlashUI.
	//! This is the main update where most game logic is expected to occur.
	//! Should be preferred if you don't need any special behavior.
	virtual void MainUpdate(unsigned updateFlags) override;

	//! Called after most of game logic has been updated, before CCryAction::PostUpdate.
	 virtual void LateUpdate(unsigned updateFlags) override;


    bool m_bDrawSettings = false;
    bool m_bDrawOverlay = true;

    ImVec2 m_vOverlayPos = ImVec2(0.3f, 0.5f);
    ImVec2 m_vOverlaySize = ImVec2(0.3f, 0.5f);

    std::mutex m_KeyElementsMutex;
    std::map<EKeyId, KeyElement> m_KeyElements;
    std::unique_ptr<MouseVisualizer> m_MouseVisualizer;
    std::unique_ptr<OverlayInputListener> m_pOverlayInputListener;

    fs::path m_KeyMapDocPath = "Mods/config/InputOverlay/KeyMaps.xml";
    fs::path m_DefaultKeyMapDocPath = "Mods/thelivingdiamond.InputOverlay/DefaultKeyMaps.xml";

    pugi::xml_document m_KeyMapDoc;
    pugi::xml_node m_KeyMapRoot;

    void LoadKeyMaps();
    void SaveKeyMaps();

    void LoadKeyMap(const pugi::xml_node keyMapNode);
    void SaveKeyMap(pugi::xml_node keyMapNode);
    pugi::xml_node getKeyMapNode(std::string& keyMapName);
    pugi::xml_node createKeyMapNode(std::string& keyMapName);
    bool doesKeyMapExist(std::string& keyMapName);



    std::string getLastUsedKeyMapName();
    void setLastUsedKeyMapName(std::string& keyMapName);

    std::string m_CurrentKeyMap = "Default";
    std::string m_newKeymapName;

    void AddKeyToMap(EKeyId key);
    void RemoveKeyFromMap(EKeyId key);
    void addMouseVisualizer();
    void removeMouseVisualizer();

    bool m_bDrawLoadPopup = false,
            m_bDrawSavePopup = false,
            m_bDrawKeyAddPopup = false;
    void DrawLoadPopup();
    void DrawSavePopup();
    void DrawKeyAddPopup();

    void openSaveKeyMapDialog(std::string keyMapName = "");
    void openLoadKeyMapDialog();
    void openAddKeyDialog();



};

extern ModMain* gMod;
