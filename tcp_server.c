/**
 * TCP server which starts on port 9000 and listens for commands. When received the commands,
 * server requests data from internal PLC controller process and sends result to client
 */
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/un.h>
#include "tcp_server.h"

/**
 * Starts main tcp server process, receives commands from clients
 * and sends results
 * @return
 */
int main() {
    int pid = fork();
    if (pid == 0) {

        chdir("/");
        setsid();
        close(stdin);
        close(stdout);
        close(stderr);
        struct sockaddr_in local;
        int ss = socket(AF_INET, SOCK_STREAM, 0);
        int cs;
        inet_aton("0.0.0.0", &local.sin_addr);
        local.sin_port = htons(9000);
        local.sin_family = AF_INET;
        bind(ss, (struct sockaddr *) &local, sizeof(local));
        listen(ss, 5000);
        printf("Server is listening on port 9000\n");
        while (1) {
            cs = accept(ss, NULL, NULL);
            pthread_t thread;
            int result = pthread_create(&thread,NULL,process_client_request,&cs);
        }
    }
    return 0;
}

/**
 * Function used to process client request
 * @param client_socket - Client socket to receive request and send response to
 */
void* process_client_request(void *arg) {
    int *client_socket = (int*)arg;
    char cmd[BUFSIZ];
    read(*client_socket, cmd, BUFSIZ);
    if (strcmp(cmd, "temperature") == 0) {
        char temperature[100];
        send_plc_command("temperature",temperature);
        write(*client_socket, temperature, sizeof(temperature));
    } else if (strcmp(cmd, "humidity") == 0) {
        char humidity[100];
        send_plc_command("humidity",humidity);
        write(*client_socket, humidity, sizeof(humidity));
    }
    close(*client_socket);
    return NULL;
}

/**
 * Sends specified command to PLC daemon via unix domain socket
 * @param cmd - Command to process
 * @param result - Result of command execution as a string
 */
void send_plc_command(const char* cmd, char* result) {
    int s = socket(AF_UNIX,SOCK_STREAM,0);
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "/tmp/plc", sizeof(addr.sun_path)-1);
    connect(s,(struct sockaddr*)&addr,sizeof(addr));
    char buf[BUFSIZ];
    write(s,cmd,strlen(cmd)+1);
    while (1) {
        int cnt = read(s,result,sizeof(buf)+1);
        if (cnt>0) {
            break;
        }
    }
    close(s);
}