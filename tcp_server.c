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

/**
 * Function used to process client request
 * @param client_socket - Client socket to receive request and send response to
 */
void process_client_request(int *client_socket) {
    char cmd[BUFSIZ];
    read(*client_socket, cmd, BUFSIZ);
    if (strcmp(cmd, "temperature") == 0) {
        FILE *fp = fopen("/tmp/temperature", "r");
        char temperature[100];
        fscanf(fp, "%s", temperature);
        write(*client_socket, temperature, sizeof(temperature));
    } else if (strcmp(cmd, "humidity") == 0) {
        FILE *fp = fopen("/tmp/humidity", "r");
        char humidity[100];
        fscanf(fp, "%s", humidity);
        write(*client_socket, humidity, sizeof(humidity));
    }
    close(*client_socket);
}

/**
 * Starts main tcp server process, receives commands from clients
 * and sends results
 * @return
 */
int main() {
    int pid = fork();
    if (pid != 0) {
    } else {
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
            process_client_request(&cs);
        }
    }
    return 0;
}
