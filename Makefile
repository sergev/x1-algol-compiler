CFLAGS = -Wall

all:    x1algol x1algc

clean:
	rm -f *.o x1algol x1algc

test:   x1algol appendix-b.a60
	./x1algol < appendix-b.a60 > app-b.pasout
	./x1algc < appendix-b.a60 > app-b.cout
	diff app-b.pasout app-b.cout

x1algol: x1algol.pas
	fpc -Miso -g -gl $<
