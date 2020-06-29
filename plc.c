/**
 * PLC controller for Raspberry PI.
 *
 * Used to monitor sensors connected to PI and run commands to change system state
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <zconf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/un.h>
#include "plc.h"

float temperature = 0;
float humidity = 0;

/**
 * Main daemon function. Starts server which constantly reads temperature and humidity and writes
 * these values to /tmp/temperature and /tmp/humidity files
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
        pthread_t thread;
        pthread_create(&thread,NULL,run_socket_server,NULL);
        printf("PLC is running. PID: %d\n",getpid());
        while (1) {
            temperature = get_temperature();
            humidity = get_humidity();
            usleep(5*1000000);
        }
    }
}

/**
 * Function used to run Unix domain socket server in background thread to receive requests
 * from external clients
 * @return
 */
void* run_socket_server(void *arg) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    int cs;
    struct sockaddr_un addr;
    remove("/tmp/plc");
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "/tmp/plc", sizeof(addr.sun_path)-1);
    bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(fd, 500);
    while (1) {
        cs = accept(fd, NULL, NULL);
        pthread_t thread;
        int result = pthread_create(&thread,NULL,process_client_request,&cs);
    }
}

/**
 * Function used to process client request
 * @param arg - Client socket to receive request and send response to
 */
void* process_client_request(void *arg) {
    int *client_socket = (int*)arg;
    char cmd[BUFSIZ];
    read(*client_socket, cmd, BUFSIZ);
    if (strcmp(cmd, "temperature") == 0) {
        char buf[100];
        sprintf(buf,"%.2f", temperature);
        write(*client_socket, buf, sizeof(buf));
    } else if (strcmp(cmd, "humidity") == 0) {
        char buf[100];
        sprintf(buf,"%.2f", humidity);
        write(*client_socket, buf, sizeof(buf));
    }
    close(*client_socket);
    return NULL;
}

/**
 * Function used to read CPU temperature from internal PI sensor
 * @return float CPU temperature
 */
float get_temperature() {
    FILE *fp = fopen("/sys/class/thermal/thermal_zone0/temp","r");
    if (fp == NULL) { return 0; }
    float temp;
    fscanf(fp,"%f",&temp);
    fclose(fp);
    return temp/1000;
}

/**
 * Function used to read environmental humidity from external sensor
 * @return float Humidity
 */
float get_humidity() {
    #define MAX_TIMINGS	85
    #define DHT_PIN		4
    wiringPiSetupGpio();
    int data[5] = { 0, 0, 0, 0, 0 };
    uint8_t laststate	= HIGH;
    uint8_t counter		= 0;
    uint8_t j			= 0, i;

    data[0] = data[1] = data[2] = data[3] = data[4] = 0;
    pinMode( DHT_PIN, OUTPUT );
    digitalWrite( DHT_PIN, LOW );
    delay( 18 );

    pinMode( DHT_PIN, INPUT );

    for ( i = 0; i < MAX_TIMINGS; i++ )
    {
        counter = 0;
        while ( digitalRead( DHT_PIN ) == laststate )
        {
            counter++;
            delayMicroseconds( 1 );
            if ( counter == 255 )
            {
                break;
            }
        }
        laststate = digitalRead( DHT_PIN );

        if ( counter == 255 )
            break;

        if ( (i >= 4) && (i % 2 == 0) )
        {
            data[j / 8] <<= 1;
            if ( counter > 16 )
                data[j / 8] |= 1;
            j++;
        }
    }

    if ( (j >= 40) &&
         (data[4] == ( (data[0] + data[1] + data[2] + data[3]) & 0xFF) ) )
    {
        float h = (float)((data[0] << 8) + data[1]) / 10;
        if ( h > 100 )
        {
            h = data[0];	// for DHT11
        }
        float c = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
        if ( c > 125 )
        {
            c = data[2];	// for DHT11
        }
        if ( data[2] & 0x80 )
        {
            c = -c;
        }
        float f = c * 1.8f + 32;
        return h;
    }else  {
        return 0.0;
    }
}