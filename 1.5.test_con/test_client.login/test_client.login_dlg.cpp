#include "pch.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

bool		test_client_dlg::m_enable_connection_test = true;
bool		test_client_dlg::m_enable_enter_channel_test = true;
bool		test_client_dlg::m_enable_message_send_test = true;

bool 		test_client_dlg::m_setting_connection_make_new_member_data = false;
uint32_t	test_client_dlg::m_setting_connection_aid_min = 1;
uint32_t	test_client_dlg::m_setting_connection_aid_max = 1000;
int			test_client_dlg::m_setting_connection_interval = 1000;
int			test_client_dlg::m_setting_connection_min = 10000;
int			test_client_dlg::m_setting_connection_max = 20000;
int			test_client_dlg::m_setting_connect_per_sec = 2000;
int			test_client_dlg::m_setting_disconnect_per_sec = 2000;

bool		test_client_dlg::m_setting_enable_disconnecting = true;

bool		test_client_dlg::m_setting_enable_channel_random = true;
int			test_client_dlg::m_setting_enable_enter_channel_selected = 1;
int			test_client_dlg::m_setting_enter_channel_min = 2;
int			test_client_dlg::m_setting_enter_channel_max = 1024;
int			test_client_dlg::m_setting_message_length = 60;
bool		test_client_dlg::m_setting_enable_disconnect_after_send_count = true;
int			test_client_dlg::m_setting_message_count_before_disconnect = 20;
chrono::tick::duration	test_client_dlg::m_setting_message_interval = 1s;

#pragma warning(disable:4407)

own_ptr<net::connector<socket_account_server>> test_client_dlg::m_pconnector_account;
own_ptr<net::connector<socket_stage_instance_server>> test_client_dlg::m_pconnector_stage_instance;


std::atomic<int> g_count_request_enter_user;

test_client_dlg::test_client_dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TEST_CLIENT_DIALOG, pParent)
{
	this->m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

test_client_dlg::~test_client_dlg() noexcept
{
}

void test_client_dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL test_client_dlg::OnInitDialog()
{
	//-----------------------------------------------------------------
	// 1. MFC Default
	//-----------------------------------------------------------------
	this->CDialogEx::OnInitDialog();

	SetIcon(this->m_hIcon, TRUE);
	SetIcon(this->m_hIcon, FALSE);

	// set locale
	setlocale(LC_ALL, "korean");
	

	//-----------------------------------------------------------------
	// 2. Log 설정
	//-----------------------------------------------------------------
	// 1) create logger view
	m_child_logger_view.CreateChild(nullptr, this);

	// 2) dialog의 크기를 얻는다.
	RECT rect_dlg;
	GetClientRect(&rect_dlg);
	RECT rect_dlg_logger;
	this->m_child_logger_view.GetClientRect(&rect_dlg_logger);

	// 3) set logger view
	this->m_child_logger_view.Resize(rect_dlg.right - rect_dlg_logger.right - 24, -120);
	this->m_child_logger_view.Move(4, -10, ORIGIN_LEFT | ORIGIN_UPON, GetDlgItem(IDC_STATIC_FOOTER));
	this->m_child_logger_view.Initialize();

	// 4) mfc log control을 연결한다.
	set_default_logger(&this->m_child_logger_view);


	//-----------------------------------------------------------------
	// 3. Connector
	//-----------------------------------------------------------------
	// 1) Connector [Login]를 생성한다.
	this->m_pconnector_account = make_own<net::connector<socket_account_server>>();
	this->m_pconnector_account->start();

	this->m_pconnector_stage_instance = make_own<net::connector<socket_stage_instance_server>>();
	this->m_pconnector_stage_instance->start();


	//-----------------------------------------------------------------
	// 4. Control 설정.
	//-----------------------------------------------------------------
	// 1) Tooltip설정
	this->m_tooltip.Create(this);
	this->m_tooltip.SetMaxTipWidth(300);
	this->m_tooltip.SetDelayTime(TTDT_AUTOPOP, 20000);

	// 2) Connective - channel manager server
	this->m_child_connective_account_server.CreateChild(&m_pconnector_account->statistics_get_connection_info(), this);
	this->m_child_connective_account_server.MoveToCenter(GetDlgItem(IDC_STATIC_FRAME_CONNECTION_INFO), 0, 35);
	this->m_child_connective_account_server.ShowWindow(SW_SHOW);
	this->m_child_connective_account_server.SetUpdateInterval(1);

	// 2) Connective - channel server
	this->m_child_connective_stage_instance_server.CreateChild(&m_pconnector_stage_instance->statistics_get_connection_info(), this);
	this->m_child_connective_stage_instance_server.MoveToCenter(GetDlgItem(IDC_STATIC_FRAME_CHANNEL_SERVER), 0, 15);
	this->m_child_connective_stage_instance_server.ShowWindow(SW_SHOW);
	this->m_child_connective_stage_instance_server.SetUpdateInterval(1);
	
	// 3) Traffice info
	this->m_child_traffic_info.CreateChild(&net::io::statistics::Nsocket::statistics_get_traffic_info_total(), this);
	this->m_child_traffic_info.MoveToCenter(GetDlgItem(IDC_STATIC_FRAME_TRAFFIC));
	this->m_child_traffic_info.ShowWindow(SW_SHOW);
	this->m_child_traffic_info.SetUpdateInterval(2);

	// 4) Pool/Thread status dialog
	this->m_dlg_factory_status.CreateModaless(factory::manager::instance(), this);
	this->m_dlg_thread_status.CreateModaless(api::manager_thread::instance(), executor::manager::instance(), system_executor::instance(), this);

	// 5) Tooltips
	this->m_tooltip.AddTool(GetDlgItem(IDC_CHECK_CONNECT_TEST_STARTSTOP), TEXT("테스트를 시도/종료한다."));

	// 6) Destination IP Address
	this->SetDlgItemText(IDC_EDIT_ADDRESS, TEXT("127.0.0.1"));
	//SetDlgItemText(IDC_EDIT_ADDRESS, TEXT("192.168.0.55"));
	this->m_tooltip.AddTool(GetDlgItem(IDC_EDIT_ADDRESS), TEXT("접속을 할 채팅 서버 주소."));

	
	this->SetDlgItemText(IDC_EDIT_ACCOUNT_NAME, L"test_name");
	this->SetDlgItemInt(IDC_EDIT_ACCOUNT_ID_MIN, m_setting_connection_aid_min);
	this->m_tooltip.AddTool(GetDlgItem(IDC_EDIT_ACCOUNT_ID_MIN), TEXT("랜덤하게 발급할 ACCOUNT_ID의 최소값"));
	this->SetDlgItemInt(IDC_EDIT_ACCOUNT_ID_MAX, m_setting_connection_aid_max);
	this->m_tooltip.AddTool(GetDlgItem(IDC_EDIT_ACCOUNT_ID_MAX), TEXT("랜덤하게 발급할 ACCOUNT_ID의 최대값"));
	((CButton*)this->GetDlgItem(IDC_CHECK_CONNECTION_TEST))->SetCheck(m_enable_connection_test ? BST_CHECKED : BST_UNCHECKED);
	this->m_tooltip.AddTool(GetDlgItem(IDC_CHECK_CONNECTION_TEST), TEXT("계속 접속/종료 테스트 켜기/끄기."));
	((CButton*)this->GetDlgItem(IDC_CHECK_MAKE_NEW_MEMBER_DATA))->SetCheck(m_setting_connection_make_new_member_data ? BST_CHECKED : BST_UNCHECKED);
	this->m_tooltip.AddTool(GetDlgItem(IDC_CHECK_MAKE_NEW_MEMBER_DATA), TEXT("신규 계정 생성."));

	this->EnableWindow_Connection(m_enable_connection_test);
	((CButton*)this->GetDlgItem(IDC_CHECK_ENTER_CHANNEL_TEST))->SetCheck(m_enable_enter_channel_test ? BST_CHECKED : BST_UNCHECKED);
	this->m_tooltip.AddTool(GetDlgItem(IDC_CHECK_ENTER_CHANNEL_TEST), TEXT("접속후 특정 채널에 입장하는 테스트 켜기/끄기."));
	this->EnableWindow_EnterChannel(m_enable_enter_channel_test);
	((CButton*)this->GetDlgItem(IDC_CHECK_MESSAGE_SEND_TEST))->SetCheck(m_enable_message_send_test ? BST_CHECKED : BST_UNCHECKED);
	this->m_tooltip.AddTool(GetDlgItem(IDC_CHECK_MESSAGE_SEND_TEST), TEXT("특정 채널 입장 후 채팅 메시지를 전송하는 테스트 켜기/끄기."));
	this->EnableWindow_SendMessage(m_enable_message_send_test);

	this->SetDlgItemInt(IDC_EDIT_CONNECTION_INTERVAL, m_setting_connection_interval);
	this->m_tooltip.AddTool(GetDlgItem(IDC_EDIT_CONNECTION_INTERVAL), TEXT("접속 및 접속 종료를 시도하는 간격을 의미한다."));
	this->SetDlgItemInt(IDC_EDIT_CONNECTION_MAX, m_setting_connection_max);
	this->m_tooltip.AddTool(GetDlgItem(IDC_EDIT_CONNECTION_MAX), TEXT("최대 접속 수를 의미한다. 이 숫자 이상의 접속수 일 때 접속을 시도하지 않는다."));
	((CButton*)this->GetDlgItem(IDC_CHECK_CONNECTION_DISCONNECT))->SetCheck(m_setting_enable_disconnecting ? BST_CHECKED : BST_UNCHECKED);
	this->m_tooltip.AddTool(GetDlgItem(IDC_CHECK_CONNECTION_DISCONNECT), TEXT("접속 종료 테스트를 할 것인가 여부."));
	this->SetDlgItemInt(IDC_EDIT_CONNECTION_MIN, m_setting_connection_min);
	this->m_tooltip.AddTool(GetDlgItem(IDC_EDIT_CONNECTION_MIN), TEXT("최소 접속 수를 의미한다. 이 숫자 이하의 접속수일 때 접속을 해제하지 않는다."));
	this->SetDlgItemInt(IDC_EDIT_CONNECT_PER_SEC, m_setting_connect_per_sec);
	this->m_tooltip.AddTool(GetDlgItem(IDC_EDIT_CONNECT_PER_SEC), TEXT("1회 평균 접속 시도수"));
	this->SetDlgItemInt(IDC_EDIT_DISCONNECT_PER_SEC, m_setting_disconnect_per_sec);
	this->m_tooltip.AddTool(GetDlgItem(IDC_EDIT_DISCONNECT_PER_SEC), TEXT("1회 평균 접속종료 시도수"));

	((CButton*)this->GetDlgItem(IDC_RADIO_CHANNEL_ID_RANDOM))->SetCheck(m_setting_enable_channel_random ? BST_CHECKED : BST_UNCHECKED);
	this->m_tooltip.AddTool(GetDlgItem(IDC_RADIO_CHANNEL_ID_RANDOM), TEXT("랜덤한 채널로 입장하는 테스트를 한다."));
	this->SetDlgItemInt(IDC_EDIT_CHANNEL_ID_MIN, m_setting_enter_channel_min);
	this->m_tooltip.AddTool(GetDlgItem(IDC_EDIT_CHANNEL_ID_MIN), TEXT("랜덤 입장 테스트 시 최저 채널값"));
	this->SetDlgItemInt(IDC_EDIT_CHANNEL_ID_MAX, m_setting_enter_channel_max);
	this->m_tooltip.AddTool(GetDlgItem(IDC_EDIT_CHANNEL_ID_MAX), TEXT("랜던 입장 테스트 시 최대 채널값"));

	((CButton*)this->GetDlgItem(IDC_RADIO_CHANNEL_ID_SELECTED))->SetCheck(m_setting_enable_channel_random ? BST_UNCHECKED : BST_CHECKED);
	this->m_tooltip.AddTool(GetDlgItem(IDC_RADIO_CHANNEL_ID_SELECTED), TEXT("정해진 채널로 입장하는 테스트를 한다."));
	this->SetDlgItemInt(IDC_EDIT_CHANNEL_ID_SELECTED, m_setting_enable_enter_channel_selected);
	this->m_tooltip.AddTool(GetDlgItem(IDC_EDIT_CHANNEL_ID_SELECTED), TEXT("정해진 입장 채널"));

	this->SetDlgItemInt(IDC_EDIT_MESSAGE_LENGTH, m_setting_message_length);
	this->m_tooltip.AddTool(GetDlgItem(IDC_EDIT_MESSAGE_LENGTH), TEXT("채팅 메시지 길이"));
	this->SetDlgItemInt(IDC_EDIT_MESSAGE_INVERVAL, static_cast<UINT>(duration_cast<chrono::tick::seconds>(m_setting_message_interval).count()));
	this->m_tooltip.AddTool(GetDlgItem(IDC_EDIT_MESSAGE_INVERVAL), TEXT("채팅 메시지를 전송하는 간격(초)"));
	((CButton*)this->GetDlgItem(IDC_CHECK_MESSAGE_SEND_COUNT_BEFORE_DISCONNECT))->SetCheck(m_setting_enable_disconnect_after_send_count ? BST_CHECKED : BST_UNCHECKED);
	this->m_tooltip.AddTool(GetDlgItem(IDC_CHECK_MESSAGE_SEND_COUNT_BEFORE_DISCONNECT), TEXT("정해진 메시지를 전송한 후 자동으로 접속 종료를 한다."));
	this->SetDlgItemInt(IDC_EDIT_MESSAGE_SEND_COUNT_BEFORE_DISCONNECT, m_setting_message_count_before_disconnect);
	this->m_tooltip.AddTool(GetDlgItem(IDC_EDIT_MESSAGE_SEND_COUNT_BEFORE_DISCONNECT), TEXT("정송할 메시지 수"));

	// 7) Destination Port
	this->SetDlgItemInt(IDC_EDIT_PORT, 31300);
	this->m_tooltip.AddTool(GetDlgItem(IDC_EDIT_ADDRESS), TEXT("접속을 할 채팅 서버 포트."));
	this->SetDlgItemInt(IDC_EDIT_CONNECT_TIMES, 2000);


	//-----------------------------------------------------------------
	// 4. Timer설정.
	//-----------------------------------------------------------------
	// 1) Random seed값을 설정한다.
	::srand(static_cast<uint32_t>(chrono::tick::now().time_since_epoch().count()));

	// 2) ...
	generate_messages();


	// return)
	return TRUE;
}

BOOL test_client_dlg::PreTranslateMessage(MSG* pMsg)
{
	this->m_tooltip.RelayEvent(pMsg);

	return __super::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(test_client_dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDOK, &test_client_dlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &test_client_dlg::OnBnClickedCancel)

	ON_BN_CLICKED(IDC_BUTTON_POOL, &test_client_dlg::OnBnClickedButtonPool)
	ON_BN_CLICKED(IDC_BUTTON_THREAD, &test_client_dlg::OnBnClickedButtonThread)

	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &test_client_dlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT1, &test_client_dlg::OnBnClickedButtonConnect1)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT10, &test_client_dlg::OnBnClickedButtonConnect10)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT100, &test_client_dlg::OnBnClickedButtonConnect100)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &test_client_dlg::OnBnClickedButtonDisconnect)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT_ALL, &test_client_dlg::OnBnClickedButtonDisconnectAll)
	ON_BN_CLICKED(IDC_CHECK_CONNECT_TEST_STARTSTOP, &test_client_dlg::OnBnClickedCheckConnectTestStartstop)
	ON_BN_CLICKED(IDC_CHECK_MAKE_NEW_MEMBER_DATA, &test_client_dlg::OnBnClickedCheckMakeNewMemberData)
	ON_EN_CHANGE(IDC_EDIT_CHANNEL_ID_MIN, &test_client_dlg::OnEnChangeEditEnterChannelIDMin)
	ON_EN_CHANGE(IDC_EDIT_CHANNEL_ID_MAX, &test_client_dlg::OnEnChangeEditEnterChannelIDMax)

	ON_EN_CHANGE(IDC_EDIT_ACCOUNT_ID_MIN, &test_client_dlg::OnEnChangeEditAccountIdMin)
	ON_EN_CHANGE(IDC_EDIT_ACCOUNT_ID_MAX, &test_client_dlg::OnEnChangeEditAccountIdMax)

	ON_BN_CLICKED(IDC_RADIO_CHANNEL_ID_RANDOM, &test_client_dlg::OnBnClickedRedioChannelIDRandom)
	ON_BN_CLICKED(IDC_RADIO_CHANNEL_ID_SELECTED, &test_client_dlg::OnBnClickedRedioChannelIDSelected)
	ON_BN_CLICKED(IDC_CHECK_CONNECTION_TEST, &test_client_dlg::OnBnClickedCheckConnectionTest)
	ON_EN_CHANGE(IDC_EDIT_CONNECTION_INTERVAL, &test_client_dlg::OnEnChangeEditConnectionInterval)
	ON_BN_CLICKED(IDC_CHECK_CONNECTION_DISCONNECT, &test_client_dlg::OnBnClickedCheckConnectionDisconnt)
	ON_EN_CHANGE(IDC_EDIT_CONNECTION_MAX, &test_client_dlg::OnEnChangeEditConnectionMax)

	ON_BN_CLICKED(IDC_CHECK_ENTER_CHANNEL_TEST, &test_client_dlg::OnBnClickedCheckEnterChannelTest)
	ON_BN_CLICKED(IDC_CHECK_CONNECTION_DISCONNECT, &test_client_dlg::OnBnClickedCheckConnectionDisconnect)
	ON_EN_CHANGE(IDC_EDIT_CONNECTION_MIN, &test_client_dlg::OnEnChangeEditConnectionMin)

	ON_BN_CLICKED(IDC_CHECK_MESSAGE_SEND_TEST, &test_client_dlg::OnBnClickedCheckMessageSendTest)
	ON_EN_CHANGE(IDC_EDIT_MESSAGE_LENGTH, &test_client_dlg::OnEnChangeEditMessageLength)
	ON_EN_CHANGE(IDC_EDIT_MESSAGE_INVERVAL, &test_client_dlg::OnEnChangeEditMessageInverval)
	ON_BN_CLICKED(IDC_CHECK_MESSAGE_SEND_COUNT_BEFORE_DISCONNECT, &test_client_dlg::OnBnClickedCheckMessageSendCountBeforeDisconnect)
	ON_EN_CHANGE(IDC_EDIT_MESSAGE_SEND_COUNT_BEFORE_DISCONNECT, &test_client_dlg::OnEnChangeEditMessageSendCountBeforeDisconnect)

	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &test_client_dlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_ALL, &test_client_dlg::OnBnClickedButtonCloseAll)
	ON_EN_CHANGE(IDC_EDIT_CONNECT_PER_SEC, &test_client_dlg::OnEnChangeEditConnectPerSec)
	ON_EN_CHANGE(IDC_EDIT_DISCONNECT_PER_SEC, &test_client_dlg::OnEnChangeEditDisconnectPerSec)
END_MESSAGE_MAP()

void test_client_dlg::OnPaint()
{
	if (this->IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, this->m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR test_client_dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(this->m_hIcon);
}

void test_client_dlg::OnEnChangeEditEnterChannelIDMin()
{
	// 1) 값을 읽는다.
	auto value = this->GetDlgItemInt(IDC_EDIT_CHANNEL_ID_MIN);

	// Check)
	RETURN_IF(this->m_setting_enter_channel_min == static_cast<int>(value), );

	// 2) 값을 대입한다.
	this->m_setting_enter_channel_min = value;

	// 3) 만약에 Max값보다 크면 max값을 변경한다.
	if (static_cast<int>(value) > this->m_setting_enter_channel_max)
	{
		this->m_setting_enter_channel_max = value;
		this->SetDlgItemInt(IDC_EDIT_CHANNEL_ID_MAX, value);
	}
}

void test_client_dlg::OnEnChangeEditEnterChannelIDMax()
{
	// 1) 값을 읽는다.
	auto value = this->GetDlgItemInt(IDC_EDIT_CHANNEL_ID_MAX);

	// Check)
	RETURN_IF(this->m_setting_enter_channel_max == static_cast<int>(value), );

	// 2) 값을 대입한다.
	this->m_setting_enter_channel_max = value;

	// 3) 만약에 Min값보다 작으면 min값을 변경한다.
	if (static_cast<int>(value) < this->m_setting_enter_channel_min)
	{
		this->m_setting_enter_channel_min = value;
		this->SetDlgItemInt(IDC_EDIT_CHANNEL_ID_MIN, value);
	}
}

void test_client_dlg::OnBnClickedOk()
{
}

void test_client_dlg::OnBnClickedCancel()
{
	// 1) 모든 소켓의 접속을 끊고 처리를 종료한다.
	this->m_pconnector_account->destroy();
	this->m_pconnector_stage_instance->destroy();

	// 2) ...
	system_executor::unregister_schedulable(this);

	// 3) Default Trace를 Reset
	reset_default_logger();

	// 4) OnOK~
	this->OnOK();
}

void test_client_dlg::OnEnChangeEditAccountIdMin()
{
	// 1) get new value
	auto aid_min_new = GetDlgItemInt(IDC_EDIT_ACCOUNT_ID_MIN);

	// check) 
	RETURN_IF(m_setting_connection_aid_min == aid_min_new,);

	if (aid_min_new < 1)
		aid_min_new = 1;

	if (aid_min_new > m_setting_connection_aid_max)
	{
		m_setting_connection_aid_max = aid_min_new;
		this->SetDlgItemInt(IDC_EDIT_ACCOUNT_ID_MAX, m_setting_connection_aid_max);
	}

	m_setting_connection_aid_min = aid_min_new;
}

void test_client_dlg::OnEnChangeEditAccountIdMax()
{
	// 1) get new value
	auto aid_max_new = GetDlgItemInt(IDC_EDIT_ACCOUNT_ID_MAX);

	// check) 
	RETURN_IF(m_setting_connection_aid_max == aid_max_new, );

	if (aid_max_new < 1)
		aid_max_new = 1;

	if (aid_max_new < m_setting_connection_aid_min)
	{
		m_setting_connection_aid_min = aid_max_new;
		this->SetDlgItemInt(IDC_EDIT_ACCOUNT_ID_MIN, m_setting_connection_aid_min);
	}

	m_setting_connection_aid_max = aid_max_new;
}

void test_client_dlg::OnBnClickedCheckConnectTestStartstop()
{
	// 1) Button의 상태를 얻는다.
	CButton* pButton = ((CButton*)GetDlgItem(IDC_CHECK_CONNECT_TEST_STARTSTOP));

	// 2.A) Button 상태가 눌려진 상태일 경우 => 접속/해제 테스트를 시작한다.
	if (pButton->GetCheck() & BST_CHECKED)
	{
		// - 접속할 Address를 UI로 부터 읽어들인다.
		read_server_address();

		// - 메시지를 생성한다.(최초 딱 1회만 실행된다.)
		generate_messages();

		// - ...
		GetDlgItem(IDC_EDIT_MESSAGE_LENGTH)->EnableWindow(FALSE);

		// - ...
		execute_interval(m_setting_message_interval);
		system_executor::register_schedulable(this);
	}
	// 2.B) Button 상태가 눌려지지 않은 상태일 경우 => 접속/해제 테스트를 중단한다.
	else
	{
		// - Unregister
		system_executor::unregister_schedulable(this);

		// - ...
		GetDlgItem(IDC_EDIT_MESSAGE_LENGTH)->EnableWindow(TRUE);
	}
}

void test_client_dlg::OnBnClickedCheckMakeNewMemberData()
{
	// 1) Button의 상태를 얻는다.
	CButton* pButton = ((CButton*)GetDlgItem(IDC_CHECK_MAKE_NEW_MEMBER_DATA));

	// 2.A) Button 상태가 눌려진 상태일 경우 => 접속/해제 테스트를 시작한다.
	if (pButton->GetCheck() & BST_CHECKED)
	{
		m_setting_connection_make_new_member_data = true;
	}
	else
	{
		m_setting_connection_make_new_member_data = false;
	}
}


void test_client_dlg::OnBnClickedButtonConnect1()
{
	// 1) Server Address를 읽는다.
	read_server_address();

	// 2) connect
	this->connect_N(1);
}

void test_client_dlg::OnBnClickedButtonConnect10()
{
	// 1) Server Address를 읽는다.
	read_server_address();

	// 2) connect
	this->connect_N(10);
}

void test_client_dlg::OnBnClickedButtonConnect100()
{
	// 1) Server Address를 읽는다.
	this->read_server_address();

	// 2) connect
	this->connect_N(100);
}

void test_client_dlg::OnBnClickedButtonConnect()
{
	// 1) 몇개의 Connection을 할지 얻는다.
	int	nTimer = this->GetDlgItemInt(IDC_EDIT_CONNECT_TIMES);

	// 2) nTimer만큼 접속을 한다.
	this->connect_N(nTimer);
}

void test_client_dlg::OnBnClickedButtonDisconnect()
{
	scoped_lock(this->m_pconnector_account->get_lockable())
	{
		if (this->m_pconnector_account->count() != 0)
		{
			(*this->m_pconnector_account->begin())->disconnect();
		}
	}

	scoped_lock(this->m_pconnector_stage_instance->get_lockable())
	{
		if (this->m_pconnector_stage_instance->count() != 0)
		{
			(*this->m_pconnector_stage_instance->begin())->disconnect();
		}
	}
}

void test_client_dlg::OnBnClickedButtonDisconnectAll()
{
	this->m_pconnector_account->disconnect_connectable_all();
	this->m_pconnector_stage_instance->disconnect_connectable_all();
}

void test_client_dlg::OnBnClickedButtonClose()
{
	scoped_lock(this->m_pconnector_account->get_lockable())
	{
		if (this->m_pconnector_account->count() != 0)
		{
			(*this->m_pconnector_account->begin())->closesocket();
		}
	}

	scoped_lock(this->m_pconnector_stage_instance->get_lockable())
	{
		if (this->m_pconnector_stage_instance->count() != 0)
		{
			(*this->m_pconnector_stage_instance->begin())->closesocket();
		}
	}
}

void test_client_dlg::OnBnClickedButtonCloseAll()
{
	this->m_pconnector_account->close_connectable_all();
	this->m_pconnector_stage_instance->close_connectable_all();
}

void test_client_dlg::OnBnClickedButtonPool()
{
	this->m_dlg_factory_status.ShowWindow(SW_SHOW);
}

void test_client_dlg::OnBnClickedButtonThread()
{
	this->m_dlg_thread_status.ShowWindow(SW_SHOW);
}

void test_client_dlg::OnBnClickedRedioChannelIDRandom()
{
	this->m_setting_enable_channel_random = ((CButton*)GetDlgItem(IDC_RADIO_CHANNEL_ID_RANDOM))->GetCheck() == BST_CHECKED;

	this->GetDlgItem(IDC_STATIC_CHANNEL_ID_MIN)->EnableWindow(m_setting_enable_channel_random);
	this->GetDlgItem(IDC_EDIT_CHANNEL_ID_MIN)->EnableWindow(m_setting_enable_channel_random);
	this->GetDlgItem(IDC_STATIC_CHANNEL_ID_MAX)->EnableWindow(m_setting_enable_channel_random);
	this->GetDlgItem(IDC_EDIT_CHANNEL_ID_MAX)->EnableWindow(m_setting_enable_channel_random);
	this->GetDlgItem(IDC_EDIT_CHANNEL_ID_SELECTED)->EnableWindow(!m_setting_enable_channel_random);
}
void test_client_dlg::OnBnClickedRedioChannelIDSelected()
{
	this->m_setting_enable_channel_random = ((CButton*)GetDlgItem(IDC_RADIO_CHANNEL_ID_SELECTED))->GetCheck() != BST_CHECKED;

	this->GetDlgItem(IDC_STATIC_CHANNEL_ID_MIN)->EnableWindow(m_setting_enable_channel_random);
	this->GetDlgItem(IDC_EDIT_CHANNEL_ID_MIN)->EnableWindow(m_setting_enable_channel_random);
	this->GetDlgItem(IDC_STATIC_CHANNEL_ID_MAX)->EnableWindow(m_setting_enable_channel_random);
	this->GetDlgItem(IDC_EDIT_CHANNEL_ID_MAX)->EnableWindow(m_setting_enable_channel_random);
	this->GetDlgItem(IDC_EDIT_CHANNEL_ID_SELECTED)->EnableWindow(!m_setting_enable_channel_random);
}

void test_client_dlg::OnBnClickedCheckConnectionTest()
{
	this->m_enable_connection_test = ((CButton*)this->GetDlgItem(IDC_CHECK_CONNECTION_TEST))->GetCheck() == BST_CHECKED;

	this->EnableWindow_Connection(m_enable_connection_test);
}

void test_client_dlg::OnBnClickedCheckEnterChannelTest()
{
	this->m_enable_enter_channel_test = ((CButton*)this->GetDlgItem(IDC_CHECK_ENTER_CHANNEL_TEST))->GetCheck() == BST_CHECKED;

	this->EnableWindow_EnterChannel(this->m_enable_enter_channel_test);
}

void test_client_dlg::OnBnClickedCheckMessageSendTest()
{
	this->m_enable_message_send_test = ((CButton*)this->GetDlgItem(IDC_CHECK_MESSAGE_SEND_TEST))->GetCheck() == BST_CHECKED;

	this->EnableWindow_SendMessage(this->m_enable_message_send_test);
}

void test_client_dlg::OnEnChangeEditConnectionInterval()
{
	this->m_setting_connection_interval = this->GetDlgItemInt(IDC_EDIT_CONNECTION_INTERVAL);
}

void test_client_dlg::OnBnClickedCheckConnectionDisconnt()
{
	m_setting_enable_disconnecting = ((CButton*)this->GetDlgItem(IDC_CHECK_CONNECTION_DISCONNECT))->GetCheck() == BST_CHECKED;

	this->GetDlgItem(IDC_STATIC_CONNECTION_MIN)->EnableWindow(m_setting_enable_disconnecting);
	this->GetDlgItem(IDC_EDIT_CONNECTION_MIN)->EnableWindow(m_setting_enable_disconnecting);
	this->GetDlgItem(IDC_EDIT_DISCONNECT_PER_SEC)->EnableWindow(m_setting_enable_disconnecting);
	this->GetDlgItem(IDC_STATIC_CONNECTION_B)->EnableWindow(m_setting_enable_disconnecting);
}

void test_client_dlg::OnEnChangeEditConnectPerSec()
{
	this->m_setting_connect_per_sec = this->GetDlgItemInt(IDC_EDIT_CONNECT_PER_SEC);
}

void test_client_dlg::OnEnChangeEditDisconnectPerSec()
{
	this->m_setting_disconnect_per_sec = this->GetDlgItemInt(IDC_EDIT_DISCONNECT_PER_SEC);
}

void test_client_dlg::OnBnClickedCheckConnectionDisconnect()
{
	if (((CButton*)this->GetDlgItem(IDC_CHECK_CONNECTION_DISCONNECT))->GetCheck() == BST_CHECKED)
	{
		this->GetDlgItem(IDC_STATIC_CONNECTION_MIN)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_EDIT_CONNECTION_MIN)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_EDIT_DISCONNECT_PER_SEC)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_STATIC_CONNECTION_B)->EnableWindow(TRUE);
	}
	else
	{
		this->GetDlgItem(IDC_STATIC_CONNECTION_MIN)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_EDIT_CONNECTION_MIN)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_EDIT_DISCONNECT_PER_SEC)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_STATIC_CONNECTION_B)->EnableWindow(FALSE);
	}
}

void test_client_dlg::OnEnChangeEditConnectionMin()
{
	this->m_setting_connection_min = GetDlgItemInt(IDC_EDIT_CONNECTION_MIN);
}

void test_client_dlg::OnEnChangeEditConnectionMax()
{
	this->m_setting_connection_max = GetDlgItemInt(IDC_EDIT_CONNECTION_MAX);
}

void test_client_dlg::OnEnChangeEditMessageLength()
{
	this->m_setting_message_length = GetDlgItemInt(IDC_EDIT_MESSAGE_LENGTH);

	generate_messages();
}

void test_client_dlg::OnBnClickedCheckMessageSendCountBeforeDisconnect()
{
	this->m_setting_enable_disconnect_after_send_count = ((CButton*)GetDlgItem(IDC_CHECK_MESSAGE_SEND_COUNT_BEFORE_DISCONNECT))->GetCheck() == BST_CHECKED;

	this->GetDlgItem(IDC_EDIT_MESSAGE_SEND_COUNT_BEFORE_DISCONNECT)->EnableWindow(m_setting_enable_disconnect_after_send_count);
}

void test_client_dlg::OnEnChangeEditMessageSendCountBeforeDisconnect()
{
	this->m_setting_message_count_before_disconnect = GetDlgItemInt(IDC_EDIT_MESSAGE_SEND_COUNT_BEFORE_DISCONNECT);
}

void test_client_dlg::OnEnChangeEditMessageInverval()
{
	// 1) 얻는다.
	auto setting_message_interval = GetDlgItemInt(IDC_EDIT_MESSAGE_INVERVAL);

	// Check) 
	RETURN_IF(chrono::tick::seconds(setting_message_interval) == m_setting_message_interval, );

	// 2) ...
	if (setting_message_interval < 0)
	{
		setting_message_interval = 1;
		this->SetDlgItemInt(IDC_EDIT_MESSAGE_INVERVAL, setting_message_interval);
	}

	// 3) ...
	this->m_setting_message_interval = chrono::tick::seconds(setting_message_interval);

	// 4) Interval을 신규로 설정.
	this->execute_interval(m_setting_message_interval);
}

void test_client_dlg::EnableWindow_Connection(BOOL _bEnable)
{
	this->GetDlgItem(IDC_EDIT_ACCOUNT_ID_MIN)->EnableWindow(_bEnable);
	this->GetDlgItem(IDC_EDIT_ACCOUNT_ID_MAX)->EnableWindow(_bEnable);

	this->GetDlgItem(IDC_STATIC_CONNECTION_INTERVAL)->EnableWindow(_bEnable);
	this->GetDlgItem(IDC_EDIT_CONNECTION_INTERVAL)->EnableWindow(_bEnable);
	this->GetDlgItem(IDC_STATIC_CONNECTION_INTERVAL_TAIL)->EnableWindow(_bEnable);

	this->GetDlgItem(IDC_STATIC_CONNECTION_MAX)->EnableWindow(_bEnable);
	this->GetDlgItem(IDC_EDIT_CONNECTION_MAX)->EnableWindow(_bEnable);

	this->GetDlgItem(IDC_CHECK_CONNECTION_DISCONNECT)->EnableWindow(_bEnable);
	this->GetDlgItem(IDC_STATIC_CONNECTION_MIN)->EnableWindow(_bEnable && m_setting_enable_disconnecting);
	this->GetDlgItem(IDC_EDIT_CONNECTION_MIN)->EnableWindow(_bEnable && m_setting_enable_disconnecting);

	this->GetDlgItem(IDC_STATIC_CONNECTION_A)->EnableWindow(_bEnable);
	this->GetDlgItem(IDC_STATIC_CONNECTION_C)->EnableWindow(_bEnable);

	this->GetDlgItem(IDC_EDIT_CONNECT_PER_SEC)->EnableWindow(_bEnable);
	this->GetDlgItem(IDC_EDIT_DISCONNECT_PER_SEC)->EnableWindow(_bEnable && m_setting_enable_disconnecting);
	this->GetDlgItem(IDC_STATIC_CONNECTION_B)->EnableWindow(_bEnable && m_setting_enable_disconnecting);
}

void test_client_dlg::EnableWindow_EnterChannel(BOOL _bEnable)
{
	this->GetDlgItem(IDC_RADIO_CHANNEL_ID_RANDOM)->EnableWindow(_bEnable);
	this->GetDlgItem(IDC_STATIC_CHANNEL_ID_MIN)->EnableWindow(_bEnable && m_setting_enable_channel_random);
	this->GetDlgItem(IDC_EDIT_CHANNEL_ID_MIN)->EnableWindow(_bEnable && m_setting_enable_channel_random);
	this->GetDlgItem(IDC_STATIC_CHANNEL_ID_MAX)->EnableWindow(_bEnable && m_setting_enable_channel_random);
	this->GetDlgItem(IDC_EDIT_CHANNEL_ID_MAX)->EnableWindow(_bEnable && m_setting_enable_channel_random);

	this->GetDlgItem(IDC_RADIO_CHANNEL_ID_SELECTED)->EnableWindow(_bEnable);
	this->GetDlgItem(IDC_EDIT_CHANNEL_ID_SELECTED)->EnableWindow(_bEnable && !m_setting_enable_channel_random);
}

void test_client_dlg::EnableWindow_SendMessage(BOOL _bEnable)
{
	this->GetDlgItem(IDC_STATIC_MESSAGE_LENGTH)->EnableWindow(_bEnable);
	this->GetDlgItem(IDC_EDIT_MESSAGE_LENGTH)->EnableWindow(_bEnable);
	this->GetDlgItem(IDC_STATIC_MESSAGE_INVERVAL)->EnableWindow(_bEnable);
	this->GetDlgItem(IDC_EDIT_MESSAGE_INVERVAL)->EnableWindow(_bEnable);
	this->GetDlgItem(IDC_STATIC_MESSAGE_INVERVAL_TAIL)->EnableWindow(_bEnable);
	this->GetDlgItem(IDC_CHECK_MESSAGE_SEND_COUNT_BEFORE_DISCONNECT)->EnableWindow(_bEnable);
	this->GetDlgItem(IDC_EDIT_MESSAGE_SEND_COUNT_BEFORE_DISCONNECT)->EnableWindow(_bEnable && m_setting_enable_disconnect_after_send_count);
	this->GetDlgItem(IDC_CHECK_MESSAGE_SEND_COUNT_BEFORE_DISCONNECT_TAIL)->EnableWindow(_bEnable);
}

void test_client_dlg::read_server_address()
{
	// declare) 
	char str_address[256];
	char str_port[256];

	// 3) 접속할 서버의 주소(DNS)와 Port를 얻는다.
	::GetDlgItemTextA(this->m_hWnd, IDC_EDIT_ADDRESS, str_address, 256);
	::GetDlgItemTextA(this->m_hWnd, IDC_EDIT_PORT, str_port, 256);

	// 4) 주소(DNS)에서 ip를 얻어낸다.
	auto resolve_result = net::ip::tcp::resolver().resolve(str_address, str_port);

	// check)
	RETURN_IF(resolve_result.empty());

	// - set session server address
	this->m_sockaddr_server = resolve_result.at(0).endpoint();
}

void test_client_dlg::connect_N(int _n)
{
	//autothis->GetDlgItemInt(IDC_EDIT_ACCOUNT_ID_MIN);
	//this->GetDlgItemInt(IDC_EDIT_ACCOUNT_ID_MAX);

	auto diff = this->m_setting_connection_aid_max - this->m_setting_connection_aid_min;

	for (int i = 0; i < _n; ++i)
	{
		sACCOUNT_CERTIFICATION temp_account_certification;
		temp_account_certification.certification_type = eCERTIFICATION_TYPE::NONE;
		temp_account_certification.aid.uid = this->m_setting_connection_aid_min + ((diff != 0) ? (rand() % diff) : 0);
		temp_account_certification.aid.serial = 1;
		temp_account_certification.time_certified = chrono::time::utc::now();
		temp_account_certification.time_expire = temp_account_certification.time_certified + 1h;

		// - checkin info를 작성한다
		auto buf_key_checkin = alloc_shared_buffer(2048);
		buf_key_checkin.append<sACCOUNT_CERTIFICATION>(temp_account_certification);

		// - Socket을 생성한다.
		auto psocket = make_object<socket_account_server>();

		// - extract checkin key
		psocket->m_key_checkin = std::move(buf_key_checkin);

		// - connect
		this->m_pconnector_account->request_connect(psocket, this->m_sockaddr_server);
	}
}

void test_client_dlg::on_execute()
{
	// 1) message test
	this->process_functions_test_functions();

	// 2) connection test
	this->process_functions_test_connect();

	// 3) match 
	this->process_functions_match();
}

void test_client_dlg::process_functions_test_connect()
{
	//-----------------------------------------------------------------
	// 1. Disconnect Test
	//-----------------------------------------------------------------
	long disconnected_a = 0;
	long disconnected_b = 0;

	// Check)
	RETURN_IF(this->m_enable_connection_test == false, );

	if (this->m_setting_enable_disconnecting)
	{
		scoped_lock(this->m_pconnector_account->get_lockable())
		{
			long count_socket = static_cast<long>(this->m_pconnector_account->count());

			if (count_socket > m_setting_connection_min)
			{
				int temp = this->m_setting_disconnect_per_sec + (rand() % ((count_socket / 10) + 1));
				int	disconnect = min(count_socket, this->m_setting_disconnect_per_sec);
				auto iter = this->m_pconnector_account->begin();
				auto iter_end = this->m_pconnector_account->end();

				for (; iter != iter_end; ++iter)
				{
					// - Disconnect한다.
					if ((*iter)->closesocket() == TRUE)
					{
						BREAK_IF((++disconnected_a) == disconnect);
					}
				}

				count_socket -= disconnected_a;
			}
		}

		scoped_lock(this->m_pconnector_stage_instance->get_lockable())
		{
			long count_socket = static_cast<long>(this->m_pconnector_stage_instance->count());

			if (count_socket > m_setting_connection_min)
			{
				int temp = this->m_setting_disconnect_per_sec + (rand() % ((count_socket / 10) + 1));
				int	disconnect = min(count_socket, this->m_setting_disconnect_per_sec);
				auto iter = this->m_pconnector_stage_instance->begin();
				auto iter_end = this->m_pconnector_stage_instance->end();

				for (; iter != iter_end; ++iter)
				{
					// - Disconnect한다.
					if ((*iter)->closesocket() == TRUE)
					{
						BREAK_IF((++disconnected_b) == disconnect);
					}
				}

				count_socket -= disconnected_b;
			}
		}
	}


	//-----------------------------------------------------------------
	// 2. Connect Test
	//-----------------------------------------------------------------
	{
		// Declare) 
		long nConnect;

		// 1) 현재 Socket 수를 계산한다.
		long count_socket = static_cast<long>(this->m_pconnector_account->allocated_socket_count() + this->m_pconnector_stage_instance->allocated_socket_count()) - disconnected_a;

		// 2) 최대접속수와 현재접속수의 차를 구한다
		nConnect = (count_socket < this->m_setting_connection_max) ? (this->m_setting_connection_max - count_socket) : 0;
		long temp_max = this->m_setting_connect_per_sec + (rand() % ((nConnect / 10) + 1));

		// 3) 최대접속수와 현재접속수의 차와 회당최대접속수 중 작은 값을 선택한다.
		nConnect = min(nConnect, temp_max);

		// 4) 접속한다.
		this->connect_N(nConnect);
	}
}

void test_client_dlg::process_functions_test_functions()
{
	// check)
	RETURN_IF(!this->m_enable_enter_channel_test && !this->m_enable_message_send_test);

	scoped_lock(this->m_pconnector_stage_instance->get_lockable())
	{
		for (auto& iter : *this->m_pconnector_stage_instance)
		{
			//iter->process_functions_test();
		}
	}
}

void test_client_dlg::process_functions_match()
{
	scoped_lock(this->m_pconnector_stage_instance->get_lockable())
	{
		for (auto& iter : *this->m_pconnector_stage_instance)
		{
			iter->process_functions_match();
		}
	}
}

void test_client_dlg::generate_messages()
{
	//{
	//	auto buf_temp = alloc_shared_buffer(1024);
	//	buf_temp.append<uint32_t>();
	//	buf_temp.append<uint32_t>(eMESSAGE::MATCH_MESSAGE);

	//	buf_temp.front<uint32_t>() = buf_temp.size<uint32_t>();

	//	socket_match_channel::m_buf_message_MATCH_MESSAGE = std::move(buf_temp);
	//}

	//{
	//	sEVENT temp_event;
	//	temp_event.type = 0;

	//	auto buf_temp = alloc_shared_buffer(1024);
	//	buf_temp.append<uint32_t>();
	//	buf_temp.append<uint32_t>(eMESSAGE::MATCH_EVENT);
	//	buf_temp.append<sEVENT>(temp_event);

	//	buf_temp.front<uint32_t>() = buf_temp.size<uint32_t>();

	//	socket_match_channel::m_buf_message_MATCH_EVENT_PLAY = std::move(buf_temp);
	//}

	//{
	//	sEVENT temp_event;
	//	temp_event.type = 1;

	//	auto buf_temp = alloc_shared_buffer(1024);
	//	buf_temp.append<uint32_t>();
	//	buf_temp.append<uint32_t>(eMESSAGE::MATCH_EVENT);
	//	buf_temp.append<sEVENT>(temp_event);

	//	buf_temp.front<uint32_t>() = buf_temp.size<uint32_t>();

	//	socket_match_channel::m_buf_message_MATCH_EVENT_DEAD = std::move(buf_temp);
	//}
}


