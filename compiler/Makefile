DESTDIR ?= .
CFLAGS = -Wall -Werror

all:    pascal x1algc

clean:
	rm -f *.o x1algol x1algc

test:   x1algol ../examples/appendix-b.a60
	cat ../examples/*.lib > tape$$$$ ;\
	./x1algol tape$$$$< ../examples/appendix-b.a60 > app-b.pasout ;\
	./x1algc ../examples/appendix-b.a60 tape$$$$ > app-b.cout ;\
	rm tape$$$$
	diff app-b.pasout app-b.cout

pascal: $(DESTDIR)/x1algol

$(DESTDIR)/x1algol: x1algol.pas
	fpc -Miso -g -gl $< -o$@
