project(SOCKET_SERV)
set(SRC_LIST main.c module/server.c module/client.c)
add_executable(socket_serv ${SRC_LIST})