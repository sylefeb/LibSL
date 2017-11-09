#!/usr/bin/perl

$file=shift;
open(IN,"<$file") or die "Cannot open file $file\n";
$strfile=$file;
$strfile=~s/\.cg/\.string/;
open(OUT,">$strfile") or die "Cannot open file $strfile\n";

print OUT "\"";
while (<IN>) {
    $line=$_;
    $line =~ s/\\/\\\\/g;
    $line =~ s/\"/\\\"/g;
    $line =~ s/\n//g;
    $line =~ s/\r//g;
    print OUT $line."\\n\\\n";
}
print OUT "\";\n";
close(IN);
close(OUT);
