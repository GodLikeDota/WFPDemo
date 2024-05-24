#include <ntddk.h>
#include <wdf.h>

VOID EvtDriverUnload(IN WDFDRIVER Driver)
{
	KdPrint(("驱动卸载\n"));
}

void EvtWdfDeviceFileCreate(
	IN WDFDEVICE Device,
	IN WDFREQUEST Request,
	IN WDFFILEOBJECT FileObject
)
{
	KdPrint(("create\n"));
	WdfRequestComplete(Request, STATUS_SUCCESS);
}
void EvtWdfFileClose(
	IN WDFFILEOBJECT FileObject
)
{
	KdPrint(("EvtWdfFileClose\n"));
}

void EvtWdfFileCleanup(
	IN WDFFILEOBJECT FileObject
)
{
	KdPrint(("EvtWdfFileCleanup\n"));
}

void EvtWdfIoQueueIoDeviceControl(
	IN WDFQUEUE Queue,
	IN WDFREQUEST Request,
	IN size_t OutputBufferLength,
	IN size_t InputBufferLength,
	IN ULONG IoControlCode
)
{
	KdPrint(("EvtWdfIoQueueIoDeviceControl\n"));
	WdfRequestComplete(Request, STATUS_SUCCESS);
}

// hcdedit /set testsigning on    开启64位下驱动的测试模式
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status = STATUS_SUCCESS;
	WDF_DRIVER_CONFIG config;
	WDFDRIVER Driver;
	WDF_FILEOBJECT_CONFIG fileConfig;
	WDFDEVICE Device;
	PWDFDEVICE_INIT DeviceInit;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\HelloWDF");
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloWDF");
	WDF_IO_QUEUE_CONFIG ioConfig;
	WDFQUEUE Queue;

	WDF_DRIVER_CONFIG_INIT(&config, NULL);
	config.DriverInitFlags = WdfDriverInitNonPnpDriver;	// no pnp
	config.EvtDriverUnload = EvtDriverUnload;

	status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES,&config,&Driver);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("驱动框架创建失败\n"));
	}
	else
	{
		KdPrint(("驱动框架创建成功\n"));
	}

	DeviceInit = WdfControlDeviceInitAllocate(Driver, &SDDL_DEVOBJ_SYS_ALL_ADM_RWX_WORLD_RW_RES_R);	// SDDL_DEVOBJ_SYS_ALL_ADM_RWX_WORLD_RW_RES_R  安全描述符，具体看msdn
	if (DeviceInit == NULL)
	{
		status = STATUS_INSUFFICIENT_RESOURCES;
		goto End;
	}
	status = WdfDeviceInitAssignName(DeviceInit, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("设备名分配失败%x\n", status));
		goto End;
	}

	WDF_FILEOBJECT_CONFIG_INIT(&fileConfig, EvtWdfDeviceFileCreate, EvtWdfFileClose, EvtWdfFileCleanup);
	WdfDeviceInitSetFileObjectConfig(DeviceInit, &fileConfig, WDF_NO_OBJECT_ATTRIBUTES);

	WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioConfig, WdfIoQueueDispatchSequential);
	ioConfig.EvtIoDeviceControl = EvtWdfIoQueueIoDeviceControl;

	status = WdfDeviceCreate(&DeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &Device);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("设备创建失败%x\n", status));
		goto End;
	}

	status = WdfDeviceCreateSymbolicLink(Device, &SymbolicLinkName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("符号链接创建失败%x\n", status));
		goto End;
	}
	status = WdfIoQueueCreate(Device, &ioConfig, WDF_NO_OBJECT_ATTRIBUTES, &Queue);

	if (!NT_SUCCESS(status))
	{
		KdPrint(("队列创建失败%x\n", status));
		goto End;
	}

	WdfControlFinishInitializing(Device);

End:
	return status;
}