#!/usr/bin/env perl
$un = substr('◌̲',3,2);
sub strop {
return join $un, split '', $_[0];
}
while (<>) {
	s/_([a-z]{1,})/strop($1).$un/eg;
	print;
}
