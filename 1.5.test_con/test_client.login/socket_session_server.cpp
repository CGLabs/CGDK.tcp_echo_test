#include "pch.h"


socket_session_server::socket_session_server()
{
}

socket_session_server::~socket_session_server()
{
}

void socket_session_server::on_request_connect(const net::sockaddr& /*_address*/)
{

	// 3) 접속 시도 시간을 얻는다.
	m_perforance_connect.check_time_start();
}

void socket_session_server::on_connect(net::io::Iconnective* /*_pconnective*/)
{
	// static) 
	static uint64_t aid_uid = 0;

	// 1) ACCOUNT_ID를 얻는다.
	m_aid.uid = ++aid_uid;

	// 2) Buffer를 할당받는다.
	m_message_REQUEST_ACCOUNT_LOGIN = message_REQUEST_ACCOUNT_LOGIN(m_aid.uid, "test_id");

	// 1) 접속 완료 시간을 얻는다.
	m_perforance_connect.check_time_end();

	// 2) send Request Checkin
	send(m_message_REQUEST_ACCOUNT_LOGIN);
}

void socket_session_server::on_fail_connect(net::io::Iconnective* /*_pconnective*/, uint64_t /*_disconnect_reason*/)
{
	// 1) 접속 완료 시간을 얻는다.
	m_perforance_connect.check_time_end();
}

void socket_session_server::on_disconnect(uint64_t /*_reason*/)
{
}

BEGIN_CGMESSAGE_MAP(socket_session_server)
	CGMESSAGE_TYPE_SUB(eMESSAGE::SYSTEM::NETWORK, ((sMESSAGE_BUFFER&)_msg).buf_message.front<uint32_t>(4))
		ON_CGMESSAGE(eMESSAGE::RESPONSE_ACCOUNT_LOGIN,		on_message_RESPONSE_ACCOUNT_LOGIN,		sMESSAGE_NETWORK);
		ON_CGMESSAGE(eMESSAGE::RESPONSE_REGISTER_MATCHING,	on_message_RESPONSE_REGISTER_MATCHING,	sMESSAGE_NETWORK);
		ON_CGMESSAGE(eMESSAGE::MATCH_ESTABLISHED,			on_message_MATCH_ESTABLISHED,			sMESSAGE_NETWORK);
	END_SUB_CGMESSAGE_MAP
END_CGMESSAGE_MAP

result_code socket_session_server::on_message(sMESSAGE_NETWORK& _msg)
{
	// 1) Message를 처리한다.
	EXECUTE_CGMESSAGE_MAP(_msg);

	// return) 성공!!!
	return eRESULT::BYPASS;
}

result_code socket_session_server::on_message_RESPONSE_ACCOUNT_LOGIN(sMESSAGE_NETWORK& _msg)
{
	// 1) ...
	auto buf = _msg.buf_message + offset(8);

	// 2) get message
	auto result = buf.extract<eRESULT>();

	// 3A) 성공하면 REQUEST_ENTER 메시지를 전송한ㄷ다.
	if (result == eRESULT::SUCCESS)
	{
		// - matching을 요청하는 message를 전송한다.
		send(message_REQUEST_REGISTER_MATCHING(1));
	}
	// 3B) 실패하면 접속을 끊는다.
	else
	{
		// - 접속을 끊는다.
		closesocket();
	}

	// return) 
	return eRESULT::DONE;
}

result_code socket_session_server::on_message_RESPONSE_REGISTER_MATCHING(sMESSAGE_NETWORK& _msg)
{
	// return) 
	return eRESULT::DONE;
}

result_code socket_session_server::on_message_MATCH_ESTABLISHED(sMESSAGE_NETWORK& _msg)
{
	// 1) ...
	auto buf = _msg.buf_message + offset(8);

	// 2) message를 읽는다.
	/*auto account_id =*/ buf.extract<sACCOUNT_ID>();
	auto result = buf.extract<eRESULT>();

	// 3) 성공일 경우
	if (result == eRESULT::SUCCESS)
	{
		// -  
		auto addr_server = buf.extract<net::sockaddr>();
		auto buf_enter_key = buf.extract<shared_buffer>();

		// - get source socket
		auto psocket_battle = alloc_object<socket_match_channel>();

		// - prepare
		psocket_battle->prepare_socket();

		// - set channel info
		psocket_battle->m_checkin_key = std::move(buf_enter_key);

		// - 접속
		auto result = g_pdlg->m_pconnector_channel->request_connect(psocket_battle, addr_server);

		if (result == false)
		{
			closesocket();
		}
	}
	else
	{
		closesocket();
	}

	// return) 
	return eRESULT::DONE;
}