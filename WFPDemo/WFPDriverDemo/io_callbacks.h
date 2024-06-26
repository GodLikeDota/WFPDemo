#pragma once
#include <ntddk.h>
#include <wdf.h>

VOID file_create(IN WDFDEVICE device, IN WDFREQUEST request, IN WDFFILEOBJECT object);
VOID file_close(IN WDFFILEOBJECT object);
VOID file_cleanup(IN WDFFILEOBJECT object);
VOID device_ioctl(IN WDFQUEUE queue, IN WDFREQUEST request, IN size_t out_length, IN size_t in_length, IN ULONG code);
