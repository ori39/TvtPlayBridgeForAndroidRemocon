// TvtPlayBridgeForAndroidRemocon.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"

#define TVTEST_PLUGIN_CLASS_IMPLEMENT
#define TVTEST_PLUGIN_VERSION TVTEST_PLUGIN_VERSION_(0,0,13)
#include "TVTestPlugin.h"
#include "TvtPlayBridgeForAndroidRemocon.h"

static LPCWSTR INFO_PLUGIN_NAME = L"TvtPlayBridgeForAndroidRemocon";
static LPCWSTR INFO_DESCRIPTION = L"AndroidRemoconからTvtPlayを制御できるようにするプラグイン";

// TvtPlayから他プラグインに情報提供するメッセージ(From: TvtPlay.cpp)
#define TVTP_CURRENT_MSGVER		1
#define WM_TVTP_GET_MSGVER		(WM_APP + 50)
#define WM_TVTP_IS_OPEN			(WM_APP + 51)
#define WM_TVTP_GET_POSITION	(WM_APP + 52)
#define WM_TVTP_GET_DURATION	(WM_APP + 53)
#define WM_TVTP_GET_TOT_TIME	(WM_APP + 54)
#define WM_TVTP_IS_EXTENDING	(WM_APP + 55)
#define WM_TVTP_IS_PAUSED		(WM_APP + 56)
#define WM_TVTP_GET_PLAY_FLAGS	(WM_APP + 57)
#define WM_TVTP_GET_STRETCH		(WM_APP + 58)
#define WM_TVTP_GET_PATH		(WM_APP + 59)
#define WM_TVTP_SEEK			(WM_APP + 60)
#define WM_TVTP_SEEK_ABSOLUTE	(WM_APP + 61)


static BOOL CALLBACK FindTvtPlayFrameEnumProc(HWND hwnd, LPARAM lParam)
{
    TCHAR className[32];
	DWORD dwProcessID = 0;
	DWORD dwCurrProcessID = 0;

	dwCurrProcessID = ::GetCurrentProcessId();
	::GetWindowThreadProcessId(hwnd, &dwProcessID);

	if (dwCurrProcessID == dwProcessID)
	{
		if (GetClassName(hwnd, className, _countof(className)) && !lstrcmp(className, TEXT("TvtPlay Frame"))) {
			*(HWND*)lParam = hwnd;
			return FALSE;
		}
	}
    return TRUE;
}

static HWND FindTvtPlayFrame()
{
    HWND hwnd = NULL;
    EnumWindows(FindTvtPlayFrameEnumProc, (LPARAM)&hwnd); // Call from another process
    return hwnd;
}


static DWORD MySendMessage(HWND hwnd, UINT msg, WPARAM wParam = 0, LPARAM lParam = 0)
{
    // プラグインから呼ぶ場合はSendMessageでOK
    DWORD_PTR dwpResult = 0;
    if (SendMessageTimeout(hwnd, msg, wParam, lParam, SMTO_NORMAL, 5000, &dwpResult)) {
        return (DWORD)dwpResult;
    }
    return 0;
}


CTvtPlayBridge::CTvtPlayBridge()
    : m_Message(0)
{
}

CTvtPlayBridge::~CTvtPlayBridge()
{
}

bool CTvtPlayBridge::GetPluginInfo(TVTest::PluginInfo *pInfo)
{
    // プラグインの情報を返す
    pInfo->Type           = TVTest::PLUGIN_TYPE_NORMAL;
    pInfo->Flags          = 0;
    pInfo->pszPluginName  = INFO_PLUGIN_NAME;
    pInfo->pszCopyright   = L"Public Domain";
    pInfo->pszDescription = INFO_DESCRIPTION;
    return true;
}


// 初期化処理
bool CTvtPlayBridge::Initialize()
{
    // イベントコールバック関数を登録
    m_pApp->SetEventCallback(EventCallback, this);

	// TvTestリモコン用
	m_Message = ::RegisterWindowMessage(TEXT("TvtPlayRemocon")); 
    return true;
}


// 終了処理
bool CTvtPlayBridge::Finalize()
{
    if (m_pApp->IsPluginEnabled())
	{
		EnablePlugin(false);
	}
    return true;
}

// プラグインの有効状態が変化した
bool CTvtPlayBridge::EnablePlugin(bool fEnable)
{
    if (fEnable) {
        m_pApp->SetWindowMessageCallback(WindowMsgCallback, this);
    }
    else
	{
        m_pApp->SetWindowMessageCallback(NULL, NULL);
    }
    return true;
}

// イベントコールバック関数
// 何かイベントが起きると呼ばれる
LRESULT CALLBACK CTvtPlayBridge::EventCallback(UINT Event, LPARAM lParam1, LPARAM lParam2, void *pClientData)
{
    CTvtPlayBridge *pThis = static_cast<CTvtPlayBridge*>(pClientData);

    switch (Event) {
    case TVTest::EVENT_PLUGINENABLE:
        // プラグインの有効状態が変化した
        return pThis->EnablePlugin(lParam1 != 0);
		break;
    }
    return 0;
}

// ウィンドウメッセージコールバック関数
// TRUEを返すとTVTest側でメッセージを処理しなくなる
// WM_CREATEは呼ばれない
BOOL CALLBACK CTvtPlayBridge::WindowMsgCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *pResult, void *pUserData)
{
    CTvtPlayBridge *pThis = static_cast<CTvtPlayBridge*>(pUserData);

	if (uMsg == pThis->m_Message)
	{
		HWND hTvtPlayWnd = NULL;

		hTvtPlayWnd = FindTvtPlayFrame();
		if (hTvtPlayWnd != NULL)
		{
			switch(wParam)
			{
			case 0:
//				OutputDebugString(L"WM_TVTP_GET_DURATION\n");
				*pResult = MySendMessage(hTvtPlayWnd, WM_TVTP_GET_DURATION);
				break;
			case 1:
//				OutputDebugString(L"WM_TVTP_SEEK_ABSOLUTE\n");
				MySendMessage(hTvtPlayWnd, WM_TVTP_SEEK_ABSOLUTE, 0, lParam);
				break;
			case 2:
//				OutputDebugString(L"WM_TVTP_SEEK\n");
				MySendMessage(hTvtPlayWnd, WM_TVTP_SEEK, 0, lParam);
				break;
			case 3:
//				OutputDebugString(L"WM_TVTP_GET_POSITION\n");
				*pResult = MySendMessage(hTvtPlayWnd, WM_TVTP_GET_POSITION);
				break;
			default:
				break;
			}
		}

		return TRUE;
	}

    return FALSE;
}


TVTest::CTVTestPlugin *CreatePluginClass()
{
    return new CTvtPlayBridge;
}



