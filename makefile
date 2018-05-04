IDIR=include
SDIR=src
ODIR=obj
CC=msp430-gcc
CFLAGS=-I $(IDIR) -mmcu=msp430f5529 -mdisable-watchdog
LIBS=

# Dependências

_DEPS=clock.h pmm.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

# Módulos

_OBJ=clock.o pmm.o main.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

install: $(OBJ) $(TEST)
	$(CC) -o main.bin $^ $(CFLAGS) $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

build:
	if [ ! -d $ODIR ]; then mkdir $(ODIR); fi

clean:
	rm -f $(ODIR)/*.o main.bin
