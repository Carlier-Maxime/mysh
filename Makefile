.PHONY: all clean

all: main.c
	gcc main.c -o mysh

clean:
	rm main.c