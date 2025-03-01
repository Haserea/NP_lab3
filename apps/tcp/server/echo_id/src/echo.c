#include "echo.h"
#include <windows.h>
#include <time.h>

SOCKET server_socket = -1;

// Прототип функции потока
DWORD WINAPI ClientHandler(LPVOID lpParam);

void free_socket()
{
    if (server_socket > 0)
        closesocket(server_socket);
}

void usage(const char* exe_name)
{
    printf("Usage:\n\t%s -p <port> -q <que_size>\n", exe_name);
}

int start(int argc, char* argv[])
{
    int port = DEFAULT_PORT;
    int queue_size = DEFAULT_QUEUE;

    if (argc >= 3)
    {
        char arg_line[128] = { 0 };
        combine_arg_line(arg_line, argv, 1, argc);

        if (sscanf(arg_line, "-p %d -q %d", &port, &queue_size) < 1) {
            usage(argv[0]);
            return -1;
        }
    }
    return init_client(port, queue_size);
}

int init_client(short port, int queue_size)
{
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket <= 0)
    {
        printf("Cannot create socket\n");
        return -1;
    }

    struct sockaddr_in address = { 0 };
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_socket, (struct sockaddr*)&address, sizeof(address)) < 0) {
        printf("Cannot bind socket to port %d\n", port);
        return -2;
    }

    if (listen(server_socket, queue_size) < 0)
    {
        printf("Cannot listen socket on port %d\n", port);
        return -3;
    }

    printf("Server running on port %d\n", port);
    return process_connection();
}

int process_connection()
{
    while (1)
    {
        struct sockaddr_in client_addr = { 0 };
        int len = sizeof(client_addr);
        SOCKET client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &len);

        if (client_socket <= 0)
        {
            printf("Error incoming connection\n");
            continue;
        }

        printf("Connected from: %s\n", inet_ntoa(client_addr.sin_addr));

        // Используем 0 вместо nullptr
        CreateThread(NULL, 0, ClientHandler, (LPVOID)client_socket, 0, NULL);
    }
    return 0;
}

DWORD WINAPI ClientHandler(LPVOID lpParam)
{
    SOCKET sock = (SOCKET)lpParam;

    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char time_str[256] = { 0 };
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

    send(sock, time_str, strlen(time_str), 0);
    closesocket(sock);
    return 0;
}