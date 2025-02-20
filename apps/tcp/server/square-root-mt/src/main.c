#include "main.h"
#include <windows.h>
#include <process.h>

SOCKET server_socket = -1;

void free_socket()
{
    if (server_socket > 0)
    {
        closesocket(server_socket);
    }
}

void usage(const char* exe_name)
{
    printf("Usage:\n");
    printf("\t%s -p <port> -q <que_size>\n", exe_name);
}

int start(int argc, char* argv[])
{
    int port = DEFAULT_PORT;
    int queue_size = DEFAULT_QUEUE;

    if (argc >= 3)
    {
        char arg_line[128] = { 0 };
        combine_arg_line(arg_line, argv, 1, argc);
        int ret = sscanf(arg_line, "-p %d -q %d", &port, &queue_size);
        if (ret < 1) {
            usage(argv[0]);
            return -1;
        }
    }

    return init_server(port, queue_size);
}

int init_server(short port, int queue_size)
{
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket <= 0)
    {
        printf("Cannot create socket\n");
        return -1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_socket, (struct sockaddr*)&address, sizeof(address))) {
        printf("Cannot bind socket to port %d\n", port);
        return -2;
    }

    if (listen(server_socket, queue_size))
    {
        printf("Cannot listen socket on port %d\n", port);
        return -3;
    }

    printf("Server running on port %d\n", port);
    return process_connections();
}

int process_connections()
{
    SOCKET client_socket = -1;
    while (1)
    {
        struct sockaddr_in client_addr;
        int len = sizeof(client_addr);

        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &len);
        if (client_socket <= 0)
        {
            printf("Error accepting connection\n");
            continue;
        }

        printf("New connection from: %s\n", inet_ntoa(client_addr.sin_addr));

        _beginthread(process_connection, 0, (void*)(intptr_t)client_socket);
    }

    return 0;
}

void process_connection(void* arg)
{
    SOCKET client_socket = (SOCKET)(intptr_t)arg;

    if (client_socket <= 0)
    {
        printf("Invalid client socket\n");
        return;
    }

    char buffer[1024] = { 0 };
    int ret = recv(client_socket, buffer, sizeof(buffer), 0);

    if (ret <= 0)
    {
        printf("Receiving data error\n");
        closesocket(client_socket);
        return;
    }

    printf("<==== Received program name: %s\n", buffer);

    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(si);

    if (CreateProcess(NULL, buffer, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("Program '%s' started successfully\n", buffer);
        strcpy(buffer, "Program started successfully.");
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        printf("Failed to start program '%s'\n", buffer);
        strcpy(buffer, "Error: Program not found or failed to start.");
    }

    ret = send(client_socket, buffer, strlen(buffer), 0);
    if (ret <= 0)
    {
        printf("Sending response error\n");
    }

    closesocket(client_socket);
}
