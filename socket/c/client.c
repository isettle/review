#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
void echo();
void file_download();

int main() {
  // echo();
  file_download();
  return 0;
}

/**
 * 回声客户端
 */
void echo() {
  // ip4地址结构体
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = PF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(9501);

  char buffer_send[BUFFER_SIZE] = {0};
  char buffer_recv[BUFFER_SIZE] = {0};

  while (1) {
    // 创建套接字
    int serv_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    // 连接
    int conn_err =
        connect(serv_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (conn_err == SO_ERROR) {
      perror("连接失败");
      close(serv_socket);
      exit(1);
    }

    printf("输入：");
    gets(buffer_send);
    // 回声
    write(serv_socket, buffer_send, sizeof(buffer_send));
    read(serv_socket, buffer_recv, sizeof(buffer_recv));
    printf("Message form server: %s\n", buffer_recv);

    // 清理
    close(serv_socket);
    memset(buffer_recv, 0, BUFFER_SIZE);
    memset(buffer_send, 0, BUFFER_SIZE);
  }
}

/**
 * 文件下载
 */
void file_download() {
  printf("保存文件名称:");
  char fileName[20] = {0};
  scanf("%s", fileName);

  FILE *fp;
  if ((fp = fopen(fileName, "wb")) == NULL) {
    perror("创建文件失败");
    exit(0);
  }

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = PF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(9502);

  // 创建套接字
  int serv_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  // 连接
  int conn_err =
      connect(serv_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (conn_err == SO_ERROR) {
    perror("连接失败");
    close(serv_socket);
    exit(1);
  }

  // 读取
  char buffer[BUFFER_SIZE] = {0};
  int nCount;
  while ((nCount = read(serv_socket, buffer, BUFFER_SIZE)) > 0) {
    fwrite(buffer, nCount, 1, fp);
  }
  puts("文件下载完成");

  // 清理
  close(serv_socket);
  fclose(fp);
}