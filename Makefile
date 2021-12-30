
#DLLWRAP=i386-mingw32msvc-dllwrap

default:
	@+make -C build

dll:
	@+make -C build dll

axis:
	@+make -C build axis

test:
	$(CC) -o test test.c $(CFLAGS) -L. -lkohn_avi

clean:
	@rm -f build/*.o *.so *.exe *.dll
	@rm -f jpeg2avi axis2avi test_audio
	@rm -f axis2avi
	@echo "Clean!"

