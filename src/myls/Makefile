CC=gcc
LD=gcc
CFLAGS=-Wall -Werror
LDFLAGS=
EXE=myls
HEADERS=$(wildcard *.h)
CFILES=$(wildcard *.c)
OBJS=$(CFILES:.c=.o)

$(EXE): $(OBJS)
	$(LD) $^ $(LDFLAGS) -o $@

makefile.dep: $(CFILES) $(HEADERS)
	 $(CC) -MM $(CFILES) > $@

clean:
	$(RM) $(OBJS) makefile.dep

mrproper: clean
	 $(RM) $(EXE)
include makefile.dep
