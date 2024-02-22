#include "pch.h"
#include <random>


void socket_certification_server::on_request_connect(const net::sockaddr& /*_address*/)
{
}

void socket_certification_server::on_connect()
{
	// check)
	CGASSERT_ERROR(m_buf_certification.empty() == false);

	// 1) get client info
	sCLIENT_INFO client_info;
	client_info.version; // sVERSION
	client_info.device_info; // sDEVICE_INFO
	client_info.os_info; // sOS_INFO
	client_info.mac = api::get_mac_address().front(); // sMAC_ADDRESS
	client_info.sockaddr_local; // net::sockaddr
	client_info.sockaddr_public = api::socket::instance().get_local_address_public_first(); // net::sockaddr

	// 2) send
	send(message_CLIENT_CERTIFY_REQUEST(client_info));
}

void socket_certification_server::on_fail_connect(uint64_t /*_disconnect_reason*/)
{
}

void socket_certification_server::on_disconnect(uint64_t /*_reason*/)
{
}

result_code	socket_certification_server::on_message(sMESSAGE_NETWORK& _msg)
{
	// 1) set source
	_msg.set_source(this);

	// 2) execute message
	BEGIN_NESTED_CGMESSAGE_MAP
		CGMESSAGE_TYPE_SUB(eMESSAGE::SYSTEM::NETWORK, ((sMESSAGE_BUFFER&)_msg).buf_message.front<uint32_t>(4))
			ON_CGMESSAGE(eMESSAGE::CLIENT_CERTIFY_RESPONSE, on_message_CLIENT_CERTIFY_RESPONSE, sMESSAGE_NETWORK);
		END_SUB_CGMESSAGE_MAP
	END_NESTED_CGMESSAGE_MAP

	// 3) 메시지를 전달한다.
	TRANSMIT_MESSAGE(_msg);

	// return)
	return eRESULT::BYPASS;
}

result_code socket_certification_server::on_message_CLIENT_CERTIFY_RESPONSE(sMESSAGE_NETWORK& _msg)
{
	// 1) ...
	auto buf = _msg.buf_message + offset(8);

	// 2) get result
	auto result = buf.extract<eRESULT>();

	if (result == eRESULT::SUCCESS)
	{
		send(m_buf_certification);
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
