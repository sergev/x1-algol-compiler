all:    test-utf8-input objfile from5track

clean:
	rm -f *.o test-utf8-input objfile

test:   test-utf8-input utf8-input.txt
	./test-utf8-input < utf8-input.txt

test-utf8-input: test-utf8-input.pas
	fpc -Miso $<
