# $@ $^ - left and right side of the :
# $< - first item in the dependencies list
# %.o - rule that applies to all files ending with the .o suffix

CC=gcc
CFLAGS=-std=c99 -O2 -I. -Wall -Wextra
LFLAGS=-mwindows
HEADERS=keyboard_state.h
OBJS=main.o keyboard_state.o ini.o resource.o

bothhands.exe : $(OBJS)
	$(CC) $(LFLAGS) -o $@ $^

resource.o : resource.rc resource.h
	windres $< -o $@

ini.o : inih/ini.c inih/ini.h
	$(CC) $(CFLAGS) -c -o $@ $<

%.o : %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean :
	rm -rf *.o
