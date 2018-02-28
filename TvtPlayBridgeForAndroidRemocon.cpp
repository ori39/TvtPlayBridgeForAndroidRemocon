// TvtPlayBridgeForAndroidRemocon.cpp : DLL �A�v���P�[�V�����p�ɃG�N�X�|�[�g�����֐����`���܂��B
//

#include "stdafx.h"

#define TVTEST_PLUGIN_CLASS_IMPLEMENT
#define TVTEST_PLUGIN_VERSION TVTEST_PLUGIN_VERSION_(0,0,13)
#include "TVTestPlugin.h"
#include "TvtPlayBridgeForAndroidRemocon.h"

static LPCWSTR INFO_PLUGIN_NAME = L"TvtPlayBridgeForAndroidRemocon";
static LPCWSTR INFO_DESCRIPTION = L"AndroidRemocon����TvtPlay�𐧌�ł���悤�ɂ���v���O�C��";

// TvtPlay���瑼�v���O�C���ɏ��񋟂��郁�b�Z�[�W(From: TvtPlay.cpp)
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
    // �v���O�C������Ăԏꍇ��SendMessage��OK
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
    // �v���O�C���̏���Ԃ�
    pInfo->Type           = TVTest::PLUGIN_TYPE_NORMAL;
    pInfo->Flags          = 0;
    pInfo->pszPluginName  = INFO_PLUGIN_NAME;
    pInfo->pszCopyright   = L"Public Domain";
    pInfo->pszDescription = INFO_DESCRIPTION;
    return true;
}


// ����������
bool CTvtPlayBridge::Initialize()
{
    // �C�x���g�R�[���o�b�N�֐���o�^
    m_pApp->SetEventCallback(EventCallback, this);

	// TvTest�����R���p
	m_Message = ::RegisterWindowMessage(TEXT("TvtPlayRemocon")); 
    return true;
}


// �I������
bool CTvtPlayBridge::Finalize()
{
    if (m_pApp->IsPluginEnabled())
	{
		EnablePlugin(false);
	}
    return true;
}

// �v���O�C���̗L����Ԃ��ω�����
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

// �C�x���g�R�[���o�b�N�֐�
// �����C�x���g���N����ƌĂ΂��
LRESULT CALLBACK CTvtPlayBridge::EventCallback(UINT Event, LPARAM lParam1, LPARAM lParam2, void *pClientData)
{
    CTvtPlayBridge *pThis = static_cast<CTvtPlayBridge*>(pClientData);

    switch (Event) {
    case TVTest::EVENT_PLUGINENABLE:
        // �v���O�C���̗L����Ԃ��ω�����
        return pThis->EnablePlugin(lParam1 != 0);
		break;
    }
    return 0;
}

// �E�B���h�E���b�Z�[�W�R�[���o�b�N�֐�
// TRUE��Ԃ���TVTest���Ń��b�Z�[�W���������Ȃ��Ȃ�
// WM_CREATE�͌Ă΂�Ȃ�
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



