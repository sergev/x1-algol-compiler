all:    x1algol

clean:
	rm -f *.o x1algol test-utf8-input

test:   test-utf8-input utf8-input.txt
	./test-utf8-input < utf8-input.txt

x1algol: x1algol.pas
	fpc -Miso $<

test-utf8-input: test-utf8-input.pas
	fpc -Miso $<
