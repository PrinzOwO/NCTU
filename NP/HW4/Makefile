CC=g++
FLAG=-Wall -Wextra -Wpedantic -g3 -O2
TARGET=socksserver
OBJ=error.o server.o socks4.o socksserver.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(FLAG) $^ -o $@

socks4.o: socks4.cpp
	$(CC) -c $(FLAG) $^ -o $@

socksserver.o: socksserver.cpp
	$(CC) -c $(FLAG) $^ -o $@

error.o: error.cpp
	$(CC) -c $(FLAG) $^ -o $@

server.o: server.cpp
	$(CC) -c $(FLAG) $^ -o $@

clean:
	rm -f $(TARGET) $(OBJ)
