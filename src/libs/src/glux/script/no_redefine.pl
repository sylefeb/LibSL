#! /usr/bin/perl

open(OUTPUT,">./src/glux_no_redefine.h") or die "Unable to open output file (glux_no_redefine.h)";
print OUTPUT "#ifndef __GLUX_NO_REDEFINE__\n";
print OUTPUT "#define __GLUX_NO_REDEFINE__\n\n";
while (1)
{

    $fname=shift;
    
    if (! defined $fname)
    {
	last;
    }
    elsif ($fname eq "--clear")
    {
	print OUTPUT "\n#endif\n";
	close (OUTPUT);
	print "glux_no_redefine.h successfully cleared.\n";
	exit;
    }
    
    print "-=-=-=- Parsing $fname -=-=-=-\n";
    open(INPUT,"<".$fname) or die "Unable to open input file";
    foreach $str (<INPUT>)
    {
	if ($str =~ /(gl.*?)\s*\(/)
	{
	    print OUTPUT "#define __GLUX__GLFCT_".$1."\n";
	}
    }
    close (INPUT);
}
print OUTPUT "\n#endif\n";
close (OUTPUT);
print "glux_no_redefine.h successfully written.\n";
# ------------------------------------------------------------
