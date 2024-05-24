#define INITGUID
#include "libblock.h"
#include <cassert>


#pragma comment(lib, "Fwpuclnt.lib")

CLibBlock::CLibBlock()
{

}
DWORD CLibBlock::libblock_init(DWORD init_type, HANDLE* engine_handle)
{
	cout << "libblock init" << endl;
	FWPM_SESSION session = { 0 };
	session.flags = FWPM_SESSION_FLAG_DYNAMIC;		// session�������Զ��������е�callout��filter
	auto status = FwpmEngineOpen(nullptr, RPC_C_AUTHN_WINNT, nullptr, &session, engine_handle);
	if (status != ERROR_SUCCESS)
	{
		return status;
	}

	status = FwpmTransactionBegin(*engine_handle, 0);
	if (status != ERROR_SUCCESS)
	{
		return status;
	}

	if (init_type & LIBBLOCK_INIT_IPV4)
	{
		FWPM_CALLOUT callout_v4 = { 0 };
		FWPM_DISPLAY_DATA display_data = { 0 };
		wchar_t callout_display_name_v4[] = L"LXBlockCalloutV4";
	    wchar_t callout_display_desc_v4[] = L"ipv4 callout for LIBBLOCK";
		display_data.name = callout_display_name_v4;
		display_data.description = callout_display_desc_v4;

		callout_v4.calloutKey = LIBBLOCK_CALLOUT_GUID_V4;
		callout_v4.displayData = display_data;
		callout_v4.applicableLayer = FWPM_LAYER_ALE_FLOW_ESTABLISHED_V4;
		callout_v4.flags = 0;
		status = FwpmCalloutAdd(*engine_handle, &callout_v4, nullptr, nullptr);
		if (status != ERROR_SUCCESS)
		{
			FwpmTransactionAbort(*engine_handle);		// �ն˵�ǰ�Ļػ����ع�
			return status;
		}
		cout << "libblock init:add ipv4 block callout" << endl;
	}
	if (init_type & LIBBLOCK_INIT_IPV6)
	{
		FWPM_CALLOUT callout_v6 = { 0 };
		FWPM_DISPLAY_DATA display_data_v6 = { 0 };
		wchar_t callout_display_name_v6[] = L"LXBlockCalloutV6";
		wchar_t callout_display_desc_v6[] = L"ipv6 callout for LIBBLOCK";
		display_data_v6.name = callout_display_name_v6;
		display_data_v6.description = callout_display_desc_v6;

		callout_v6.calloutKey = LIBBLOCK_CALLOUT_GUID_V6;
		callout_v6.displayData = display_data_v6;
		callout_v6.applicableLayer = FWPM_LAYER_ALE_FLOW_ESTABLISHED_V6;
		callout_v6.flags = 0;
		status = FwpmCalloutAdd(*engine_handle, &callout_v6, nullptr, nullptr);
		if (status != ERROR_SUCCESS)
		{
			FwpmTransactionAbort(*engine_handle);
			return status;
		}
		cout << "libblock init:add ipv6 block callout" << endl;
	}

	// ����Ӳ�
	FWPM_SUBLAYER sublayer = { 0 };
	sublayer.subLayerKey = LIBBLOCK_SUBLAYER_GUID;
	wchar_t sublayer_display_name[] = L"LIBBLOCKSublayer";
	sublayer.displayData.name = sublayer_display_name;
	wchar_t sublayer_display_desc[] = L"Sublayer for LIBBLOCK";
	sublayer.displayData.description = sublayer_display_desc;
	sublayer.flags = 0;
	sublayer.weight = 0x0f;
	status = FwpmSubLayerAdd(*engine_handle, &sublayer, nullptr);
	if (status != ERROR_SUCCESS)
	{
		FwpmTransactionAbort(*engine_handle);
		return status;
	}
	cout << "libblock init:add block sublayer" << endl;

	//������
	if (init_type & LIBBLOCK_INIT_IPV4)
	{
		FWPM_FILTER filter_v4 = { 0 };
		UINT64 filter_id_v4 = 0;
		wchar_t filter_display_name_v4[] = L"LXLIBBLOCKFilterV4";
		filter_v4.displayData.name = filter_display_name_v4;
		wchar_t filter_display_desc_v4[] = L"IPV4 filter for LIBBLOCK";
		filter_v4.displayData.description = filter_display_desc_v4;
		filter_v4.action.type = FWP_ACTION_CALLOUT_TERMINATING;	//����ʼ�շ�����ֹ������ı�ע
		filter_v4.subLayerKey = LIBBLOCK_SUBLAYER_GUID;
		filter_v4.weight.type = FWP_UINT8;
		filter_v4.weight.uint8 = 0xf;
		filter_v4.numFilterConditions = 0;
		filter_v4.layerKey = FWPM_LAYER_ALE_FLOW_ESTABLISHED_V4;
		filter_v4.action.calloutKey = LIBBLOCK_CALLOUT_GUID_V4;
		status = FwpmFilterAdd(*engine_handle, &filter_v4, nullptr, &filter_id_v4);
		if (status != ERROR_SUCCESS)
		{
			FwpmTransactionAbort(*engine_handle);
		}
		cout << "libblock init : add ipv4 block filter" << endl;
	}

	if (init_type & LIBBLOCK_INIT_IPV6)
	{
		FWPM_FILTER filter_v6 = { 0 };
		UINT64 filter_id_v6 = 0;
		wchar_t filter_display_name_v6[] = L"LXLIBBLOCKFilterV6";
		filter_v6.displayData.name = filter_display_name_v6;
		wchar_t filter_display_desc_v6[] = L"IPV6 filter for LIBBLOCK";
		filter_v6.displayData.description = filter_display_desc_v6;
		filter_v6.action.type = FWP_ACTION_CALLOUT_TERMINATING;
		filter_v6.subLayerKey = LIBBLOCK_SUBLAYER_GUID;
		filter_v6.weight.type = FWP_UINT8;
		filter_v6.weight.uint8 = 0xf;
		filter_v6.numFilterConditions = 0;
		filter_v6.layerKey = FWPM_LAYER_ALE_FLOW_ESTABLISHED_V6;
		filter_v6.action.calloutKey = LIBBLOCK_CALLOUT_GUID_V6;
		status = FwpmFilterAdd(*engine_handle, &filter_v6, nullptr, &filter_id_v6);
		if (status != ERROR_SUCCESS)
		{
			FwpmTransactionAbort(*engine_handle);
			return status;
		}

		cout << "libblock init: add ipv6 block filter" << endl;
	
	}
	//�ڵ�ǰ�Ự���ύ��ǰ����
	status = FwpmTransactionCommit(*engine_handle);

	return status;
}

HANDLE CLibBlock::libblock_open()
{
	HANDLE handle = CreateFile(L"\\\\.\\LIBBLOCK",
		GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, INVALID_HANDLE_VALUE);
	cout << "libblock open handle" << handle << endl;
	return handle;
}

int CLibBlock::libblock_set_domain(HANDLE handle, const rule_t& conn)
{
	DWORD recv_num;
	DeviceIoControl(handle, IOCTL_SET_DOMAIN, (LPVOID)&conn, sizeof(rule_t), nullptr, 0, &recv_num, nullptr);

	return recv_num;
}

int CLibBlock::libblock_del_domain(HANDLE handle, const rule_t& conn)
{
	DWORD recv_num;
	DeviceIoControl(handle, IOCTL_DEL_DOMAIN, (LPVOID)&conn, sizeof(rule_t), nullptr, 0, &recv_num, nullptr);
	return recv_num;
}

int CLibBlock::libblock_uninit(HANDLE engine_handle)
{
	cout << "libblock uninit" << endl;
	FwpmEngineClose(engine_handle);
	return 0;
}

void CLibBlock::libblock_close(HANDLE handle)
{
	cout << "libblock close" << endl;
	CloseHandle(handle);
}
