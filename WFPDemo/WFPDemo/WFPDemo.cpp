// WFPDemo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <WS2tcpip.h>
#include "libblock.h"
#include <iostream>
using namespace std;
#pragma comment(lib, "ws2_32.lib")
int main()
{
    HANDLE engine_handle;
    CLibBlock libBlock;
    libBlock.libblock_init(LIBBLOCK_INIT_IPV4 | LIBBLOCK_INIT_IPV6, &engine_handle);
    HANDLE handle = libBlock.libblock_open();

    rule_t rule;
    memset(&rule, 0, sizeof(rule_t));
    rule.action = BLOCK;
    strcpy(rule.remote_addr_info.domin_str, "www.baidu.com");
    rule.remote_addr_info.ip_version = 4;
    rule.remote_addr_info.v4.cnt = 2;
    inet_pton(AF_INET, "180.101.50.188", &rule.remote_addr_info.v4.domain_address[0]);

    libBlock.libblock_set_domain(handle, rule);

    system("pause");

    libBlock.libblock_del_domain(handle, rule);
    libBlock.libblock_uninit(engine_handle);
    libBlock.libblock_close(handle);

    return 0;
}


