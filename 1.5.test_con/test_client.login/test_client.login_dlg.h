#pragma once

// test_client_dlg dialogbox
class test_client_dlg : 
	public CDialogEx,
	public schedulable::Nexecutable,
	public Idisposable::Ndelete 
{
public:
	test_client_dlg(CWnd* pParent = nullptr);
	~test_client_dlg() noexcept;

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEST_CLIENT_DIALOG };
#endif

protected:
	virtual void				DoDataExchange(CDataExchange* pDX);	// DDX/DDV
	virtual BOOL				OnInitDialog();
	virtual BOOL				PreTranslateMessage(MSG* pMsg);

protected:
	// 4) MFC Message map
	DECLARE_MESSAGE_MAP()

	afx_msg HCURSOR				OnQueryDragIcon();
	afx_msg void				OnPaint();

	afx_msg void				OnBnClickedOk();
	afx_msg void				OnBnClickedCancel();
	afx_msg void				OnBnClickedButtonPool();
	afx_msg void				OnBnClickedButtonThread();
	afx_msg void				OnBnClickedButtonConnect1();
	afx_msg void				OnBnClickedButtonConnect10();
	afx_msg void				OnBnClickedButtonConnect100();
	afx_msg void				OnBnClickedButtonConnect();
	afx_msg void				OnBnClickedButtonDisconnectAll();
	afx_msg void				OnBnClickedButtonDisconnect();
	afx_msg void				OnBnClickedButtonClose();
	afx_msg void				OnBnClickedButtonCloseAll();
	afx_msg void				OnBnClickedCheckConnectTestStartstop();
	afx_msg void				OnBnClickedCheckMakeNewMemberData();
	afx_msg void				OnEnChangeEditAccountIdMin();
	afx_msg void				OnEnChangeEditAccountIdMax();

	afx_msg void				OnBnClickedRedioChannelIDRandom();
	afx_msg void				OnBnClickedRedioChannelIDSelected();
	afx_msg void				OnBnClickedCheckConnectionTest();
	afx_msg void				OnEnChangeEditConnectionInterval();
	afx_msg void				OnBnClickedCheckConnectionDisconnt();

	afx_msg void				OnBnClickedCheckEnterChannelTest();
	afx_msg void				OnBnClickedCheckConnectionDisconnect();
	afx_msg void				OnEnChangeEditConnectPerSec();
	afx_msg void				OnEnChangeEditDisconnectPerSec();
	afx_msg void				OnEnChangeEditConnectionMin();
	afx_msg void				OnEnChangeEditConnectionMax();

	afx_msg void				OnBnClickedCheckMessageSendTest();
	afx_msg void				OnEnChangeEditMessageLength();
	afx_msg void				OnBnClickedCheckMessageSendCountBeforeDisconnect();
	afx_msg void				OnEnChangeEditMessageSendCountBeforeDisconnect();
	afx_msg void				OnEnChangeEditMessageInverval();

	afx_msg void				OnEnChangeEditEnterChannelIDMin();
	afx_msg void				OnEnChangeEditEnterChannelIDMax();

			void				EnableWindow_Connection(BOOL _bEnable);
			void				EnableWindow_EnterChannel(BOOL _bEnable);
			void				EnableWindow_SendMessage(BOOL _bEnable);

protected:
			HICON				m_hIcon;

public:
	// - contorls
			CToolTipCtrl				m_tooltip;
			CChild_connective_info		m_child_connective_account_server;
			CChild_connective_info		m_child_connective_stage_instance_server;
			CChild_traffic_info			m_child_traffic_info;
			CDialog_object_factory_status m_dlg_factory_status;
			CDialog_thread_status		m_dlg_thread_status;
			CChild_logger_view			m_child_logger_view;

	// - settings
	static	bool				m_enable_connection_test;
	static	bool				m_enable_enter_channel_test;
	static	bool				m_enable_message_send_test;
	static	bool 				m_setting_connection_make_new_member_data;
	static	uint32_t 			m_setting_connection_aid_min;
	static	uint32_t 			m_setting_connection_aid_max;
	static	int					m_setting_connection_interval;
	static	int					m_setting_connection_min;
	static	int					m_setting_connection_max;
	static	int					m_setting_connect_per_sec;
	static	int					m_setting_disconnect_per_sec;
	static	bool				m_setting_enable_disconnecting;
	static	bool				m_setting_enable_channel_random;
	static	int					m_setting_enable_enter_channel_selected;
	static	int					m_setting_enter_channel_min;
	static	int					m_setting_enter_channel_max;
	static	int					m_setting_message_length;
	static	bool				m_setting_enable_disconnect_after_send_count;
	static	int					m_setting_message_count_before_disconnect;
	static	chrono::tick::duration	m_setting_message_interval;

	// - sockets.
			net::sockaddr		m_sockaddr_server;
	static	own_ptr<net::connector<socket_account_server>> m_pconnector_account;
	static	own_ptr<net::connector<socket_stage_instance_server>> m_pconnector_stage_instance;
			
public:
	// - 
			void				connect_N(int _n);
			void				read_server_address();
			void				process_functions_test_connect();
			void				process_functions_test_functions();
			void				process_functions_match();

			void				generate_messages();

protected:
	virtual	void				on_execute() override;
};
