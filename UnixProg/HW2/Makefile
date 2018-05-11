CC = c++ -std=c++11 -O3
TARGET = ps
DIR = Process
OBJ = $(DIR)/process.o $(TARGET).o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $^ -o $@

%.o: %.cpp
	$(CC) -c $^ -o $@

clean:
	rm $(OBJ) $(TARGET)
