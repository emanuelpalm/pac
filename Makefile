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
MKDIR             ?= mkdir
RM                ?= rm
XFLAGS            ?= -std=c11 -Wall -Wpedantic -Wextra

ifdef DEBUG
CFLAGS            ?= ${OS_CFLAGS} ${XFLAGS} -O0 -DDEBUG
LDFLAGS           ?= ${OS_LDFLAGS} -O0
else
CFLAGS            ?= ${OS_CFLAGS} ${XFLAGS} -O3 -DNDEBUG
LDFLAGS           ?= ${OS_LDFLAGS} -O1
endif

.PHONY: all clean default help

default: all

all: pac${BINEXT} pacdoc${BINEXT} pacfmt${BINEXT} tests${BINEXT}

clean:
	$(foreach F,$(shell find . -iname *.[do] -type f),${RM} $F ${\n})
	$(foreach F,$(shell find . -maxdepth 1 -type f -executable),${RM} $F ${\n})

help:
	@echo "Available Makefile commands:"
	@echo "  make all         - Build all binaries."
	@echo "  make all DEBUG=1 - Build all binaries in debug mode."
	@echo "  make clean       - Delete all built files."
	@echo "  make help        - Show this help message."

define \n


endef

-include $(shell find . -iname *.d)

pac${BINEXT}: \
		source/binaries/pac/main.o \
		source/library/arg/parse.o

pacdoc${BINEXT}: \
		source/binaries/pacdoc/main.o \
		source/library/arg/parse.o

pacfmt${BINEXT}: \
		source/binaries/pacfmt/main.o \
		source/library/arg/parse.o

tests${BINEXT}: \
		source/tests/main.unit.o \
		source/library/arg/parse.o \
		source/library/unit/unit.o

${OUTDIR}/:
	${MKDIR} $@

.SUFFIXES:

%.o: ${OUTDIR}/
	${CC} ${CFLAGS} -c -MMD $*.c -o $@

%${BINEXT}:
	${CC} ${LDFLAGS} ${LIBS} -o $@ $^
