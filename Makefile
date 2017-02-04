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

# Source files generated from scripts.
GENFILES          ?= \
	source/library/meta/git.gen \

# Special build commands.

default: all

all: pac${BINEXT} pacdoc${BINEXT} pacfmt${BINEXT} tests${BINEXT}

clean:
	$(foreach F,$(shell find . -iname *.[do] -type f),${RM} $F;)
	$(foreach F,$(shell find . -iname *.gen -type f),${RM} $F;)
	$(foreach F,$(shell find . -maxdepth 1 -type f -executable),${RM} $F;)
	$(foreach F,$(wildcard dox),${RM} -Rf $F;)

help:
	@echo "Available Makefile commands:"
	@echo "  make all         - Build all binaries."
	@echo "  make all DEBUG=1 - Build all binaries in debug mode."
	@echo "  make clean       - Delete all built and generated files."
	@echo "  make dox         - Generate developer documentation."
	@echo "  make help        - Show this help message."

.PHONY: all clean default dox help

# Dependency file inclusion.
-include $(shell find . -iname *.d)

# Dependency maps for primary compile targets.
#
# Most dependencies are resolved automatically via the GCC/clang -MMD command,
# which means that just *.o files need to be listed as required by each target.
# In case of special cases which cannot easily be catered via -MMD, such as
# reliance on files generated from scripts, there is a special section further
# down for writing extra dependency rules.

dox: Doxyfile.gen
	doxygen $<

pac${BINEXT}: ${GENFILES} \
		source/binaries/pac/main.o \
		source/library/arg/parse.o \

pacdoc${BINEXT}: ${GENFILES} \
		source/binaries/pacdoc/main.o \
		source/library/arg/parse.o \

pacfmt${BINEXT}: ${GENFILES} \
		source/binaries/pacfmt/main.o \
		source/library/arg/parse.o \

tests${BINEXT}: ${GENFILES} \
		source/tests/main.unit.o \
		source/library/arg/parse.o \
		source/library/unit/unit.o \

# Special dependency rules.
Doxygen.gen: .git/index
source/library/meta/git.gen: .git/index

# General compile rules.
.SUFFIXES:

%.gen:
	cd $(dir $@) && sh $(notdir $@).sh

%.o:
	${CC} ${CFLAGS} -c -MMD $*.c -o $@

%${BINEXT}:
	${CC} ${LDFLAGS} ${LIBS} -o $@ $(filter %.o,$^)
