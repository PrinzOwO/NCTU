TEST = test
TARGET = $(TEST)0  $(TEST)1  $(TEST)2  $(TEST)3 \
	 $(TEST)0s $(TEST)1s $(TEST)2s $(TEST)3s
LIB = libminic
ASMFLAGS = -f elf64
CCFLAGS = -c -g -Wall -fno-stack-protector
CXXFLAGS = -nostdlib -fno-pie -no-pie

.PHONY: clean

all: $(TEST) $(TEST)s

clean:
	rm -f *.o *.so $(TARGET) $(TEST) $(TEST)s
	export LD_LIBRARY_PATH=

#########################################################
#                         TEST                          #
#########################################################

$(TEST): $(LIB).o $(LIB).so start.o $(TEST).o
	gcc $(CXXFLAGS) -o $@ $^

$(TEST)s: $(LIB).so start.o $(TEST).o   
	gcc $(CXXFLAGS) -o $@ $^

#########################################################
#                        TEST.o                         #
#########################################################

%.o: %.c
	export LD_LIBRARY_PATH=.
	gcc $(CCFLAGS) $<

#########################################################
#                          ASM                          #
#########################################################

$(LIB).o: $(LIB).asm
	nasm $(ASMFLAGS) -DPIC $< -o $@

start.o: start.asm
	nasm $(ASMFLAGS) $< -o $@

$(LIB).so: $(LIB).o
	ld -shared $< -o $@
