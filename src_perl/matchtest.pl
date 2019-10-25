#!/usr/bin/env perl
$newstr = "/osxpf/v6r1m16f.yj9/bld/cmvc/base.pgm";
$sub = "v7r2m0f";
print("oldstr=".$newstr."\n");
if($sub =~ m"v[0-9]r[0-9]m[0-9]{1,2}f?"){
	$newstr =~ s/\/v[0-9]r[0-9]m[0-9]{1,2}f?\./\/$sub\./;
}
print("sub=".$sub."\n");
print("newstr=".$newstr."\n");
$sub = "shadow";
if($sub =~ m"cmvc|gen|shadow|shadowRoch"){
	$newstr =~ s/\/(cmvc|gen|shadow|shadowRoch)\//\/$sub\//;
	}

print("sub=".$sub."\n");
print("newstr=".$newstr."\n");

