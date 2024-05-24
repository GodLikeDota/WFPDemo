#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#include <fwpmu.h>
#include <fwpstypes.h>
#include "common.h"
#include <iostream>
using namespace std;
#define LIBBLOCK_INIT_IPV4	1
#define LIBBLOCK_INIT_IPV6	2
class CLibBlock {
public:
	CLibBlock();
	DWORD libblock_init(DWORD init_type, HANDLE* engine_handle);
	HANDLE libblock_open();

	int libblock_set_domain(HANDLE handle, const rule_t& conn);
	int libblock_del_domain(HANDLE handle, const rule_t& conn);
	int libblock_uninit(HANDLE engine_handle);
	void libblock_close(HANDLE handle);

};



