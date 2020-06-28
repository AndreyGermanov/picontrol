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

#define MAX_TIMINGS	85
#define DHT_PIN		4

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
float get_humidity()
{
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

/**
 * Main daemon function. Starts server which constantly reads temperature and humidity and writes
 * these values to /tmp/temperature and /tmp/humidity files
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
        float prev_temperature = 0;
        float prev_humidity = 0;
        printf("PLC is running. PID: %d\n",getpid());
        while (1) {
            float temperature = get_temperature();
            FILE *tempChan = fopen("/tmp/temperature","w");
            fprintf(tempChan,"%.2f\n",temperature);
            prev_temperature = temperature;
            fclose(tempChan);
            float humidity = get_humidity();
            FILE *humidityChan = fopen("/tmp/humidity","w");
            fprintf(humidityChan,"%.2f\n",humidity);
            prev_humidity = humidity;
            fclose(humidityChan);
            usleep(5*1000000);
        }
    }
}