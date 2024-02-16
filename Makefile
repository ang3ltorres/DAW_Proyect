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

compile: build
	$(MAKE) -C $<
.PHONY: compile

build:
	cmake -B $@ -S . $(TARGET)

lib:
    export LD_LIBRARY_PATH=.$(SLASH)$@$(SLASH)

bin: compile
	@echo "Setting bin"
	@$(if $(wildcard unit_tests$(SLASH)$@),,$(MKDIR) unit_tests$(SLASH)$@)

	@$(MV) build$(SLASH)unit_tests$(SLASH)UT*$(EXT) unit_tests$(SLASH)$@

compile_UT: build
	$(MAKE) -C $</unit_tests
	@$(MAKE) bin
.PHONY: compile_UT

run_decoder: lib

	$(if $(wildcard unit_tests$(SLASH)bin), \
    @.$(SLASH)unit_tests$(SLASH)bin$(SLASH)UTDecoder$(EXT), \
    @echo "Compile bin first")
.PHONY: run_decoder

UTportaudioex: lib

	$(if $(wildcard unit_tests$(SLASH)bin), \
    @.$(SLASH)unit_tests$(SLASH)bin$(SLASH)UTportaudioex$(EXT), \
    @echo "Compile bin first")
.PHONY: UTportaudioex

clean:
	$(RM) unit_tests$(SLASH)bin
	$(RM) build
.PHONY: clean