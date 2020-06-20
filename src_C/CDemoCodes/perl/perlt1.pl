#!/usr/bin/perl
my $c = 20;
$a = 3.14159;
zsub();
print "Outside block, \$a = $a\n";
print "Outside block, \$::a = $::a\n";
print "Outside block, \$b = $b\n";
print "Outside block, \$::b = $::b\n";
print "Outside block, \$c = $c\n";
print "Outside block, \$::c = $::c\n";
print "Outside block, \$d = $d\n";
print "Outside block, \$::d = $::d\n";
sub zsub
{
	my $a = 3;
	my $b=5;
	$d = 10;
	print "In block, \$a = $a\n";
	print "In block, \$::a = $::a\n";
	print "In block, \$b = $b\n";
	print "In block, \$::b = $::b\n";
	print "In block, \$c = $c\n";
	print "In block, \$::c = $::c\n";
	print "In block, \$d = $d\n";
	print "In block, \$::d = $::d\n";
}
