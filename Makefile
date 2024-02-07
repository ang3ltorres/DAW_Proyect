# NOTE: This only works for LINUX
.PHONY: all clean make

all: bin

make: build
	$@ -C $<

build: CMakeLists.txt
    ifeq ($(shell uname),Linux)
		@cmake -B $@ -S .
    else
        @echo "This Makefile only works in a Linux env"
    endif


bin: make
	@mkdir -p unit_tests/$@
	@mv build/unit_tests/UTDecoder unit_tests/$@
	@mv build/unit_tests/UTProjectFF unit_tests/$@


clean:
	@rm -rf unit_tests/bin
	@rm -rf build