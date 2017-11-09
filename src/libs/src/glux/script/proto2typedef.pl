#! /usr/bin/perl

$fname=shift;

if (! defined $fname)
{
    die "Input file name (glext.h ?) must be first arg on command line";
}

open(INPUT,"<".$fname) or die "Unable to open input file";
foreach $str (<INPUT>)
{
    if ($str =~ /GLAPI\s+(.*?)\s+APIENTRY\s+(\w*?)\s+\((.*?)\)/)
    {
	$name=uc($2);
	$name="PFN".$name."PROC";
	print "typedef $1 (APIENTRY * $name) ($3);\n";
    }
}
close (INPUT);
