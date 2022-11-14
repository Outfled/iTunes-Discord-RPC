
// iTunes Discord RPC 2Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "iTunes Discord RPC 2.h"
#include "iTunes Discord RPC 2Dlg.h"
#include "afxdialogex.h"
#include "AppReg.h"
#include "iTunesThread.h"
#include "iTunesEvents.h"
#include "DiscordStatus.h"
#include "AppTray.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

static CWnd *GetControlPosition( int nCtlID, LPRECT lpr, CWnd *pWnd)
{
	/* Get dialogue window position */
	if (nCtlID == 0) {
		pWnd->GetClientRect( lpr );
		return NULL;
	}

	CWnd *pCtlWnd = pWnd->GetDlgItem( nCtlID );

	pCtlWnd->GetWindowRect( lpr );
	pWnd->ScreenToClient( lpr );

	return pCtlWnd;
}

static VOID AlignDlgControls( CDialogEx *pWnd )
{
	RECT DlgRect;
	RECT TitleRect;
	RECT VersionRect;
	
	// Get dialogue position
	GetControlPosition( 0, &DlgRect, pWnd );

	// Title and version text positions
	CWnd *pTitleWnd	= GetControlPosition( IDC_STATIC_TITLETEXT, &TitleRect, pWnd );
	CWnd *pVersionWnd =	GetControlPosition( IDC_STATIC_VERSIONTEXT, &VersionRect, pWnd );

	TitleRect.left = 1;

	/* Attach each control to top of dialogue */
	TitleRect.top	= 1;
	VersionRect.top = 1;

	/* Set each text to have same height */
	TitleRect.bottom = VersionRect.bottom;
		
	pTitleWnd->MoveWindow( &TitleRect );
	pVersionWnd->MoveWindow( &VersionRect );

	/* Set title 2 */
	pTitleWnd = GetControlPosition( IDC_STATIC_AUTHORTEXT, &TitleRect, pWnd );
	TitleRect.left = 1;
	pTitleWnd->MoveWindow( &TitleRect );

	RECT EnabledCBRect;
	RECT TrayCBRect;
	RECT StartupCBRect;

	/* Update each check box */
	CWnd *pEnabledCB = GetControlPosition( IDC_CHECKBOX_ENABLED, &EnabledCBRect, pWnd );
	CWnd *pTrayCB = GetControlPosition( IDC_CHECKBOX_TRAYENABLED, &TrayCBRect, pWnd );
	CWnd *pStartupCB = GetControlPosition( IDC_CHECKBOX_STARTUPAPP, &StartupCBRect, pWnd );

	EnabledCBRect.left	= 1;
	StartupCBRect.left	= 1;
	TrayCBRect.bottom	= StartupCBRect.bottom;
	TrayCBRect.top		= StartupCBRect.top;

	pEnabledCB->MoveWindow( &EnabledCBRect );
	pTrayCB->MoveWindow( &TrayCBRect );
	pStartupCB->MoveWindow( &StartupCBRect );

	/* Update button position */
	RECT CloseBtnRect;
	CWnd *pButton = GetControlPosition( IDC_BUTTON_CLOSE, &CloseBtnRect, pWnd);

	CloseBtnRect.right = DlgRect.right;
	CloseBtnRect.left = 0;

	pButton->MoveWindow( &CloseBtnRect );
}


static HANDLE g_hTrayThread;
static HANDLE g_hITunesThread;

// CiTunesDiscordRPC2Dlg dialog

CiTunesDiscordRPC2Dlg::CiTunesDiscordRPC2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ITUNES_DISCORD_RPC_2_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CiTunesDiscordRPC2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CiTunesDiscordRPC2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED( IDC_CHECKBOX_STARTUPAPP, &CiTunesDiscordRPC2Dlg::OnBnClickedCheckboxStartupapp )
	ON_BN_CLICKED( IDC_CHECKBOX_ENABLED, &CiTunesDiscordRPC2Dlg::OnBnClickedCheckboxEnabled )
	ON_BN_CLICKED( IDC_CHECKBOX_TRAYENABLED, &CiTunesDiscordRPC2Dlg::OnBnClickedCheckboxTrayenabled )
	ON_BN_CLICKED( IDC_BUTTON_CLOSE, &CiTunesDiscordRPC2Dlg::OnBnClickedButtonClose )
END_MESSAGE_MAP()


// CiTunesDiscordRPC2Dlg message handlers

BOOL CiTunesDiscordRPC2Dlg::OnInitDialog()
{
	BOOL	bIsEnabled;
	BOOL	bMinimizeToTray;
	BOOL	bStartupApp;
	LRESULT lStatus;

	CDialogEx::OnInitDialog();

	/* Initialize events for threads */
	g_hITunesClosingEvent	= CreateEvent( NULL, TRUE, FALSE, NULL );
	g_hAppDisabledEvent		= CreateEvent( NULL, TRUE, FALSE, NULL );

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	/* Align the controls */
	AlignDlgControls( this );

	/* Get the previous checkbox values from registry  */
	lStatus = GetApplicationRegValue( APP_REG_VALUE_ENABLED, &bIsEnabled );
	if (SUCCEEDED( lStatus )) {
		lStatus = GetApplicationRegValue( APP_REG_VALUE_MINIMIZETRAY, &bMinimizeToTray );
	}
	if (SUCCEEDED( lStatus )) {
		lStatus = GetApplicationRegValue( APP_REG_VALUE_STARTUPAPP, &bStartupApp );
	}
	
	if (FAILED( lStatus ))
	{
		TerminateApplication( this, L"An unknown error occurred", lStatus );
	}

	/* Set the checkbox values */
	((CButton *)GetDlgItem( IDC_CHECKBOX_ENABLED ))->SetCheck( bIsEnabled );
	((CButton *)GetDlgItem( IDC_CHECKBOX_TRAYENABLED ))->SetCheck( bMinimizeToTray );
	((CButton *)GetDlgItem( IDC_CHECKBOX_STARTUPAPP ))->SetCheck( bStartupApp );

	/* Check if disabled */
	if (bIsEnabled == 0)
	{
		SetEvent( g_hAppDisabledEvent );
	}

	/* Start threads */
	g_hITunesThread = CreateThread( NULL, 0, iTunesHandlerThread, this, 0, NULL );
	g_hTrayThread	= CreateThread( NULL, 0, ApplicationTrayHandler, this, 0, NULL );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CiTunesDiscordRPC2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CiTunesDiscordRPC2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CiTunesDiscordRPC2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Set the background color
HBRUSH CiTunesDiscordRPC2Dlg::OnCtlColor( CDC *pDC, CWnd *pWnd, UINT uCtlColor )
{
	return (HBRUSH)GetStockObject( WHITE_BRUSH );
}


void CiTunesDiscordRPC2Dlg::OnBnClickedCheckboxEnabled()
{
	CButton *pButton = (CButton *)GetDlgItem( IDC_CHECKBOX_ENABLED );
	SetApplicationRegValue( APP_REG_VALUE_ENABLED, pButton->GetCheck() );

	if (pButton->GetCheck())
	{
		ResetEvent( g_hAppDisabledEvent );
		
	}
	else
	{
		SetEvent( g_hAppDisabledEvent );
	}

	Sleep( 10 );

	/* Send event to iTunes to trigger status display */
	LONG cVolume;
	if (g_ItunesConnection && SUCCEEDED(CoInitializeEx( NULL, COINITBASE_MULTITHREADED )))
	{
		g_ItunesConnection->get_SoundVolume( &cVolume );
		g_ItunesConnection->put_SoundVolume( ++cVolume );
		g_ItunesConnection->put_SoundVolume( --cVolume );
		CoUninitialize();
	}
}

void CiTunesDiscordRPC2Dlg::OnBnClickedCheckboxStartupapp()
{
	CButton *pButton = (CButton *)GetDlgItem( IDC_CHECKBOX_STARTUPAPP );

	SetApplicationRegValue( APP_REG_VALUE_STARTUPAPP, pButton->GetCheck() );
	SetApplicationStartupProgram( pButton->GetCheck() );
}

void CiTunesDiscordRPC2Dlg::OnBnClickedCheckboxTrayenabled()
{
	CButton *MinimizeTrayBtn = (CButton *)GetDlgItem( IDC_CHECKBOX_TRAYENABLED );
	SetApplicationRegValue( APP_REG_VALUE_MINIMIZETRAY, MinimizeTrayBtn->GetCheck() );
	if (MinimizeTrayBtn->GetCheck() == 0)
	{
		((CButton *)GetDlgItem( IDC_CHECKBOX_ENABLED ))->EnableWindow( FALSE );
	}
}

void CiTunesDiscordRPC2Dlg::OnBnClickedButtonClose()
{
	CButton *MinimizeTrayBtn = (CButton *)GetDlgItem( IDC_CHECKBOX_TRAYENABLED );
	if (MinimizeTrayBtn->GetCheck() == 0)
	{
		/* Minimize to tray is disabled -- exit the app */
		UpdateDiscordStatus( NULL ); // Clear Discord status

		/* Delete tray icon */
		Shell_NotifyIcon( NIM_DELETE, &g_IconData );
		::DestroyWindow( g_hTrayWnd );
		UnregisterClassW( TRAY_WNDCLASS_NAME, GetModuleHandle( NULL ) );

		/* Cleanup iTunes connection */
		SafeRelease( &g_ItunesConnection );
		WaitForSingleObject( g_hITunesThread, 1000 );

		ExitProcess( 0 );
	}

	AfxGetMainWnd()->ShowWindow( SW_HIDE );
}
