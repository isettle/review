#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/errno.h>

#define BACKLOG 10
#define BUFFER_SIZE 100

/**
 * 回声服务器
 */
void echo_server(int port) {
  // 创建套接字
  int serv_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  // ip4地址结构体
  // 与sockaddr区别：sockaddr为通用结构，并且sockaddr.sa_data无相关函数转换为有效的char给其赋值。sockadd_in与sockaddr长度一致，可以无损强转
  // ip6地址结构体为 sockaddr_in6
  struct sockaddr_in serve_addr;
  memset(&serve_addr, 0, sizeof(serve_addr));
  serve_addr.sin_family = PF_INET;
  serve_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serve_addr.sin_port = htons(port);
  // 绑定地址族、IP与端口
  bind(serv_socket, (struct sockaddr *) &serve_addr, sizeof(serve_addr));

  // 监听
  // backlog 缓冲区request.queue长度 (可默认为[SOMAXCONN]由系统调配)
  listen(serv_socket, BACKLOG);

  // 接收客户端请求
  struct sockaddr_in client_addr;
  socklen_t client_addr_size = sizeof(client_addr);
  char buffer[BUFFER_SIZE] = {0};
  while (1) {
    int client_socket =
        accept(serv_socket, (struct sockaddr *) &client_addr, &client_addr_size);

    // 回声
    read(client_socket, buffer, sizeof(buffer) - 1);
    write(client_socket, buffer, sizeof(buffer));

    // 清理工作
    close(client_socket);            // 关闭客户端套接字
    memset(buffer, 0, BUFFER_SIZE);  // 重置数据缓冲区
  }

  // 关闭服务端套接字
  close(serv_socket);
}

/**
 * 服务器文件下载
 */
void file_download_server(int port, const char *file) {
  FILE *fp = fopen(file, "rb");
  if (fp == NULL) {
    printf("[%s]打开文件失败: %s", file, strerror(errno));
    exit(1);
  }

  // 创建套接字
  int serv_socket = socket(PF_INET, SOCK_STREAM, 0);
  // 绑定
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = PF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(port);
  int bind_res = bind(serv_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  if (bind_res != 0) {
    perror("socket_serve绑定失败");
    close(serv_socket);
    exit(1);
  }
  // 监听
  int listen_res = listen(serv_socket, BACKLOG);
  if (listen_res != 0) {
    perror("socket_serve监听失败");
    close(serv_socket);
    exit(1);
  }
  // 接收请求
  struct sockaddr clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);
  int clnt_socket = accept(serv_socket, &clnt_addr, &clnt_addr_size);

  // 下载
  char buffer[BUFFER_SIZE] = {0};
  size_t nCount;
  while ((nCount = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
    write(clnt_socket, buffer, nCount);
  }
  shutdown(serv_socket, SHUT_WR);  // 断开输出流
  read(serv_socket, buffer, sizeof(buffer));

  // 清理工作
  fclose(fp);
  close(serv_socket);
  close(clnt_socket);
}

void echo_udp_server(int port) {
  // create socket
  int serv_socket = socket(PF_INET, SOCK_DGRAM, 0);
  // bind
  struct sockaddr_in sock_addr;
  memset(&sock_addr, 0, sizeof(sock_addr));
  sock_addr.sin_family = PF_INET;
  sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  sock_addr.sin_port = htons(port);
  int bind_res = bind(serv_socket, (struct sockaddr *) &sock_addr, sizeof(sock_addr));
  if (bind_res != 0) {
    perror("udp echo server bind failed:");
    close(serv_socket);
    exit(1);
  }
  // client
  struct sockaddr_in sock_clnt;
  socklen_t sock_clnt_size = sizeof(sock_clnt);

  // recv
  char *buffer_recv[BUFFER_SIZE] = {0};
  while (1) {
    ssize_t recv_res =
        recvfrom(serv_socket, buffer_recv, sizeof(buffer_recv), 0, (struct sockaddr *) &sock_clnt, &sock_clnt_size);
    if (recv_res == -1) {
      perror("udp echo server recv failed:");
      close(serv_socket);
      exit(1);
    }
    if (strcmp((const char *) buffer_recv, "quit") == 0) {
      break;
    }
    // send
    sendto(serv_socket, buffer_recv, sizeof(buffer_recv), 0, (struct sockaddr *) &sock_clnt, sock_clnt_size);
    memset(buffer_recv, 0, BUFFER_SIZE);
  }

  close(serv_socket);
}
