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
// socket_session_server
//
// ----------------------------------------------------------------------------
class socket_session_server :
	virtual public				Isocket,
	public						net::socket::tcp<>
{
public:
			socket_session_server();
	virtual ~socket_session_server();

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
			result_code			on_message_RESPONSE_ACCOUNT_LOGIN(sMESSAGE_NETWORK& _msg);
			result_code			on_message_RESPONSE_REGISTER_MATCHING(sMESSAGE_NETWORK& _msg);
			result_code			on_message_MATCH_ESTABLISHED(sMESSAGE_NETWORK& _msg);

public:
	// - ...
			shared_buffer		m_message_REQUEST_ACCOUNT_LOGIN;

	// - AID & Checkin Message
			shared_buffer		m_checkin_key;

	// - Performance
			query_performance	m_perforance_connect;
};
