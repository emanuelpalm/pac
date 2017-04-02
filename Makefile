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
XFLAGS            ?= -std=c11 -Wall -Wpedantic -Wextra

ifdef DEBUG
CFLAGS            ?= ${OS_CFLAGS} ${XFLAGS} -O0 -DDEBUG
LDFLAGS           ?= ${OS_LDFLAGS} -O0
OUTDIR            ?= target/debug
else
CFLAGS            ?= ${OS_CFLAGS} ${XFLAGS} -O3 -DNDEBUG
LDFLAGS           ?= ${OS_LDFLAGS} -O1
OUTDIR            ?= target/release
endif

# Source files generated from scripts.
GENFILES          ?= \
	src/lib/meta/git.gen \

# Special build commands.

default: all

all: pac pacdoc pacfmt tests doc

clean:
	$(foreach F,$(shell find . -iname *.gen -type f),${RM} $F;)
	$(foreach F,$(wildcard Doxyfile),${RM} $F;)
	$(foreach F,$(wildcard target),${RMDIR} $F;)

doc: target/doc

help:
	@echo "Available Makefile commands:"
	@echo "  make all         - Build all binaries."
	@echo "  make all DEBUG=1 - Build all binaries in debug mode."
	@echo "  make clean       - Delete all built and generated files."
	@echo "  make doc         - Generate developer documentation."
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

# Dependency maps for primary compile targets.
#
# Most dependencies are resolved automatically via the GCC/clang -MMD command,
# which means that just *.o files need to be listed as required by each binary
# target.

Doxyfile: Doxyfile.sh .git/index
	sh Doxyfile.sh

target/doc: target Doxyfile
	doxygen

src/lib/meta/git.gen: .git/index

${OUTDIR}/pac${BINEXT}: ${GENFILES} \
		${OUTDIR}/bin-pac-main.o \
		${OUTDIR}/lib-arg-parse.o \

${OUTDIR}/pacdoc${BINEXT}: ${GENFILES} \
		${OUTDIR}/bin-pacdoc-main.o \
		${OUTDIR}/lib-arg-parse.o \

${OUTDIR}/pacfmt${BINEXT}: ${GENFILES} \
		${OUTDIR}/bin-pacfmt-main.o \
		${OUTDIR}/lib-arg-parse.o \

${OUTDIR}/tests${BINEXT}: ${GENFILES} \
		${OUTDIR}/tests-main.unit.o \
		${OUTDIR}/lib-arg-parse.o \
		${OUTDIR}/lib-unit-unit.o \

# General compile rules.
.SUFFIXES:

%.gen:
	cd $(dir $@) && sh $(notdir $@).sh

%.o:
	@${MKDIRP} $(dir $@)
	${CC} ${CFLAGS} -c -MMD src/$(subst -,/,$(notdir $*)).c -o $@

%${BINEXT}:
	@${MKDIRP} $(dir $@)
	${CC} ${LDFLAGS} ${LIBS} -o $@ $(filter %.o,$^)
