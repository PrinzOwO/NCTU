CC = g++ -std=c++11
TARGET = shell
DIR = Command
OBJ = $(DIR)/command.o secureInteractiveShell.o

all: $(TARGET)
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) -static

%.o: %.cpp
	$(CC) -c $^ -o $@

.PHONY: install clean
install: 
	cp shell /home/user/tmproot/bin
clean:
	rm -f $(OBJ) $(TARGET)
