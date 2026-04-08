#
# Makefile para Pthreads
#

CC=gcc

# Ajuste das CFLAGS para usar pthreads em vez de OpenMP
ifeq ($(CC),gcc)
	CFLAGS = -O3 -pthread -Wall
else ifeq ($(CC),clang)
	CFLAGS = -O3 -pthread -Wall
else ifeq ($(CC),pgcc)
	CFLAGS = -fast -g -lpthread -Minfo=all
else
	CFLAGS = -O3 -pthread
endif

# Bibliotecas (math library e pthread como fallback)
LDLIBS=-lm -lpthread
LIBS=-lm -lpthread

RM=rm -vf
MV=mv
BINDIR=./bin/
SRCDIR=./src/
PROG=teste

vpath %.c ./src/

# Garante que o diretório bin exista
$(shell mkdir -p $(BINDIR))

SRCFILES= $(wildcard src/*.c)
_PROGS= $(patsubst src/%.c, %, $(SRCFILES))
PROGFILES=$(addprefix $(BINDIR),$(_PROGS))

.PHONY: all clean run list

all: $(PROGFILES)

# Regra de compilação corrigida
$(BINDIR)%: $(SRCDIR)%.c
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

run:
	$(BINDIR)$(PROG)

list:
	ls $(BINDIR)*

clean:
	$(RM) $(PROGFILES) *~
