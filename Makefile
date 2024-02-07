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
    SLASH := /
    EXT := 
else ifeq ($(OS),Windows)
    MAKE := mingw32-make
    RM := del /Q /S
    MKDIR := mkdir
    MV := move
	TARGET := -G "MinGW Makefiles"
    SLASH := \\
    EXT := .exe
else
    $(error Unsupported operating system: $(OS))
endif

# Compiler options (GCC)
CC := gcc

.PHONY: all clean make

all: bin

make: build
	$(MAKE) -C $<

build:
	cmake -B $@ -S . $(TARGET)

bin: make
	if not exist unit_tests$(SLASH)$@ $(MKDIR) unit_tests$(SLASH)$@
	$(MV) build$(SLASH)unit_tests$(SLASH)UTDecoder$(EXT) unit_tests$(SLASH)$@
	$(MV) build$(SLASH)unit_tests$(SLASH)UTProjectFF$(EXT) unit_tests$(SLASH)$@

clean:
	$(RM) unit_tests$(SLASH)bin
	$(RM) build
