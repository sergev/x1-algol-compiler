program test_utf8_input(input, output);
var
    ascii_table: array[0..127] of integer;
    input_line: shortstring;
    input_pos: integer;
    i, count: integer;

    function read_next_byte: integer;
    var i: integer;
        ch: char;
    begin
        if input_pos >= length(input_line) then begin
            writeln('Bad input: ', input_line);
            halt
        end;
        input_pos := input_pos + 1;
        ch := input_line[input_pos];
        i := ord(ch);
        {writeln(ch, ' ', i);}
        read_next_byte := i;
    end;

    function read_utf8_symbol: integer;
    label 1;
    var i, a: integer;
    begin
1:      if input_pos >= length(input_line) then begin
            if eof(input) then begin
                writeln('End of input');
                halt
            end;
            readln(input, input_line);
            input_pos := 0;
            {writeln('---');}
            exit(119); {newline}
        end;
        i := read_next_byte;
        if i < 128 then begin
            a := ascii_table[i];
            if a < 0 then
                goto 1;
            exit(a);
        end;
        if i = 194 then begin
            i := read_next_byte;
            if i = 172 then exit(76); {¬}
        end else if i = 195 then begin
            i := read_next_byte;
            if i = 151 then exit(66); {×}
        end else if i = 226 then begin
            i := read_next_byte;
            if i = 136 then begin
                i := read_next_byte;
                if i = 167 then exit(77); {∧}
                if i = 168 then exit(78); {∨}
            end;
            if i = 143 then begin
                i := read_next_byte;
                if i = 168 then exit(89); {⏨}
            end;
        end;
        writeln('Bad input: ', input_line);
        halt
    end;

    procedure test(expect: integer);
    var ch: integer;
    begin
        ch := read_utf8_symbol;
        if ch <> expect
        then begin
            writeln('Bad input ', ch, ' expect ', expect, ' (count ', count, ')');
            halt
        end;
        count := count + 1;
    end;

{main program}
begin
    {initialization of ascii_table}
    for i:= 0 to 127 do
        ascii_table[i] := -1;
    ascii_table[ord('0')] := 0;
    ascii_table[ord('1')] := 1;
    ascii_table[ord('2')] := 2;
    ascii_table[ord('3')] := 3;
    ascii_table[ord('4')] := 4;
    ascii_table[ord('5')] := 5;
    ascii_table[ord('6')] := 6;
    ascii_table[ord('7')] := 7;
    ascii_table[ord('8')] := 8;
    ascii_table[ord('9')] := 9;
    ascii_table[ord('a')] := 10;
    ascii_table[ord('b')] := 11;
    ascii_table[ord('c')] := 12;
    ascii_table[ord('d')] := 13;
    ascii_table[ord('e')] := 14;
    ascii_table[ord('f')] := 15;
    ascii_table[ord('g')] := 16;
    ascii_table[ord('h')] := 17;
    ascii_table[ord('i')] := 18;
    ascii_table[ord('j')] := 19;
    ascii_table[ord('k')] := 20;
    ascii_table[ord('l')] := 21;
    ascii_table[ord('m')] := 22;
    ascii_table[ord('n')] := 23;
    ascii_table[ord('o')] := 24;
    ascii_table[ord('p')] := 25;
    ascii_table[ord('q')] := 26;
    ascii_table[ord('r')] := 27;
    ascii_table[ord('s')] := 28;
    ascii_table[ord('t')] := 29;
    ascii_table[ord('u')] := 30;
    ascii_table[ord('v')] := 31;
    ascii_table[ord('w')] := 32;
    ascii_table[ord('x')] := 33;
    ascii_table[ord('y')] := 34;
    ascii_table[ord('z')] := 35;
    ascii_table[ord('A')] := 37;
    ascii_table[ord('B')] := 38;
    ascii_table[ord('C')] := 39;
    ascii_table[ord('D')] := 40;
    ascii_table[ord('E')] := 41;
    ascii_table[ord('F')] := 42;
    ascii_table[ord('G')] := 43;
    ascii_table[ord('H')] := 44;
    ascii_table[ord('I')] := 45;
    ascii_table[ord('J')] := 46;
    ascii_table[ord('K')] := 47;
    ascii_table[ord('L')] := 48;
    ascii_table[ord('M')] := 49;
    ascii_table[ord('N')] := 50;
    ascii_table[ord('O')] := 51;
    ascii_table[ord('P')] := 52;
    ascii_table[ord('Q')] := 53;
    ascii_table[ord('R')] := 54;
    ascii_table[ord('S')] := 55;
    ascii_table[ord('T')] := 56;
    ascii_table[ord('U')] := 57;
    ascii_table[ord('V')] := 58;
    ascii_table[ord('W')] := 59;
    ascii_table[ord('X')] := 60;
    ascii_table[ord('Y')] := 61;
    ascii_table[ord('Z')] := 62;
    ascii_table[ord('+')] := 64;
    ascii_table[ord('-')] := 65;
    ascii_table[ord('/')] := 67;
    ascii_table[ord('>')] := 70;
    ascii_table[ord('=')] := 72;
    ascii_table[ord('<')] := 74;
    ascii_table[ord(',')] := 87;
    ascii_table[ord('.')] := 88;
    ascii_table[ord(';')] := 91;
    ascii_table[ord('(')] := 98;
    ascii_table[ord(')')] := 99;
    ascii_table[ord('[')] := 100;
    ascii_table[ord(']')] := 101;
    ascii_table[ord(' ')] := 119;
    ascii_table[9]        := 118; {tab}
    ascii_table[10]       := 119; {newline}
    ascii_table[ord('''')] := 120; {'}
    ascii_table[ord('"')] := 121;
    ascii_table[ord('?')] := 122;
    ascii_table[ord(' ')] := 123; {space}
    ascii_table[ord(':')] := 124;
    ascii_table[ord('|')] := 162;
    ascii_table[ord('_')] := 163;

    readln(input, input_line);
    count := 0;
    test(0);   { 0 }
    test(1);   { 1 }
    test(2);   { 2 }
    test(3);   { 3 }
    test(4);   { 4 }
    test(5);   { 5 }
    test(6);   { 6 }
    test(7);   { 7 }
    test(8);   { 8 }
    test(9);   { 9 }
    test(119);
    test(10);  { a }
    test(11);  { b }
    test(12);  { c }
    test(13);  { d }
    test(14);  { e }
    test(15);  { f }
    test(16);  { g }
    test(17);  { h }
    test(18);  { i }
    test(19);  { j }
    test(119);
    test(20);  { k }
    test(21);  { l }
    test(22);  { m }
    test(23);  { n }
    test(24);  { o }
    test(25);  { p }
    test(26);  { q }
    test(27);  { r }
    test(28);  { s }
    test(29);  { t }
    test(119);
    test(30);  { u }
    test(31);  { v }
    test(32);  { w }
    test(33);  { x }
    test(34);  { y }
    test(35);  { z }
    test(37);  { A }
    test(38);  { B }
    test(39);  { C }
    test(40);  { D }
    test(119);
    test(41);  { E }
    test(42);  { F }
    test(43);  { G }
    test(44);  { H }
    test(45);  { I }
    test(46);  { J }
    test(47);  { K }
    test(48);  { L }
    test(49);  { M }
    test(50);  { N }
    test(119);
    test(51);  { O }
    test(52);  { P }
    test(53);  { Q }
    test(54);  { R }
    test(55);  { S }
    test(56);  { T }
    test(57);  { U }
    test(58);  { V }
    test(59);  { W }
    test(60);  { X }
    test(119);
    test(61);  { Y }
    test(62);  { Z }
    test(64);  { + }
    test(65);  { - }
    test(66);  { × }
    test(67);  { / }
    test(70);  { > }
    test(72);  { = }
    test(74);  { < }
    test(76);  { ¬ }
    test(119);
    test(77);  { ∧ }
    test(78);  { ∨ }
    test(87);  { , }
    test(88);  { . }
    test(89);  { ⏨ }
    test(91);  { ; }
    test(98);  { ( }
    test(99);  { ) }
    test(100); { [ }
    test(101); { ] }
    test(119);
    test(118); { tab }
    test(120); { ' }
    test(121); { " }
    test(122); { ? }
    test(123); { space }
    test(124); { : }
    test(162); { | }
    test(163); { _ }
    writeln('Test PASSED.');
end.
