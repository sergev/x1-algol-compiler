all:

test:   test-utf8-input utf8-input.txt
	./test-utf8-input < utf8-input.txt

test-utf8-input: test-utf8-input.pas
	fpc test-utf8-input.pas
