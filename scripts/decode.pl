#!/usr/bin/perl

use strict;

my @letters = qw(A A S S X LA D LS B B T T Y Y Z Z);

my @mods = (' ', 'A', 'B', 'C');
my @conds = (' ', 'P', 'Z', 'E');
my @reacts = (' ', 'U', 'Y', 'N');

my @opc = ();
$opc[  8] ='ETMR';
$opc[  9] ='ETMP';
$opc[ 10] ='FTMR';
$opc[ 11] ='FTMP';
$opc[ 12] ='RET';
$opc[ 13] ='EIS';
$opc[ 14] ='TRAD';
$opc[ 15] ='TRAS';
$opc[ 16] ='TIAD';
$opc[ 17] ='TIAS';
$opc[ 18] ='TFA';
$opc[ 19] ='FOR0';
$opc[ 20] ='FOR1';
$opc[ 21] ='FOR2';
$opc[ 22] ='FOR3';
$opc[ 23] ='FOR4';
$opc[ 24] ='FOR5';
$opc[ 25] ='FOR6';
$opc[ 26] ='FOR7';
$opc[ 27] ='FOR8';
$opc[ 28] ='GTA';
$opc[ 29] ='SSI';
$opc[ 30] ='CAC';
$opc[ 31] ='TRRD';
$opc[ 32] ='TRRS';
$opc[ 33] ='TIRD';
$opc[ 34] ='TIRS';
$opc[ 35] ='TFR';
$opc[ 36] ='ADRD';
$opc[ 37] ='ADRS';
$opc[ 38] ='ADID';
$opc[ 39] ='ADIS';
$opc[ 40] ='ADF';
$opc[ 41] ='SURD';
$opc[ 42] ='SURS';
$opc[ 43] ='SUID';
$opc[ 44] ='SUIS';
$opc[ 45] ='SUF';
$opc[ 46] ='MURD';
$opc[ 47] ='MURS';
$opc[ 48] ='MUID';
$opc[ 49] ='MUIS';
$opc[ 50] ='MUF';
$opc[ 51] ='DIRD';
$opc[ 52] ='DIRS';
$opc[ 53] ='DIID';
$opc[ 54] ='DIIS';
$opc[ 55] ='DIF';
$opc[ 56] ='IND';
$opc[ 57] ='NEG';
$opc[ 58] ='TAR';
$opc[ 59] ='ADD';
$opc[ 60] ='SUB';
$opc[ 61] ='MUL';
$opc[ 62] ='DIV';
$opc[ 63] ='IDI';
$opc[ 64] ='TTP';
$opc[ 65] ='MOR';
$opc[ 66] ='LST';
$opc[ 67] ='EQU';
$opc[ 68] ='MST';
$opc[ 69] ='LES';
$opc[ 70] ='UQU';
$opc[ 71] ='NON';
$opc[ 72] ='AND';
$opc[ 73] ='OR';
$opc[ 74] ='IMP';
$opc[ 75] ='QVL';
$opc[ 76] ='abs';
$opc[ 77] ='sign';
$opc[ 78] ='sqrt';
$opc[ 79] ='sin';
$opc[ 80] ='cos';
$opc[ 82] ='ln';
$opc[ 83] ='exp';
$opc[ 84] ='entier';
$opc[ 85] ='ST';
$opc[ 86] ='STA';
$opc[ 87] ='STP';
$opc[ 88] ='STAP';
$opc[ 89] ='SCC';
$opc[ 90] ='RSF';
$opc[ 91] ='ISF';
$opc[ 92] ='RVA';
$opc[ 93] ='IVA';
$opc[ 94] ='LAP';
$opc[ 95] ='VAP';
$opc[ 96] ='START';
$opc[ 97] ='STOP';
$opc[ 98] ='TFP';
$opc[ 99] ='TAS';
$opc[100] ='OBC6';
$opc[101] ='FLOATER';
$opc[102] ='read';
$opc[103] ='print';
$opc[104] ='TAB';
$opc[105] ='NLCR';
$opc[106] ='XEEN';
$opc[107] ='SPACE';
$opc[108] ='stop';
$opc[109] ='P21';

# Opcode proper, if it does not follow conventional naming.
my @ocp = ();
$ocp[16] = 'MULAS';
$ocp[17] = 'MULAS-';
$ocp[18] = 'MULS';
$ocp[19] = 'MULS-';
$ocp[24] = 'DIVAS';
$ocp[25] = 'DIVAS-';
$ocp[26] = 'DIVA';
$ocp[27] = 'DIVA-';

$ocp[40] = 'JUMP';
$ocp[41] = 'JUMP-';
$ocp[42] = 'GOTO';      # also GOTOR
$ocp[46] = 'SUB0';      # also SUB1-SUB15
$ocp[066] = 'MOVR';
$ocp[067] = 'NEGR';

my $decr = $ARGV[0] if $#ARGV >= 0;
@ARGV = ();
my $op;

while (my $line = <>) {
    # print $line;    
    if ($line =~ /^\s*([0-9]+)\s+([0-9]+)\s*(.*)$/) {
        next if $1 < $decr;
	my $code = $2;
        my $tail = $3;
        $op = 0;
        if ($code % 32768 >= $decr) {
                $code -= $decr;
                $op = 1;
        } elsif ($code > 7 && $code < 110) {
                $op = $code;
        }
        if ($op >= 8) {
                printf "%#6o %3d               ", $1-$decr, $op;
        } else {
                printf "%#6o %3d %#010o  ", $1-$decr, $op, $code;
        }
	if ($code < 110) {
	    printf "%s", $opc[$code]
	} else {
	    my $addr = $code & 0o77777;
	    my $letter = $letters[($code >> 23) & 15];
	    my $func = ($code >> 21) & (substr($letter, 0, 1) eq 'L' ? 3 : 7);
	    my $mod = $mods[($code >> 19) & 3];
	    my $cond = $conds[($code >> 17) & 3];
	    my $react = $reacts[($code >> 15) & 3];
	    if (defined($ocp[$code >> 21])) {
		printf "%s %s %#06o %s %s", $react, $ocp[$code >> 21], $addr, $mod, $cond;
	    } else {
		printf "%s %d%s %#06o %s %s", $react, $func, $letter, $addr, $mod, $cond;
	    }
	}
	printf " ($tail)" if $tail;
	printf "\n";
    }
}

