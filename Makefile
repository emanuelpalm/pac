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

CFILES_PAC        := \
	src/bin/pac/main.c \
	src/lib/arg/parse.c \

CFILES_PACDOC     := \
	src/bin/pacdoc/main.c \
	src/lib/arg/parse.c \

CFILES_PACFMT     := \
	src/bin/pacfmt/main.c \
	src/lib/arg/parse.c \

CFILES_TESTS      := \
	src/tests/main.unit.c \
	src/lib/arg/parse.c \
	src/lib/unit/unit.c \

# Other build variables.

objectify          = $(addprefix ${OUTDIR}/,$(subst /,-,$(1:src/%.c=%.o)))
OFILES_PAC        := $(call objectify,$(CFILES_PAC))
OFILES_PACDOC     := $(call objectify,$(CFILES_PACDOC))
OFILES_PACFMT     := $(call objectify,$(CFILES_PACFMT))
OFILES_TESTS      := $(call objectify,$(CFILES_TESTS))

# Special build commands.

default: all

all: pac pacdoc pacfmt tests doc

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
	@echo "  make pac         - Build pac binary."
	@echo "  make pacdoc      - Build pacdoc binary."
	@echo "  make pacfmt      - Build pacfmt binary."
	@echo "  make tests       - Build tests binary."

pac: ${OUTDIR}/pac${BINEXT}
pacdoc: ${OUTDIR}/pacdoc${BINEXT}
pacfmt: ${OUTDIR}/pacfmt${BINEXT}
tests: ${OUTDIR}/tests${BINEXT}

.PHONY: all clean default doc help pac pacdoc pacfmt tests

# Dependency file inclusion.
-include $(shell find . -iname *.d)

# Build rules for concrete targets.

target/doc: target/Doxyfile
	cd target && doxygen

target/Doxyfile: Doxyfile.sh .git/index src/lib/meta/version.h
	sh Doxyfile.sh

src/lib/meta/version.h: .git/index
	cd $(dir $@) && sh version.sh

${OUTDIR}/pac${BINEXT}: src/lib/meta/version.h ${OFILES_PAC}
${OUTDIR}/pacdoc${BINEXT}: src/lib/meta/version.h ${OFILES_PACDOC}
${OUTDIR}/pacfmt${BINEXT}: src/lib/meta/version.h ${OFILES_PACFMT}
${OUTDIR}/tests${BINEXT}: src/lib/meta/version.h ${OFILES_TESTS}

# Build rules for general file types.
.SUFFIXES:

%.o:
	@${MKDIRP} $(dir $@)
	${CC} ${CFLAGS} -c -MMD src/$(subst -,/,$(notdir $*)).c -o $@

%${BINEXT}:
	@${MKDIRP} $(dir $@)
	${CC} ${LDFLAGS} ${LIBS} -o $@ $(filter %.o,$^)
