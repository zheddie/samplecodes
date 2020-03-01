#!/usr/bin/perl
our %this ;
$this[1]{"hello"} = "world";
$this[2]{"hello"} = "world2";
my $default       = "this";
$d          = ${ $default }[2]{"hello"};
print $d."\n";
