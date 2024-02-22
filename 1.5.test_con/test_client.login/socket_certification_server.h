//*****************************************************************************
//*                                                                           *
//*                               project ok                               *
//*                                                                           *
//*                                  client                                   *
//*                                                                           *
//*                                                                           *
//*                                                                           *
//*                                                                           *
//*                  ¨Ï2017. Cho SangHyun. All rights reserved                *
//*                          http://www.CGCIII.co.kr                          *
//*                                                                           *
//*****************************************************************************

#pragma once

class socket_certification_server :
	virtual public				Isocket,
	public						net::io::connect_requestable::Ntcp
{
protected:
	virtual void				on_request_connect(const net::sockaddr& _address) override;
	virtual void				on_connect() override;
	virtual void				on_fail_connect(uint64_t _disconnect_reason) override;
	virtual	void				on_disconnect(uint64_t _reason) override;
	virtual result_code			on_message(sMESSAGE_NETWORK& _msg) override;
			result_code			on_message_CLIENT_CERTIFY_RESPONSE(sMESSAGE_NETWORK& _msg);
public:
			shared_buffer		m_buf_certification;

			std::string			m_email_id;
			std::string			m_email_domain;
			std::string			m_password;

};
