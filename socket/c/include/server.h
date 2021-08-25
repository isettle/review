#ifndef SOCKET_SERV_SOCKET_C_INCLUDE_SERVER_H_
#define SOCKET_SERV_SOCKET_C_INCLUDE_SERVER_H_
void echo_server(int port);
void echo_udp_server(int port);
void file_download_server(int port, const char *file);
#endif //SOCKET_SERV_SOCKET_C_INCLUDE_SERVER_H_
