/**
 * CGI interface for TCP server. Allows to receive data from PLC controller by HTTP protocol using Web Server.
 * Script can be installed on Web server and executed with following query strings:
 *
 * picontrol.cgi?temperature - returns current CPU temperature
 * picontrol.cgi?humidity - returns current humidity of environment
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

void send_tcp_command(const char* cmd, char* result);

/**
 * Main function which implements CGI protocol: receives HTTP request, and sends response to stdout.
 * @return
 */
int main() {
    if (getenv("QUERY_STRING") == NULL) {
        printf("This app should run by HTTP server\n");
        return 0;
    }
    char *query_string = getenv("QUERY_STRING");
    printf("Content-type: text/html\n\n");
    if (strcmp(query_string,"temperature") == 0) {
        char result[100];
        send_tcp_command("temperature",result);
        printf("%s",result);
    } else if (strcmp(query_string,"humidity") == 0) {
        char result[100];
        send_tcp_command("humidity",result);
        printf("%s",result);
    } else {
        printf("\n");
    }
    return 0;
}

/**
 * Process HTTP command, passed via query string. (Sends command to PLC via TCP server process)
 * @param cmd - Command to process
 * @param result - Result of command execution as a string
 */
void send_tcp_command(const char* cmd, char* result) {
    struct sockaddr_in local;
    int s = socket(AF_INET,SOCK_STREAM,0);
    inet_aton("0.0.0.0",&local.sin_addr);
    local.sin_port = htons(9000);
    local.sin_family = AF_INET;
    connect(s,(struct sockaddr*)&local,sizeof(local));
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