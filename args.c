/**
 * Command line arguments parser. Used to parse command line arguments and provide interface to get these values
 */
#include <unistd.h>
#include <stdlib.h>
#include "args.h"

// Should server run in background as a daemon
int isDaemon = 0;
// Port number to run TCP server
int portNumber = 9000;
// Path to Unix Domain socket to communicate with PLC server
char* unixSocketPath = "/tmp/plc";

/**
 * Function used to parse command line arguments and populate global variables, defined above
 * @param argc - Number of command line arguments
 * @param argv - Command line arguments array
 */
void parseArgs(int argc, char* argv[]) {
    char* optString = "dp::u::";
    int opt = getopt(argc,argv,optString);
    while (opt != -1) {
        switch (opt) {
            case 'd':
                isDaemon = 1;
                break;
            case 'p': ;
                if (optarg != NULL && strlen(optarg)>0) {
                    int v = atoi(optarg);
                    if (v > 0) {
                        portNumber = v;
                    }
                }
                break;
            case 'u':
                if (optarg != NULL && strlen(optarg)>0) {
                    unixSocketPath = optarg;
                }
                break;
        }
        opt = getopt(argc,argv,optString);
    }
}

/**
 * Returns "daemon" mode flag. If true, then application will run in background as a daemon
 * @return 1 or 0
 */
int is_daemon() {
    return isDaemon;
}

/**
 * Returns port number for TCP server or for TCP client
 * @return
 */
int get_port_number() {
    return portNumber;
}

/**
 * Returns Unix Domain Socket path for PLC server
 * @return
 */
char* get_unix_socket_path() {
    return unixSocketPath;
}