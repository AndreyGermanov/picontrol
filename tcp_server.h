#ifndef PICONTROL_TCP_SERVER_H
#define PICONTROL_TCP_SERVER_H

void send_plc_command(const char* cmd, char* result);
void* process_client_request(void *arg);
void daemonize();

#endif
