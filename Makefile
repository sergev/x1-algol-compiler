all:    x1algol

clean:
	rm -f *.o x1algol

test:   x1algol appendix-b.a60
	./x1algol < appendix-b.a60

x1algol: x1algol.pas
	fpc -Miso -g -gl $<
