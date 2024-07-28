all:    test-utf8-input

clean:
	rm -f *.o test-utf8-input

test:   test-utf8-input utf8-input.txt
	./test-utf8-input < utf8-input.txt

test-utf8-input: test-utf8-input.pas
	fpc -Miso $<
