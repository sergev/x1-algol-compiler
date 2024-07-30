#!/usr/bin/env perl
sub strop {
return join '_', split '', $_[0];
}
while (<>) {
	s/_([a-z]{2,})/'_'.strop($1)/eg;
	print;
}
