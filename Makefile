all:    x1algol

clean:
	rm -f *.o x1algol

x1algol: x1algol.pas
	fpc -Miso -g -gl $<
