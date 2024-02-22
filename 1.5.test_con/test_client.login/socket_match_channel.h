//*****************************************************************************
//*                                                                           *
//*                         CGCII match making system                         *
//*                                                                           *
//*                                test client                                *
//*                                                                           *
//*  Programmed by Cho SangHyun(sangduck@cgcii.co.kr)                         *
//*                                                                           *
//*  Updated : 2021/07/10                                                     *
//*                                                                           *
//*****************************************************************************

#pragma once
// ----------------------------------------------------------------------------
//
// socket_match_channel
//
// ----------------------------------------------------------------------------
class socket_match_channel :
	virtual public				net::io::connect_requestable::Ntcp_reconnect,
	virtual public				Isocket,
	public						net::socket::tcp<>
{
public:
			socket_match_channel();
	virtual ~socket_match_channel();

public:
			void				process_functions_test();
			void				process_functions_match();

protected:
	virtual void				on_request_connect(const net::sockaddr& _address) override;
	virtual void				on_connect() override;
	virtual void				on_fail_connect(uint64_t _disconnect_reason) override;
	virtual	void				on_disconnect(uint64_t _reason) override;

protected:
	// declare message map) 
	DECLARE_CGMESSAGE_MAP

	// - messagee function
	virtual result_code			on_message(sMESSAGE_NETWORK& _msg) override;
			result_code			on_message_MATCH_PREPARE(sMESSAGE_NETWORK& _msg);
			result_code			on_message_MATCH_START(sMESSAGE_NETWORK& _msg);
			result_code			on_message_MATCH_RESULT(sMESSAGE_NETWORK& _msg);
			result_code			on_message_MATCH_MEMBER_ENTER(sMESSAGE_NETWORK& _msg);
			result_code			on_message_MATCH_MEMBER_LEAVE(sMESSAGE_NETWORK& _msg);
			result_code			on_message_MATCH_MESSAGE(sMESSAGE_NETWORK& _msg);
			result_code			on_message_MATCH_EVENT(sMESSAGE_NETWORK& _msg);
			result_code			on_message_SEND_CHATTING_MESSAGE(sMESSAGE_NETWORK& _msg);

public:
	// - AID & Checkin Message
			shared_buffer		m_checkin_key;
			bool				m_match_started = false;
			int					m_count_match_process = 0;
	static	shared_buffer		m_buf_message_MATCH_MESSAGE;
	static	shared_buffer		m_buf_message_MATCH_EVENT_PLAY;
	static	shared_buffer		m_buf_message_MATCH_EVENT_DEAD;

			int					m_count_send = 0;

	// - Performance
			query_performance	m_perforance_connect;
};
