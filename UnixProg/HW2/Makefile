CC = c++ -std=c++11
CPPFLAG = \
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

TARGET = ps
DIR = Process
OBJ = $(DIR)/process.o $(TARGET).o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CPPFLAG) $^ -o $@

%.o: %.cpp
	$(CC) $(CPPFLAG) -c $^ -o $@

clean:
	rm -f $(OBJ) $(TARGET)
