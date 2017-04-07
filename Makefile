# Platform settings.
ifndef OS
OS                ?= $(shell uname -s)
endif

ifeq (${OS},Darwin)
OS_BINEXT         :=
OS_CFLAGS         := -I/usr/local/include
OS_LDFLAGS        := -L/usr/local/lib
OS_LIBS           :=
endif
ifeq (${OS},Linux)
OS_BINEXT         :=
OS_CFLAGS         :=
OS_LDFLAGS        :=
OS_LIBS           :=
endif
ifeq (${OS},Windows_NT)
OS_BINEXT         := .exe
OS_CFLAGS         :=
OS_LDFLAGS        :=
OS_LIBS           :=
endif

# Build settings.
BINEXT            ?= ${OS_BINEXT}
CC                ?= clang
LIBS              ?= ${OS_LIBS}
MKDIRP            ?= mkdir -p
RM                ?= rm -f
RMDIR             ?= rm -Rf
XFLAGS            ?= -std=c99 -Wall -Wpedantic -Wextra

ifdef DEBUG
CFLAGS            ?= ${OS_CFLAGS} ${XFLAGS} -O0 -DDEBUG
LDFLAGS           ?= ${OS_LDFLAGS} -O0
OUTDIR            ?= target/debug
else
CFLAGS            ?= ${OS_CFLAGS} ${XFLAGS} -O3 -DNDEBUG
LDFLAGS           ?= ${OS_LDFLAGS} -O1
OUTDIR            ?= target/release
endif

# Dependency maps.
#
# Each C build target requires a list of the .c source files required to build
# it. Header files are resolved automatically.

CFILES_RIM        := \
	src/bin/rim/main.c \
	src/lib/arg/parse.c \

CFILES_RIMDOC     := \
	src/bin/rimdoc/main.c \
	src/lib/arg/parse.c \

CFILES_RIMFMT     := \
	src/bin/rimfmt/main.c \
	src/lib/arg/parse.c \

CFILES_TESTS      := \
	src/tests/main.unit.c \
	src/lib/arg/parse.c \
	src/lib/unit/unit.c \

# Other build variables.

objectify          = $(addprefix ${OUTDIR}/,$(subst /,-,$(1:src/%.c=%.o)))
OFILES_RIM        := $(call objectify,$(CFILES_RIM))
OFILES_RIMDOC     := $(call objectify,$(CFILES_RIMDOC))
OFILES_RIMFMT     := $(call objectify,$(CFILES_RIMFMT))
OFILES_TESTS      := $(call objectify,$(CFILES_TESTS))

# Special build commands.

default: all

all: rim rimdoc rimfmt tests doc

clean:
	$(foreach F,$(wildcard target),${RMDIR} $F;)

doc: target/doc

help:
	@echo "Available Makefile commands:"
	@echo "  make all         - Build all binaries and documentation."
	@echo "  make all DEBUG=1 - Build all in debug mode."
	@echo "  make clean       - Delete all built files."
	@echo "  make doc         - Build developer documentation."
	@echo "  make help        - Show this help message."
	@echo "  make rim         - Build rim binary."
	@echo "  make rimdoc      - Build rimdoc binary."
	@echo "  make rimfmt      - Build rimfmt binary."
	@echo "  make tests       - Build tests binary."

rim: ${OUTDIR}/rim${BINEXT}
rimdoc: ${OUTDIR}/rimdoc${BINEXT}
rimfmt: ${OUTDIR}/rimfmt${BINEXT}
tests: ${OUTDIR}/tests${BINEXT}

.PHONY: all clean default doc help rim rimdoc rimfmt tests

# Dependency file inclusion.
-include $(shell find . -iname *.d)

# Build rules for concrete targets.

target/doc: target/Doxyfile
	cd target && doxygen

target/Doxyfile: Doxyfile.sh .git/index src/lib/meta/version.h
	sh Doxyfile.sh

src/lib/meta/version.h: .git/index
	cd $(dir $@) && sh version.sh

${OUTDIR}/rim${BINEXT}: src/lib/meta/version.h ${OFILES_RIM}
${OUTDIR}/rimdoc${BINEXT}: src/lib/meta/version.h ${OFILES_RIMDOC}
${OUTDIR}/rimfmt${BINEXT}: src/lib/meta/version.h ${OFILES_RIMFMT}
${OUTDIR}/tests${BINEXT}: src/lib/meta/version.h ${OFILES_TESTS}

# Build rules for general file types.
.SUFFIXES:

%.o:
	@${MKDIRP} $(dir $@)
	${CC} ${CFLAGS} -c -MMD src/$(subst -,/,$(notdir $*)).c -o $@

%${BINEXT}:
	@${MKDIRP} $(dir $@)
	${CC} ${LDFLAGS} ${LIBS} -o $@ $(filter %.o,$^)
