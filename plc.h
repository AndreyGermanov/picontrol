#ifndef PICONTROL_PLC_H
#define PICONTROL_PLC_H

float get_temperature();
float get_humidity();
void* process_client_request(void*);
void* run_socket_server(void*);
void daemonize();

#endif
