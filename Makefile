
#DLLWRAP=i386-mingw32msvc-dllwrap

default:
	@+make -C build

dll:
	@+make -C build dll

axis:
	@+make -C build axis

.PHONY: samples
samples:
	@+make -C build samples

clean:
	@rm -f build/*.o *.so *.exe *.dll
	@rm -f jpeg2avi axis2avi test_audio readavi
	@rm -f axis2avi
	@echo "Clean!"

