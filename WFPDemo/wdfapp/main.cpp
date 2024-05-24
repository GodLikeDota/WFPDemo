#include <windows.h>
#include <stdio.h>
#include <winioctl.h>
#define IOCTL_TEST CTL_CODE(FILE_DEVICE_UNKNOWN,0x8000,METHOD_BUFFERED,FILE_ANY_ACCESS )

int main()
{
	HANDLE hFile = CreateFile(L"\\\\.\\HelloWDF",
		GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("文件打开失败%x\n", GetLastError());
	}
	DWORD dwret;
	DeviceIoControl(hFile, IOCTL_TEST, NULL, 0, NULL, 0, &dwret, NULL);

	CloseHandle(hFile);
	return 0;
}