TARGET = single
OBJ = error.o chat.o shell.o server.o single.o
FLG = -std=c++14 -Wall -Wextra -g3

all: $(TARGET)
$(TARGET): $(OBJ)
	g++ $(FLG) $^ -o $@

single.o: single.cpp
	g++ -c $(FLG) $^ -o $@

chat.o: chat.cpp
	g++ -c $(FLG) $^ -o $@

server.o: server.cpp
	g++ -c $(FLG) $^ -o $@

error.o: error.cpp
	g++ -c $(FLG) $^ -o $@

shell.o: shell.cpp
	g++ -c $(FLG) $^ -o $@

clean:
	rm -f $(OBJ) $(TARGET)
