# picontrol

PLC controller based on Raspberry PI.

## Contents

* `plc.c` - Main PLC server, used to collect data from sensors to cache
* `tcp_server.c` - Server, used to receive requests and forward them to PLC. Runs on TCP port.
* `cgi.c` - CGI script, which can be used as a gateway, to send requests to TCP server using HTTP. Should be compiled and installed to Web server with CGI support
* `/html` - Demo HTML site, which uses CGI script to display various data from PLC on dashboard 

## Prerequisites

* Raspbian Linux
* WiringPI C library
* Posix Threading C library
* Optional: Web server with CGI support (to access data via HTTP using CGI script)

## Installation

Run the following commands:

```bash
make
make install
```

