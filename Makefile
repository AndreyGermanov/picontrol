all: plc tcp_server cgi
plc: plc.c
	gcc -o plc plc.c -lwiringPi -lpthread
tcp_server: tcp_server.c
	gcc -o tcp_server tcp_server.c -lpthread
cgi: cgi.c
	gcc -o picontrol.cgi cgi.c
install:
	cp -af picontrol.cgi /usr/lib/cgi-bin/
	cp -af html/* /var/www/html/
clean:
	rm plc tcp_server picontrol.cgi

