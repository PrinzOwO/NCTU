CC = g++
#CXXFLAGS = -g3 -Wall -Wextra -pedantic -Wshadow -std=c++14
CXXFLAGS = -std=c++14 \
	-pthread \
        -Os -s \
        -ffunction-sections \
        -fdata-sections \
        -Wl,--gc-sections \
        -fno-exceptions \
        -fno-rtti \
        -fno-stack-protector \
        -fno-unwind-tables -fno-asynchronous-unwind-tables \
        -fno-math-errno \
        -fno-strict-aliasing \
        -fmerge-all-constants \
        -Wl,-z,norelro \
        -W -Wall \
        -mfpmath=387 -mfancy-math-387 \
        -Wl,--build-id=none  \
        -fno-unroll-loops \
        -fno-ident \
        -fomit-frame-pointer

TARGET = lanchat
OBJ = lan.o $(TARGET).o

all: $(TARGET)

clean: 
	rm -f *.o $(TARGET)

test:
	./$(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(CXXFLAGS) -c $^
