CC?=gcc
#DEBUG=-DDEBUG
CFLAGS=-Wall -O3 $(DEBUG)
VPATH=../src

OBJECTS=fileio.o

default: $(OBJECTS)
	$(CC) -o ../libkohn_avi.so ../src/kohn_avi.c $(OBJECTS) -shared $(CFLAGS)
	$(CC) -o ../jpeg2avi ../src/jpeg2avi.c $(CFLAGS) -L.. -lkohn_avi
	$(CC) -o ../readavi ../src/readavi.c $(CFLAGS) -L.. -lkohn_avi

dll: $(OBJECTS)
	$(CC) -c ../src/kohn_avi.c $(CFLAGS)
	$(DLLWRAP) --def ../src/kohn_avi.def --dllname kohn_avi \
   	-o ../kohn_avi.dll fileio.o kohn_avi.o
	$(CC) -o ../jpeg2avi.exe jpeg2avi.c $(CFLAGS) -L. -lkohn_avi

samples:
	$(CC) -o ../test_audio ../src/test_audio.c $(CFLAGS) -L.. -lkohn_avi

%.o: %.cxx %.h
	$(CC) -c $< -o $*.o \
	  $(CFLAGS)

