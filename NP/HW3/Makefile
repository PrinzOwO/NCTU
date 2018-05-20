CC = g++
FLAG = -Wall -g3 -O2
TARGET = httpd cgiclient
HTTPOBJ = error.o server.o httpd.o
CGIOBJ = error.o cgi.o cgiclient.o

all: $(TARGET)

httpd: $(HTTPOBJ)
	$(CC) $^ $(FLAG) -o $@

cgiclient: $(CGIOBJ)
	$(CC) $^ $(FLAG) -o $@

error.o: error.cpp
	$(CC) $< $(FLAG) -c -o $@

server.o: server.cpp
	$(CC) $< $(FLAG) -c -o $@

httpd.o: httpd.cpp
	$(CC) $< $(FLAG) -c -o $@

cgi.o: cgi.cpp
	$(CC) $< $(FLAG) -c -o $@

cgiclient.o: cgiclient.cpp
	$(CC) $< $(FLAG) -c -o $@

clean:
	rm -f $(HTTPOBJ) $(CGIOBJ) $(TARGET)
