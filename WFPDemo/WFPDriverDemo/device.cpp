#include "device.h"
#include "io_callbacks.h"

NTSTATUS device_init(WDFDRIVER driver, WDFDEVICE& device)
{
	DECLARE_CONST_UNICODE_STRING(device_name, L"\\Device\\libblock");
	DECLARE_CONST_UNICODE_STRING(dos_device_name, L"\\??\\libblock");

	NTSTATUS status = STATUS_SUCCESS;

	KdPrint(("[LIBBLOCK]device init begain"));

	auto device_init = WdfControlDeviceInitAllocate(driver, &SDDL_DEVOBJ_SYS_ALL_ADM_RWX_WORLD_RW_RES_R);
	if (!device_init)
	{
		status = STATUS_INSUFFICIENT_RESOURCES;
		KdPrint(("[LIBBLOCK]device_init failed to allocate WDF control device :%d", status));
		return status;
	}

	WdfDeviceInitSetDeviceType(device_init, FILE_DEVICE_NETWORK);
	WdfDeviceInitSetIoType(device_init, WdfDeviceIoBuffered);	// 缓冲模式，类似DO_BUFFERED_IO
	status = WdfDeviceInitAssignName(device_init, &device_name);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("[LIBBLOCK]device init failed to create WDF device name:%d", status));
		WdfDeviceInitFree(device_init);
		return status;
	}

	WDF_FILEOBJECT_CONFIG file_config;
	WDF_FILEOBJECT_CONFIG_INIT(&file_config, file_create, file_close, file_cleanup);
	WDF_OBJECT_ATTRIBUTES obj_attrs;
	WDF_OBJECT_ATTRIBUTES_INIT(&obj_attrs);
	obj_attrs.ExecutionLevel = WdfExecutionLevelPassive;
	obj_attrs.SynchronizationScope = WdfSynchronizationScopeNone;
	WdfDeviceInitSetFileObjectConfig(device_init, &file_config, &obj_attrs);
	WDF_OBJECT_ATTRIBUTES_INIT(&obj_attrs);

	status = WdfDeviceCreate(&device_init, &obj_attrs, &device);

	if (!NT_SUCCESS(status))
	{
		KdPrint(("[LIBBLOCK]device init failed to create WDF control device:%d", status));
		WdfDeviceInitFree(device_init);
		return status;
	}

	WDF_IO_QUEUE_CONFIG queue_config;
	WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queue_config, WdfIoQueueDispatchParallel);
	queue_config.EvtIoRead = NULL;
	queue_config.EvtIoWrite = NULL;
	queue_config.EvtIoDeviceControl = device_ioctl;		// 请求的处理函数
	WDF_OBJECT_ATTRIBUTES_INIT(&obj_attrs);
	obj_attrs.ExecutionLevel = WdfExecutionLevelPassive;
	obj_attrs.SynchronizationScope = WdfSynchronizationScopeNone;

	WDFQUEUE queue;
	status = WdfIoQueueCreate(device, &queue_config, &obj_attrs, &queue);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("[LIBCLOCK] device init failed to create default WDF queue:%d", status));
		WdfDeviceInitFree(device_init);
		return status;
	}
	status = WdfDeviceCreateSymbolicLink(device, &dos_device_name);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("[LIBBLOCK]device init failed to create symbolic link %d", status));
		WdfDeviceInitFree(device_init);
		return status;
	}
	WdfControlFinishInitializing(device);
	KdPrint(("[LIBBLOCK]device init end"));

	return status;
}

void device_uninit()
{
	// 不用删除符号链接，框架会自动删除
	KdPrint(("[LIBBLOCK]device_uninit"));
	UNICODE_STRING dos_device_name = { 0 };
	RtlInitUnicodeString(&dos_device_name, L"\\??\\libblock");
	IoDeleteSymbolicLink(&dos_device_name);
}