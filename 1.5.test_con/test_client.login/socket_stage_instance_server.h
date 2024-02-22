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

class socket_stage_instance_server :
	virtual public				Isocket,
	public						net::io::connect_requestable::Ntcp
{
private:
	virtual void				on_request_connect(const net::sockaddr& _address) override;
	virtual void				on_connect() override;
	virtual void				on_fail_connect(uint64_t _disconnect_reason) override;
	virtual	void				on_disconnect(uint64_t _reason) override;
	virtual result_code			on_message(sMESSAGE_NETWORK& _msg) override;
			result_code			on_message_CLIENT_CERTIFY_RESPONSE(sMESSAGE_NETWORK& _msg);
			result_code			on_message_STAGE_INSTANCE_ENTER_RESPONSE(sMESSAGE_NETWORK& _msg);
			result_code			on_message_MATCH_PREPARE(sMESSAGE_NETWORK& _msg);
			result_code			on_message_MATCH_START(sMESSAGE_NETWORK& _msg);
			result_code			on_message_MATCH_END(sMESSAGE_NETWORK& _msg);
			result_code			on_message_MATCH_MEMBER_RESULT(sMESSAGE_NETWORK& _msg);
public:
			void				process_functions_match();

			shared_buffer		m_buf_enter;
			std::atomic<int>	m_count_prepared = 0;
};
