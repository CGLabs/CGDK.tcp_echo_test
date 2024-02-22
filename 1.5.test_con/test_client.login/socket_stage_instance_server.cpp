#include "pch.h"


void socket_stage_instance_server::on_request_connect(const net::sockaddr& /*_address*/)
{
	// 1) notify to dlg
	if (g_pdlg != nullptr)
	{
		//g_pdlg->m_dlg_battle.on_request_connect(this);
	}
}

void socket_stage_instance_server::on_connect()
{
	// 1) request enter
	this->send(this->m_buf_enter);
}

void socket_stage_instance_server::on_fail_connect(uint64_t /*_disconnect_reason*/)
{
	// 2) reset
	this->m_buf_enter.clear();

	this->m_count_prepared.store(0);
}

void socket_stage_instance_server::on_disconnect(uint64_t /*_reason*/)
{
	// 2) reset
	this->m_buf_enter.clear();
}

result_code	socket_stage_instance_server::on_message(sMESSAGE_NETWORK& _msg)
{
	// 1) set source
	_msg.set_source(this);

	// 2) execute message
	BEGIN_NESTED_CGMESSAGE_MAP
		CGMESSAGE_TYPE_SUB(eMESSAGE::SYSTEM::NETWORK, ((sMESSAGE_BUFFER&)_msg).buf_message.front<uint32_t>(4))
			ON_CGMESSAGE(eMESSAGE::CLIENT_CERTIFY_RESPONSE,			on_message_CLIENT_CERTIFY_RESPONSE, sMESSAGE_NETWORK);
			ON_CGMESSAGE(eMESSAGE::STAGE_INSTANCE_ENTER_RESPONSE,	on_message_STAGE_INSTANCE_ENTER_RESPONSE, sMESSAGE_NETWORK);
			ON_CGMESSAGE(eMESSAGE::MATCH_PREPARE,					on_message_MATCH_PREPARE,			sMESSAGE_NETWORK);
			ON_CGMESSAGE(eMESSAGE::MATCH_START,						on_message_MATCH_START,				sMESSAGE_NETWORK);
			ON_CGMESSAGE(eMESSAGE::MATCH_END,						on_message_MATCH_END,				sMESSAGE_NETWORK);
			ON_CGMESSAGE(eMESSAGE::MATCH_MEMBER_RESULT,				on_message_MATCH_MEMBER_RESULT,		sMESSAGE_NETWORK);

		END_SUB_CGMESSAGE_MAP
	END_NESTED_CGMESSAGE_MAP

	// 3) 메시지를 전달한다.
	TRANSMIT_MESSAGE(_msg);

	// return)
	return eRESULT::BYPASS;
}

result_code socket_stage_instance_server::on_message_CLIENT_CERTIFY_RESPONSE(sMESSAGE_NETWORK& _msg)
{
	// 1) ...
	auto buf = _msg.buf_message + offset(8);

	// 2) get result
	auto result = buf.extract<eRESULT>();

	if (result == eRESULT::SUCCESS)
	{

	}
	else
	{
		// - read result message
		auto result_message = buf.extract<std::string_view>();

		MessageBoxA(NULL, result_message.data(), "Fail", MB_OK);
	}

	// return) 
	return eRESULT::DONE;
}

result_code socket_stage_instance_server::on_message_STAGE_INSTANCE_ENTER_RESPONSE(sMESSAGE_NETWORK& _msg)
{
	// return) 
	return eRESULT::DONE;
}

void socket_stage_instance_server::process_functions_match()
{
}

result_code socket_stage_instance_server::on_message_MATCH_PREPARE(sMESSAGE_NETWORK& _msg)
{
	CGASSERT_ERROR(this->m_count_prepared.load() == 0);

	// 1) ready 신호를 전송한다.
	this->send(message_MATCH_READY());

	++this->m_count_prepared;

	// return) 
	return eRESULT::DONE;
}

result_code socket_stage_instance_server::on_message_MATCH_START(sMESSAGE_NETWORK& _msg)
{
	// 1) set timer
	//this->SetTimer(0, 2000, 0);

	// return) 
	return eRESULT::DONE;
}

result_code socket_stage_instance_server::on_message_MATCH_END(sMESSAGE_NETWORK& /*_msg*/)
{
	// return) 
	return eRESULT::DONE;
}

result_code socket_stage_instance_server::on_message_MATCH_MEMBER_RESULT(sMESSAGE_NETWORK& /*_msg*/)
{
	// return) 
	return eRESULT::DONE;
}