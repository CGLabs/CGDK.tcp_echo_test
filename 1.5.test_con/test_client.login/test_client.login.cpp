#include "pch.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

// test_client_app
BEGIN_MESSAGE_MAP(test_client_app, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

test_client_app::test_client_app()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

test_client_app theApp;
test_client_dlg* g_pdlg;

BOOL test_client_app::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	CShellManager *pShellManager = new CShellManager;

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("CGCII.match_making_test_client"));

	object_ptr<test_client_dlg> pdlg = new test_client_dlg();
	m_pMainWnd = pdlg;
	g_pdlg = pdlg;
	pdlg->DoModal();

	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif
	return FALSE;
}

