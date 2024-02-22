#ifndef PCH_H
#define PCH_H

#include "framework.h"

#include "cgdk/sdk10/net.socket.h"
#include "cgdk/sdk10/net.socket.mfc_controls.h"
#include "cgdk/sdk10/server.mfc_controls.h"

using namespace CGDK;


// 1) definitions
#include "../../common/common.h"

#include "Isocket.h"
#include "socket_certification_server.h"
#include "socket_stage_instance_server.h"
#include "socket_account_server.h"

#include "test_client.login.h"
#include "test_client.login_dlg.h"

extern	test_client_dlg* g_pdlg;

#endif //PCH_H
