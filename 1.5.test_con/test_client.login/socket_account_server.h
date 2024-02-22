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

class socket_account_server :
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
			result_code			on_message_ACCOUNT_LOGIN_RESPONSE(sMESSAGE_NETWORK& _msg);
			result_code			on_message_ACCOUNT_CREATE_RESPONSE(sMESSAGE_NETWORK& _msg);
			result_code			on_message_WORLD_POINT_LIST_RESPONSE(sMESSAGE_NETWORK& _msg);
			result_code			on_message_WORLD_POINT_ENTER_RESPONSE(sMESSAGE_NETWORK& _msg);
			result_code			on_message_WORLD_POINT_ENTER_CANCEL(sMESSAGE_NETWORK& _msg);
			result_code			on_message_WORLD_POINT_LEAVE_RESPONSE(sMESSAGE_NETWORK& _msg);
			result_code			on_message_MATCH_JOIN_REQUEST(sMESSAGE_NETWORK& _msg);
			result_code			on_message_MATCH_MEMBER_KICKOUT(sMESSAGE_NETWORK& _msg);
			result_code			on_message_MATCH_ESTABLISH(sMESSAGE_NETWORK& _msg);
			result_code			on_message_MATCH_CANCEL(sMESSAGE_NETWORK& _msg);
			result_code			on_message_MATCH_MEMBER_RESULT(sMESSAGE_NETWORK& _msg);
			result_code			on_message_MATCH_RESULT(sMESSAGE_NETWORK& _msg);


			void				process_execute();
public:
			sSERVER_CONNECTION_INFO m_server_connection_info;
			shared_buffer		m_key_checkin;
			lockable<>			m_cs_account_info;
			sACCOUNT_INFO		m_account_info;

			lockable<>			m_cs_vector_area_point;
			world::AREA::sID	m_waid{ 0 };
			std::vector<world::STAGE::sINFO> m_vector_area_point;
};
