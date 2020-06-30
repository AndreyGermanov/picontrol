#ifndef PICONTROL_ARGS_H
#define PICONTROL_ARGS_H

void parseArgs(int argc, char* argv[]);
int is_daemon();
int get_port_number();
char* get_unix_socket_path();

#endif
