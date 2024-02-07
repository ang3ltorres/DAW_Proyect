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

all: bin
.PHONY: all

make: build
	$(MAKE) -C $<
.PHONY: make

build:
	cmake -B $@ -S . $(TARGET)

bin: make
	$(if $(wildcard unit_tests$(SLASH)$@),,$(MKDIR) unit_tests$(SLASH)$@)

	$(MV) build$(SLASH)unit_tests$(SLASH)UTDecoder$(EXT) unit_tests$(SLASH)$@
	$(MV) build$(SLASH)unit_tests$(SLASH)UTProjectFF$(EXT) unit_tests$(SLASH)$@

run_decoder:
	$(if $(wildcard unit_tests$(SLASH)bin), \
    @.$(SLASH)unit_tests$(SLASH)bin$(SLASH)UTDecoder$(EXT), \
    @echo "Compile bin first")
.PHONY: run_decoder

clean:
	$(RM) unit_tests$(SLASH)bin
	$(RM) build
.PHONY: clean