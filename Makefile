CC=gcc
CFLAGS=-O3
DEPS=
OBJ=update-volume-widget.o
LIBS=-lasound

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

update-volume-widget: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o update-volume-widget
