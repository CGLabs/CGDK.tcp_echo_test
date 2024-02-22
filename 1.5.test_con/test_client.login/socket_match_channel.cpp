#include "pch.h"


socket_match_channel::socket_match_channel()
{
}

socket_match_channel::~socket_match_channel()
{
}

void socket_match_channel::process_functions_test()
{
	// Check) 
	RETURN_IF(test_client_dlg::m_enable_message_send_test == false);
}

void socket_match_channel::process_functions_match()
{
	// match가 시작되었으면 MATCH MESSAGE를 쏜다.
	if (m_match_started)
	{
		// check) 
		RETURN_IF(m_count_match_process == 0);

		// - send MATCH_MESSAGE
		send(m_buf_message_MATCH_MESSAGE);

		// - send RECORD (5% ratio)
		if ((rand() % 20) == 0)
		{
			send(m_buf_message_MATCH_EVENT_PLAY);
		}

		auto count_match_process = --m_count_match_process;
		// - 
		if (count_match_process == 0)
		{
			send(m_buf_message_MATCH_EVENT_DEAD);
		}
	}
}

void socket_match_channel::on_request_connect(const net::sockaddr& /*_address*/)
{
	// 1) 접속 시도 시간을 얻는다.
	m_perforance_connect.check_time_start();
}

void socket_match_channel::on_connect(net::io::Iconnective* /*_pconnective*/)
{
	// 1) 접속 완료 시간을 얻는다.
	m_perforance_connect.check_time_end();

	// 2) 초기화한다.
	m_count_send = -1;
	m_match_started = false;

	// 3) send Request Checkin
	send(message_REQUEST_ACCOUNT_CHECKIN(0, m_checkin_key));
}

void socket_match_channel::on_fail_connect(net::io::Iconnective* /*_pconnective*/, uint64_t /*_disconnect_reason*/)
{
	// 1) 접속 완료 시간을 얻는다.
	m_perforance_connect.check_time_end();

	// 2) reset
	m_checkin_key.clear();
}

void socket_match_channel::on_disconnect(uint64_t /*_reason*/)
{
	// 1) reset
	m_checkin_key.clear();
}

BEGIN_CGMESSAGE_MAP(socket_match_channel)
	CGMESSAGE_TYPE_SUB(eMESSAGE::SYSTEM::NETWORK, ((sMESSAGE_BUFFER&)_msg).buf_message.front<uint32_t>(4))
		ON_CGMESSAGE(eMESSAGE::MATCH_PREPARE,			on_message_MATCH_PREPARE,			sMESSAGE_NETWORK)
		ON_CGMESSAGE(eMESSAGE::MATCH_START,				on_message_MATCH_START,				sMESSAGE_NETWORK)
		ON_CGMESSAGE(eMESSAGE::MATCH_RESULT,			on_message_MATCH_RESULT,			sMESSAGE_NETWORK)
		ON_CGMESSAGE(eMESSAGE::MATCH_MEMBER_ENTER,		on_message_MATCH_MEMBER_ENTER,		sMESSAGE_NETWORK)
		ON_CGMESSAGE(eMESSAGE::MATCH_MEMBER_LEAVE,		on_message_MATCH_MEMBER_LEAVE,		sMESSAGE_NETWORK)
		ON_CGMESSAGE(eMESSAGE::MATCH_MESSAGE,			on_message_MATCH_MESSAGE,			sMESSAGE_NETWORK);
		ON_CGMESSAGE(eMESSAGE::MATCH_EVENT,				on_message_MATCH_EVENT,				sMESSAGE_NETWORK);
		ON_CGMESSAGE(eMESSAGE::SEND_CHATTING_MESSAGE,	on_message_SEND_CHATTING_MESSAGE,	sMESSAGE_NETWORK)
	END_SUB_CGMESSAGE_MAP
END_CGMESSAGE_MAP

result_code socket_match_channel::on_message(sMESSAGE_NETWORK& _msg)
{
	// 1) Message를 처리한다.
	EXECUTE_CGMESSAGE_MAP(_msg);

	// return) 성공!!!
	return eRESULT::BYPASS;
}

result_code socket_match_channel::on_message_MATCH_PREPARE(sMESSAGE_NETWORK& /*_msg*/)
{
	// 1) readn
	send(message_MATCH_READY());

	// return) 
	return eRESULT::DONE;
}

result_code socket_match_channel::on_message_MATCH_START(sMESSAGE_NETWORK& /*_msg*/)
{
	m_match_started = true;
	m_count_match_process = 20;

	// return) 
	return eRESULT::DONE;
}

result_code socket_match_channel::on_message_MATCH_RESULT(sMESSAGE_NETWORK& /*_msg*/)
{
	m_match_started = false;

	// return) 
	return eRESULT::DONE;
}

result_code socket_match_channel::on_message_MATCH_MEMBER_ENTER(sMESSAGE_NETWORK& /*_msg*/)
{
	// return) 
	return eRESULT::DONE;
}

result_code socket_match_channel::on_message_MATCH_MEMBER_LEAVE(sMESSAGE_NETWORK& /*_msg*/)
{
	// return) 
	return eRESULT::DONE;
}

result_code socket_match_channel::on_message_MATCH_MESSAGE(sMESSAGE_NETWORK& /*_msg*/)
{
	// return) 
	return eRESULT::DONE;
}

result_code socket_match_channel::on_message_MATCH_EVENT(sMESSAGE_NETWORK& /*_msg*/)
{
	// return) 
	return eRESULT::DONE;
}

result_code socket_match_channel::on_message_SEND_CHATTING_MESSAGE(sMESSAGE_NETWORK& /*_msg*/)
{
	// return) 
	return eRESULT::DONE;
}
