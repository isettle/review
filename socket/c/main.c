#include <stdio.h>
#include <string.h>
#include "include/server.h"
#include "include/client.h"

int server(const char *func);
int client(const char *func);

/**
 * 结合到工程，方便调试
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("缺少参数");
    return 1;
  }

  if (strcmp(argv[1], "server") == 0) {
    server(argv[2]);
  } else if (strcmp(argv[1], "client") == 0) {
    client(argv[2]);
  } else {
    printf("参数错误：server OR client 二选一");
    return 1;
  }
}

int server(const char *func) {
  // 回声服务器
  if (strcmp(func, "echo") == 0) {
    echo_server(9501);
    return 0;
  }
  // 下载服务器
  if (strcmp(func, "download") == 0) {
    file_download_server(9502, "../test.amr");
    return 0;
  }
  // UDP回声
  if(strcmp(func, "echo_udp") == 0) {
    echo_udp_server(9503);
  }

  printf("server服务器参数错误：echo/echo_udp/download");
  return 1;
}

int client(const char *func) {
  // 回声客户端
  if (strcmp(func, "echo") == 0) {
    echo_client();
    return 0;
  }
  // 下载客户端
  if (strcmp(func, "download") == 0) {
    file_download_client();
    return 0;
  }
  // UDP回声
  if(strcmp(func, "echo_udp") == 0) {
    echo_udp_client(9503);
  }

  printf("client服务器参数错误：echo/echo_udp/download");
  return 1;
}