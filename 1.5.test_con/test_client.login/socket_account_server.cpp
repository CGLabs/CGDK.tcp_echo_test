#include "pch.h"
#include <random>


void socket_account_server::process_execute()
{
	lockable<>			m_cs_vector_area_point;
	world::AREA::sID	m_waid{ 0 };
	std::vector<world::STAGE::sINFO> m_vector_area_point;

	scoped_lock(this->m_cs_vector_area_point)
	{
		if (this->m_waid != 0)
		{
			//this->m_waid = buf_temp.extract<world::AREA::sID>();
			//this->m_vector_area_point = buf_temp.extract<std::vector<world::STAGE::sINFO>>();
		}
	}

}

void socket_account_server::on_request_connect(const net::sockaddr& /*_address*/)
{
}

void socket_account_server::on_connect()
{
	scoped_lock(this->m_cs_vector_area_point)
	{
		this->m_waid.clear();
		this->m_vector_area_point.clear();
	}

	// 1) get client info
	sCLIENT_INFO client_info;
	client_info.version; // sVERSION
	client_info.device_info; // sDEVICE_INFO
	client_info.os_info; // sOS_INFO
	client_info.mac = api::get_mac_address().front(); // sMAC_ADDRESS
	client_info.sockaddr_local; // net::sockaddr
	client_info.sockaddr_public = api::socket::instance().get_local_address_public_first(); // net::sockaddr

	// 2) send request message
	this->send(message_CLIENT_CERTIFY_REQUEST(client_info));
}

void socket_account_server::on_fail_connect(uint64_t /*_disconnect_reason*/)
{
}

void socket_account_server::on_disconnect(uint64_t /*_reason*/)
{
}

result_code	socket_account_server::on_message(sMESSAGE_NETWORK& _msg)
{
	// 1) set source
	_msg.set_source(this);

	// 2) execute message
	BEGIN_NESTED_CGMESSAGE_MAP
		CGMESSAGE_TYPE_SUB(eMESSAGE::SYSTEM::NETWORK, ((sMESSAGE_BUFFER&)_msg).buf_message.front<uint32_t>(4))
			ON_CGMESSAGE(eMESSAGE::CLIENT_CERTIFY_RESPONSE,		on_message_CLIENT_CERTIFY_RESPONSE,		sMESSAGE_NETWORK);
			ON_CGMESSAGE(eMESSAGE::ACCOUNT_LOGIN_RESPONSE,		on_message_ACCOUNT_LOGIN_RESPONSE,		sMESSAGE_NETWORK)
			ON_CGMESSAGE(eMESSAGE::ACCOUNT_CREATE_RESPONSE,		on_message_ACCOUNT_CREATE_RESPONSE,		sMESSAGE_NETWORK)
			ON_CGMESSAGE(eMESSAGE::WORLD_POINT_LIST_RESPONSE,	on_message_WORLD_POINT_LIST_RESPONSE,	sMESSAGE_NETWORK)
			ON_CGMESSAGE(eMESSAGE::WORLD_POINT_ENTER_RESPONSE,	on_message_WORLD_POINT_ENTER_RESPONSE,	sMESSAGE_NETWORK);
			ON_CGMESSAGE(eMESSAGE::WORLD_POINT_ENTER_CANCEL,	on_message_WORLD_POINT_ENTER_CANCEL,	sMESSAGE_NETWORK);
			ON_CGMESSAGE(eMESSAGE::WORLD_POINT_LEAVE_RESPONSE,	on_message_WORLD_POINT_LEAVE_RESPONSE,	sMESSAGE_NETWORK);
			ON_CGMESSAGE(eMESSAGE::MATCH_JOIN_REQUEST,			on_message_MATCH_JOIN_REQUEST,			sMESSAGE_NETWORK);
			ON_CGMESSAGE(eMESSAGE::MATCH_MEMBER_KICKOUT,		on_message_MATCH_MEMBER_KICKOUT,		sMESSAGE_NETWORK)
			ON_CGMESSAGE(eMESSAGE::MATCH_ESTABLISH,				on_message_MATCH_ESTABLISH,				sMESSAGE_NETWORK);
			ON_CGMESSAGE(eMESSAGE::MATCH_CANCEL,				on_message_MATCH_CANCEL,				sMESSAGE_NETWORK);
			ON_CGMESSAGE(eMESSAGE::MATCH_MEMBER_RESULT,			on_message_MATCH_MEMBER_RESULT,			sMESSAGE_NETWORK);
			ON_CGMESSAGE(eMESSAGE::MATCH_RESULT,				on_message_MATCH_RESULT,				sMESSAGE_NETWORK);
		END_SUB_CGMESSAGE_MAP
	END_NESTED_CGMESSAGE_MAP

	// 3) 메시지를 전달한다.
	TRANSMIT_MESSAGE(_msg);

	// return)
	return eRESULT::BYPASS;
}

result_code socket_account_server::on_message_CLIENT_CERTIFY_RESPONSE(sMESSAGE_NETWORK& _msg)
{
	// 1) ...
	auto buf = _msg.buf_message + offset(8);

	// 2) get result
	auto result = buf.extract<eRESULT>();

	if (result == eRESULT::SUCCESS)
	{
		this->send(message_ACCOUNT_LOGIN_REQUEST(1, this->m_key_checkin, api::socket::instance().get_local_address_public_first()));
	}
	else
	{
		this->closesocket();
	}

	// return) 
	return eRESULT::DONE;
}

result_code socket_account_server::on_message_ACCOUNT_LOGIN_RESPONSE(sMESSAGE_NETWORK& _msg)
{
	// 1) ...
	auto buf = _msg.buf_message + offset(8);

	// 2) get message
	/*auto result =*/ buf.extract<REQUEST_ID>();
	auto result = buf.extract<eRESULT>();

	// 3A) 성공하면 REQUEST_ENTER 메시지를 전송한다.
	if (result == eRESULT::SUCCESS)
	{
		// - key_checkin을 저장해 놓는다.
		this->m_account_info = buf.extract<sACCOUNT_INFO>();

		// - request enter area
		this->send(message_WORLD_POINT_LIST_REQUEST(1, 0x4001));
	}
	// 3B) ...
	else if (result == eRESULT::NO_MEMBER_INFO)
	{
		if (test_client_dlg::m_setting_connection_make_new_member_data == true)
		{
			// declare) 
			sACCOUNT_CREATE_INFO account_creat_info;

			// 1) get info
			{
				// - account info
				account_creat_info.nickname = NFMT::format("test_{}{}{}", rand(), rand(), rand());

				// - gruma info
				account_creat_info.gruma_name = NFMT::format("gruma_{}{}{}", rand(), rand(), rand());
				account_creat_info.gruma_cid = UID::CLASS::UNIT::GRUMA_BASE + 1;
			}

			// 2) send
			this->send(message_ACCOUNT_CREATE_REQUEST(1, account_creat_info));
		}
		else
		{
			this->closesocket();
		}
	}
	else
	{
		this->closesocket();
	}

	// return) 
	return eRESULT::DONE;
}

result_code socket_account_server::on_message_ACCOUNT_CREATE_RESPONSE(sMESSAGE_NETWORK& _msg)
{
	// 1) ...
	auto buf = _msg.buf_message + offset(8);

	// 2) get result
	auto request_id = buf.extract<REQUEST_ID>();
	auto result = buf.extract<eRESULT>();

	// 3-1) on SUCCESS
	if (result == eRESULT::SUCCESS)
	{
		// - set checkin info
		this->m_account_info = buf.extract<sACCOUNT_INFO>();

		sACCOUNT_CERTIFICATION temp_account_certification;
		temp_account_certification.certification_type = eCERTIFICATION_TYPE::NONE;
		temp_account_certification.aid = { this->m_account_info.aid.uid, 1 };
		temp_account_certification.time_certified = chrono::time::utc::now();
		temp_account_certification.time_expire = temp_account_certification.time_certified + 1h;

		// - checkin info를 작성한다
		auto buf_key_checkin = alloc_shared_buffer(2048);
		buf_key_checkin.append<sACCOUNT_CERTIFICATION>(temp_account_certification);

		// - LOGIN REQUUEST message 전송
		this->send(message_ACCOUNT_LOGIN_REQUEST(1, buf_key_checkin, api::socket::instance().get_local_address_public_first()));
	}
	// 3-2) on FAIL
	else
	{
		this->closesocket();
	}

	// return) 
	return eRESULT::DONE;
}


result_code socket_account_server::on_message_WORLD_POINT_LIST_RESPONSE(sMESSAGE_NETWORK& _msg)
{
	// 1) buffer for extract
	auto buf_temp = _msg.buf_message + offset(8);

	// 2) object_event를 append한다.
	auto request_id = buf_temp.extract<REQUEST_ID>();
	auto result = buf_temp.extract<eRESULT>();

	if (result == eRESULT::SUCCESS)
	{
		// - extract area point info
		scoped_lock(this->m_cs_vector_area_point)
		{
			this->m_waid = buf_temp.extract<world::AREA::sID>();
			this->m_vector_area_point = buf_temp.extract<std::vector<world::STAGE::sINFO>>();
		}

		// - request enter
		{
			world::STAGE::sID stage_id;

			scoped_lock(this->m_cs_vector_area_point)
			{
				RETURN_IF(this->m_vector_area_point.empty(), eRESULT::DONE);

				auto index = rand() % this->m_vector_area_point.size();
				stage_id = this->m_vector_area_point[index].id;
			}

			// - get unit
			std::vector<eid_t> vector_unit_eids;
			scoped_lock(this->m_cs_account_info)
			{
				RETURN_IF(this->m_account_info.map_entity.empty(), eRESULT::DONE);

				vector_unit_eids.push_back(this->m_account_info.map_entity.begin()->second->m_entity_info.eid);
			}

			this->send(message_WORLD_POINT_ENTER_REQUEST(1, this->m_account_info.aid, stage_id, vector_unit_eids));
		}
	}
	else
	{
		this->closesocket();
	}

	// return) 
	return eRESULT::DONE;
}


result_code socket_account_server::on_message_WORLD_POINT_ENTER_RESPONSE(sMESSAGE_NETWORK& _msg)
{
	// 1) buffer for extract
	auto buf_temp = _msg.buf_message + offset(8);

	// 2) object_event를 append한다.
	auto request_id = buf_temp.extract<REQUEST_ID>();
	auto result = buf_temp.extract<eRESULT>();

	if (result == eRESULT::SUCCESS)
	{
		/*auto aid =*/ buf_temp.extract<sMATCH_REQUESTOR>();
		auto area_point_info = buf_temp.extract<world::STAGE::sINFO>();
	}
	else
	{
	}

	// return) 
	return eRESULT::DONE;
}

result_code socket_account_server::on_message_WORLD_POINT_ENTER_CANCEL(sMESSAGE_NETWORK& /*_msg*/)
{
	//// 1) buffer for extract
	//auto buf_temp = _msg.buf_message + offset(8);

	//// 2) object_event를 append한다.
	//auto request_id = buf_temp.extract<REQUEST_ID>();
	//auto result = buf_temp.extract<eRESULT>();
	//auto aid = buf_temp.extract<sACCOUNT_ID>();
	//auto wpid = buf_temp.extract<world::STAGE::sID>();
	//auto result_message = buf_temp.extract<std::string>();

	//// 3) clolse...
	//this->m_dlg_stage_enter_waiting.ShowWindow(SW_HIDE);

	//// 4) ...
	//::MessageBoxA(m_hWnd, result_message.c_str(), "request cancled", MB_OK);

	// return) 
	return eRESULT::DONE;
}

result_code socket_account_server::on_message_WORLD_POINT_LEAVE_RESPONSE(sMESSAGE_NETWORK& /*_msg*/)
{
	//// 1) buffer for extract
	//auto buf_temp = _msg.buf_message + offset(8);

	//// 2) object_event를 append한다.
	//auto request_id = buf_temp.extract<REQUEST_ID>();
	//auto result = buf_temp.extract<eRESULT>();
	//auto wpid = buf_temp.extract<world::STAGE::sID>();

	//RETURN_IF(wpid != this->m_dlg_stage_enter_waiting.m_area_stage_info.id, eRESULT::DONE);

	//if (result != eRESULT::SUCCESS)
	//{
	//	auto result_message = buf_temp.extract<std::string>();
	//	::MessageBoxA(m_hWnd, result_message.c_str(), "failed", MB_OK);
	//}

	//this->m_dlg_stage_enter_waiting.ShowWindow(SW_HIDE);

	// return) 
	return eRESULT::DONE;
}

result_code socket_account_server::on_message_MATCH_JOIN_REQUEST(sMESSAGE_NETWORK& /*_msg*/)
{
	// return) 
	return eRESULT::DONE;
}

result_code socket_account_server::on_message_MATCH_ESTABLISH(sMESSAGE_NETWORK& _msg)
{
	// 1) buffer for extract
	auto buf_temp = _msg.buf_message + offset(8);

	// 2) writee message
	/*auto wpid =*/ buf_temp.extract<world::STAGE_INSTANCE::sID>();
	auto requestor = buf_temp.extract<sMATCH_REQUESTOR>();

	// check) aid is valid
	RETURN_IF(requestor.aid.uid == 0, eRESULT::DONE);

	auto addr_server = buf_temp.extract<net::sockaddr>();
	auto buf_enter_key = buf_temp.extract<buffer_view>();

	// 3) alloc socket
	auto psocket = make_object<socket_stage_instance_server>();

	// 4) set message
	psocket->m_buf_enter = message_ACCOUNT_CHECKIN_REQUEST(1, buf_enter_key, api::socket::instance().get_local_address_public_first());

	// 6) connect
	test_client_dlg::m_pconnector_stage_instance->request_connect(psocket, addr_server);

	// return) 
	return eRESULT::DONE;
}

result_code socket_account_server::on_message_MATCH_MEMBER_KICKOUT(sMESSAGE_NETWORK& /*_msg*/)
{
	//// 1) close dialog
	//this->m_dlg_stage_enter_waiting.ShowWindow(SW_HIDE);

	// return) 
	return eRESULT::DONE;
}

result_code socket_account_server::on_message_MATCH_CANCEL(sMESSAGE_NETWORK& /*_msg*/)
{
	//// 1) buffer for extract
	//auto buf_temp = _msg.buf_message + offset(8);

	//// 2) writee message
	///*auto rid =*/ buf_temp.extract<REQUEST_ID>();
	///*auto wpid =*/ buf_temp.extract<world::STAGE::sID>();
	//auto aid = buf_temp.extract<sACCOUNT_ID>();
	//auto result = buf_temp.extract<eRESULT>();

	//// 3) ...
	//AfxMessageBox(TEXT("Error"), MB_OK);

	// return) 
	return eRESULT::DONE;
}

result_code socket_account_server::on_message_MATCH_MEMBER_RESULT(sMESSAGE_NETWORK& /*_msg*/)
{
	// return) 
	return eRESULT::DONE;
}

result_code socket_account_server::on_message_MATCH_RESULT(sMESSAGE_NETWORK& /*_msg*/)
{
	// return) 
	return eRESULT::DONE;
}