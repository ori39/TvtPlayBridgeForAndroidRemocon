#ifndef INCLUDE_TVTPLAYFORANDROIDREMOCON_H
#define INCLUDE_TVTPLAYFORANDROIDREMOCON_H

// プラグインクラス
class CTvtPlayBridge : public TVTest::CTVTestPlugin
{
public:
    // CTVTestPlugin
    CTvtPlayBridge();
    ~CTvtPlayBridge();
    bool GetPluginInfo(TVTest::PluginInfo *pInfo);
    bool Initialize();
    bool Finalize();

private:
    bool EnablePlugin(bool fEnable);
    static LRESULT CALLBACK EventCallback(UINT Event, LPARAM lParam1, LPARAM lParam2, void *pClientData);
    static BOOL CALLBACK WindowMsgCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *pResult, void *pUserData);

	UINT m_Message;
};

#endif // INCLUDE_TVTPLAYFORANDROIDREMOCON_H
