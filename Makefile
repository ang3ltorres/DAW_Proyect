# Detect the operating system
ifdef SystemRoot
    # Windows
    OS := Windows
else
    # Linux
    OS := $(shell uname)
endif

# Rule for Linux and Windows
ifeq ($(OS),Linux)
    MAKE := make
    RM := rm -rf
    MKDIR := mkdir -p
    MV := mv
	TARGET :=
else ifeq ($(OS),Windows)
    MAKE := mingw32-make
    RM := del /Q /S
    MKDIR := mkdir
    MV := move
	TARGET := -G "MinGW-Makefiles"
else
    $(error Unsupported operating system: $(OS))
endif

# Compiler options (GCC)
CC := gcc

.PHONY: all clean make

all: bin

make: build
	$(MAKE) -C $<

build: CMakeLists.txt
	$(MKDIR) $@
	cmake -B $@ -S . $(TARGET)

bin: make
	$(MKDIR) unit_tests/$@
	$(MV) build/unit_tests/UTDecoder unit_tests/$@
	$(MV) build/unit_tests/UTProjectFF unit_tests/$@

clean:
	$(RM) unit_tests/bin
	$(RM) build
