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

$(TEST)0: $(LIB).o start.o $(TEST)0.o
	gcc $(CXXFLAGS) -o $@ $^

$(TEST)0s: $(LIB).so start.o $(TEST)0.o  
	gcc $(CXXFLAGS) -o $@ $^

$(TEST)1: $(LIB).o $(LIB).so start.o $(TEST)1.o
	gcc $(CXXFLAGS) -o $@ $^

$(TEST)1s: $(LIB).so start.o $(TEST)1.o  
	gcc $(CXXFLAGS) -o $@ $^

$(TEST)2: $(LIB).o $(LIB).so start.o $(TEST)2.o
	gcc $(CXXFLAGS) -o $@ $^

$(TEST)2s: $(LIB).so start.o $(TEST)2.o  
	gcc $(CXXFLAGS) -o $@ $^


$(TEST): $(LIB).o $(LIB).so start.o $(TEST).o
	gcc $(CXXFLAGS) -o $@ $^

$(TEST)s: $(LIB).so start.o $(TEST).o   
	gcc $(CXXFLAGS) -o $@ $^

#########################################################
#                        TEST.o                         #
#########################################################

$(TEST)0.o: $(TEST)0.c
	gcc $(CCFLAGS) $<

$(TEST)1.o: $(TEST)1.c
	gcc $(CCFLAGS) $<

$(TEST)2.o: $(TEST)2.c
	gcc $(CCFLAGS) $<

$(TEST).o: $(TEST).c
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
