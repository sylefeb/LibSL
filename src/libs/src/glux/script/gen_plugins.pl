#! /usr/bin/perl

$srcdir = shift;
print "Outputting generate source in ".$srcdir."\n";
%exts=();
@extfiles=();
while (1)
{
    $fname=shift;
    push (@extfiles,$fname);
    if (! defined $fname)
    {
		last; # die "Input file names (glext.h wglext.h ?) must be first arg on command line";
    }

    print "-=-=-=- Parsing extension file $fname -=-=-=-\n";
    open(INPUT,"<".$fname) or die "[glux script] Unable to open input file";
    $in_ext_defs=0;
    $in_ext="";
	$in_cond="";
	$in_cond_test="";
    foreach $str (<INPUT>)
    {
		if ($str =~ /\#ifndef (W?GLX?)\_(\u\w*?)\_(\l\w*)/)
		{
			# if (!($2 eq "VERSION"))
			{
				$in_ext_defs=1;
				$if_level=0;
				$in_ext=$1."_".$2."_".$3;
				# print "EXT $1_$2_$3\n";
				if (! exists $exts{$in_ext})
				{
					$exts{$in_ext}=[[],[],[],[],[]];
				}
			}
			#else
			#{
			#	$in_ext_defs=0;
			#}
		}
		elsif ($in_ext_defs == 1)
		{
			# function prototypes
			if ($str =~ /GLAPI.*APIENTRY\s*(\w*?)\s*\(/)
			{
				#	print "GL  :::: $1 :::: $in_cond_test $in_cond\n";
				$extnfo=$exts{$in_ext};
				($tbl0,$tbl1,$tbl2,$tbl3,$extcond)=@$extnfo;
				$tmp=$1;
				$tmp=~s/PFNGL/PFNGLUX/g;
				push (@$tbl1,[$tmp,"$in_cond_test $in_cond"]);
			}
			elsif ($str =~ /.*\s+\*?(glX\w*?)\s+\(/)
			{
				# print "GLX :::: $1 :::: $in_cond_test $in_cond\n";
				$extnfo=$exts{$in_ext};
				($tbl0,$tbl1,$tbl2,$tbl3,$extcond)=@$extnfo;
				$tmp=$1;
				$tmp=~s/PFNGLX/PFNXGLUX/g;
				push (@$tbl1,[$tmp,"$in_cond_test $in_cond"]);
			}
			elsif ($str =~ /.*WINAPI\s+(\w*?)\s+\(/)
			{
				# print "WGL :::: $1 :::: $in_cond_test $in_cond\n";
				$extnfo=$exts{$in_ext};
				($tbl0,$tbl1,$tbl2,$tbl3,$extcond)=@$extnfo;
				$tmp=$1;
				$tmp=~s/PFNWGL/PFNWGLUX/g;
				push (@$tbl1,[$tmp,"$in_cond_test $in_cond"]);
			}
			# function pointers type
			elsif ($str =~ /typedef.*?\(.*?(PFN\w*?)\s*\)/)
			{
				# print "PFN :::: $1 :::: $in_cond_test $in_cond\n";
				$extnfo=$exts{$in_ext};
				($tbl0,$tbl1,$tbl2,$tbl3,$extcond)=@$extnfo;
				$tmp=$1;
				$tmp=~s/PFNGLX/PFNXGLUX/g;
				$str=~s/PFNGLX/PFNXGLUX/g;
				$tmp=~s/PFNWGL/PFNWGLUX/g;
				$str=~s/PFNWGL/PFNWGLUX/g;
				$tmp=~s/PFNGL/PFNGLUX/g;
				$str=~s/PFNGL/PFNGLUX/g;
				push (@$tbl2,[$tmp,$str,"$in_cond_test $in_cond"]);
			}
			# extension defines
			elsif ($str =~ /\#define (W?GLX?.*?)\s+(.*)/)
			{
				if (!($1 eq $in_ext))
				{
					$extnfo=$exts{$in_ext};
					($tbl0,$tbl1,$tbl2,$tbl3,$extcond)=@$extnfo;
					push (@$tbl0,[$1,$2,"$in_cond_test $in_cond"]);
				}
			}
			# goes out of extension definition ?
			elsif ($if_level == 0 && $str =~ /\#endif/)
			{
				$in_ext_defs=0;
			}
			else
			{
				# extension special conditions
				if ($str =~ /\#ifdef\s+(\w*)/)
				{
					$if_level++;
					if (      !($str =~ /W?GLX?_W?GLX?EXT_PROTOTYPES/)
					   && !($str =~ /GLAPI/)
					   && !($str =~ /APIENTRY/)
					   && !($str =~ /APIENTRYP/)
					   && !($str =~ /\_\_cplusplus/)
					   && !($str =~ /\_\_glext\_h\_/)
					   && !($str =~ /\_\_wglext\_h\_/)
					   && !($str =~ /\_\_glxext\_h\_/))
					{
					    $in_cond_test = "#ifdef";
						$in_cond = $1;
						# SL 2015-07-10 this is depreated, now each element is decorated by condition (assumes single cond)
						# also add to extension conditions
						# $extnfo=$exts{$in_ext};
						# ($tbl0,$tbl1,$tbl2,$tbl3,$extcond)=@$extnfo;
						# push(@$extcond,$str);
					}
				}
				elsif ($str =~ /\#ifndef\s+(\w*)/)
				{
					$if_level++;
					if (      !($str =~ /W?GLX?_W?GLX?EXT_PROTOTYPES/)
					   && !($str =~ /GLAPI/)
					   && !($str =~ /APIENTRY/)
					   && !($str =~ /APIENTRYP/)
					   && !($str =~ /\_\_cplusplus/)
					   && !($str =~ /\_\_glext\_h\_/)
					   && !($str =~ /\_\_wglext\_h\_/)
					   && !($str =~ /\_\_glxext\_h\_/))
					{
					    $in_cond_test = "#ifndef";
						$in_cond = $1;
					}
				}
				elsif ($str =~ /\#if\s+(\w*)/)
				{
					$if_level++;
					# SL: these conditions are otherwise ignored. Fix?
				}
				elsif ($str =~ /\#else/)
				{
					if (length($in_cond)>0) {
						if ($in_cond_test eq "#ifdef") {
							$in_cond_test = "#ifndef";
						} else {
							$in_cond_test = "#ifdef";
						}
					}
				}
				elsif ($str =~ /\#endif/)
				{
					$if_level--;
					$in_cond_test = "";
					$in_cond = "";
				}
				if (!($str=~/W?GLX?_W?GLX?EXT_PROTOTYPES/))
				{
					$extnfo=$exts{$in_ext};
					($tbl0,$tbl1,$tbl2,$tbl3,$extcond)=@$extnfo;
					push (@$tbl3,[$str,"$in_cond_test $in_cond"]);
				}
			}
		}
    }
    close (INPUT);
}

print "-=-=-=- End -=-=-=-\n\n";

  # foreach my $ext (keys %exts)
  # {
      # print $ext."\n";
      # $extnfo=$exts{$ext};
      # ($defstbl,$fctstbl,$defstbl,$supltbl)=@$extnfo;
      # print " Stuff \n";
      # foreach my $supp (@$supltbl)
      # {
      	# print "  ".$supp."\n";
      # }
      # print " Defines \n";
      # foreach my $define (@$defstbl)
      # {
      	# print "  ".$$define[0]." ".$$define[1]."\n";
      # }
      # print " Functions \n";
      # foreach my $fct (@$fctstbl)
      # {
      	# print "  ".$fct."\n";
      # }
      # print " Typedefs \n";
      # foreach my $typ (@$typstbl)
      # {
      	# print "  ".$$typ[0]."\n  ".$$typ[1]."\n";
      # }
 # }

# gen glux header
print "glux.h\n";
$fout=$srcdir."/glux.h";
open(OUTPUT,">".$fout) or die "[glux script] Can't open file ".$fout;
write_glux_header(OUTPUT);
close(OUTPUT);

# gen glux main cpp
print "glux.cpp\n";
$fout=$srcdir."/glux.cpp";
open(OUTPUT,">".$fout) or die "[glux script] Can't open file ".$fout;
write_glux_cpp(OUTPUT);
close(OUTPUT);

print "-=-=-=-= Creating plugins =-=-=-=-\n";
foreach my $ext (keys %exts)
{
    $fout=$srcdir."/".$ext.".h";
#	$fout=$srcdir."/glux.h";
    print "-> $ext [.h]";
    open(OUTPUT,">".$fout) or die "[glux script] Can't open file ".$fout;
    write_h_header($ext,OUTPUT);
    write_h_defines($ext,OUTPUT);
    write_h_typedefs($ext,OUTPUT);
    write_h_externals($ext,OUTPUT);
    write_h_eof($ext,OUTPUT);
    close(OUTPUT);
    print " [.cpp]";
#    $fout=$srcdir."/".$ext.".cpp";
	$fout=$srcdir."/glux.cpp";
    open(OUTPUT,">>".$fout) or die "[glux script] Can't open file ".$fout;
    write_cpp_header($ext,OUTPUT);
    write_cpp_fake($ext,OUTPUT);
    write_cpp_globals($ext,OUTPUT);
    write_cpp_load($ext,OUTPUT);
    write_cpp_eof($ext,OUTPUT);
    close(OUTPUT);
    print "\n";
}

# gen extension defines
# print "glux_ext_defs.h\n";
# $fout=$srcdir."/glux_ext_defs.h";
# open(OUTPUT,">".$fout) or die "[glux script] Can't open file ".$fout;
# write_glux_ext_defs_header(OUTPUT);
# close(OUTPUT);

# gen load all header
# print "glux_load_all.h\n";
# $fout=$srcdir."/glux_load_all.h";
# open(OUTPUT,">".$fout) or die "[glux script] Can't open file ".$fout;
# write_glux_load_all_header(OUTPUT);
# close(OUTPUT);

# gen list of all extensions
@lst=();
foreach my $ext (keys %exts)
{
    push (@lst,$ext);
}
@lst=sort(@lst);
print "list.txt\n";
$fout="./list.txt";
open(OUTPUT,">".$fout) or die "[glux script] Can't open file ".$fout;
foreach $ext (@lst)
{
    print OUTPUT $ext."\n";
}
close(OUTPUT);

# copy extension files into glux src directory
foreach $name (@extfiles) {
  unless ($name eq "") {
  $cpname=$name;
  if ($cpname =~ /wgl/) {
    $cpname=~s/(.*)wgl/wgl/;
  } else {
    $cpname=~s/(.*)gl/gl/;
  }
  $cpname=$srcdir."/glux_".$cpname;
  open(INEXT,"<".$name);
  open(OUTEXT,">".$cpname);
  while (<INEXT>) {
    print OUTEXT $_;
  }
  close (INEXT);
  close (OUTEXT);
  }
}

# done
print "Done !\n";


# ------------------------------------------------------------
# ------------------------------------------------------------
# ------------------------------------------------------------


sub write_h_header()
{
    local($ext,$OUT) = @_;
    my $date=localtime;
    print $OUT "
// --------------------------------------------------------
// Generated by glux perl script ($date)
//
// Sylvain Lefebvre - 2002-2014 - Sylvain.Lefebvre\@inria.fr
// --------------------------------------------------------
#if defined(_WIN32) || defined(_WIN64)
#  include <windows.h>
#endif
#ifdef __APPLE__
#  define GL_GLEXT_LEGACY
#  define GL_GLEXT_FUNCTION_POINTERS
#endif

#ifndef GL_ARB_multitexture // hack due to MESA headers including GL_ARB_multitexture
#define GL_ARB_multitexture
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#undef  GL_ARB_multitexture
#else
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#undef  GL_ARB_multitexture
#endif

#undef  GL_GLEXT_PROTOTYPES
#if defined(_WIN32) || defined(_WIN64)
#  include \"glux_glext.h\"
#  include \"glux_wglext.h\"
#else
#ifdef __APPLE__
//#  include \"glux_glext_apple.h\"
#  include \"glux_glext.h\"
#else
#ifdef EMSCRIPTEN
#  include \"glux_glext.h\"
#else
#  include <GL/glx.h>
#  include \"glux_glext.h\"
#  include \"glux_glxext.h\"
#endif
#endif
#endif
#include \"gluxLoader.h\"
#include \"gluxPlugin.h\"
// --------------------------------------------------------
//         Plugin creation
// --------------------------------------------------------

#ifndef __GLUX_".$ext."__
#define __GLUX_".$ext."__

GLUX_NEW_PLUGIN($ext)
";

    if ($ext=~/WGL/)
    {
	print $OUT "// --------------------------------------------------------\n";
	print $OUT "#if defined(_WIN32) || defined(_WIN64)\n";
    }
    elsif ($ext=~/GLX/)
    {
	print $OUT "// --------------------------------------------------------\n";
	print $OUT "#if !(defined WIN32 || defined(_WIN64) || defined __APPLE__ || defined EMSCRIPTEN)\n";
    }

    # print $OUT "// --------------------------------------------------------\n";
    # print $OUT "//           Extension conditions\n";
    # print $OUT "// --------------------------------------------------------\n";
    ## extension conditions
    # my $extnfo=$exts{$ext};
    # my ($tbl0,$tbl1,$tbl2,$tbl3,$extcond)=@$extnfo;
    # foreach $cnd (@$extcond)
    # {
		# print $OUT $cnd;
    # }
}


# ------------------------------------------------------------


sub write_h_defines()
{
    local($ext,$OUT) = @_;
    my $extnfo=$exts{$ext};
    my ($defstbl,$fctstbl,$typstbl,$supltbl)=@$extnfo;
    print $OUT "// --------------------------------------------------------\n";
    print $OUT "//           Extension defines\n";
    print $OUT "// --------------------------------------------------------\n";
    foreach my $define (@$defstbl)
    {
		if (length($$define[2]) > 1) {print $OUT $$define[2]."\n"}
		print $OUT "#  ifndef ".$$define[0]."\n";
    	print $OUT "#    define ".$$define[0]." ".$$define[1]."\n";
		print $OUT "#  endif\n";
		if (length($$define[2]) > 1) {print $OUT "#endif\n";}
    }
}


# ------------------------------------------------------------


sub write_h_typedefs()
{
    local($ext,$OUT) = @_;
    my $extnfo=$exts{$ext};
    my ($defstbl,$fctstbl,$typstbl,$supltbl)=@$extnfo;
    print $OUT "// --------------------------------------------------------\n";
    print $OUT "//           Extension gl function typedefs\n";
    print $OUT "// --------------------------------------------------------\n";
    @fcts=@$fctstbl;
    foreach my $typ (@$typstbl)
    {
		$fct=shift @fcts;
		if (length($$typ[2])>1) {print $OUT $$typ[2]."\n";}
		if (length($$fct[1])>1) {print $OUT $$fct[1]."\n";}
		print $OUT "#ifndef __GLUX__GLFCT_".$$fct[0]."\n";
		my $decl=$$typ[1];
		$decl=~s/\(.*?PFN.*?\)/\(APIENTRYP $$typ[0]\)/;
		print $OUT $decl;
		print $OUT "#endif\n";
		if (length($$fct[1])>1) {print $OUT "#endif\n";}
		if (length($$typ[2])>1) {print $OUT "#endif\n";}
	}
}


# ------------------------------------------------------------


sub write_h_externals()
{
    local($ext,$OUT) = @_;
    my $extnfo=$exts{$ext};
    my ($defstbl,$fctstbl,$typstbl,$supltbl)=@$extnfo;
    print $OUT "// --------------------------------------------------------\n";
    print $OUT "//           Extension gl functions\n";
    print $OUT "// --------------------------------------------------------\n";
    print $OUT "namespace glux {\n";
    my @fcts=@$fctstbl;
    foreach my $typ (@$typstbl)
    {
		my $fct=shift @fcts;
		if (length($$typ[2])>1) {print $OUT $$typ[2]."\n";}
		if (length($$fct[1])>1) {print $OUT $$fct[1]."\n";}
		print $OUT "#ifndef __GLUX__GLFCT_".$$fct[0]."\n";
      	print $OUT "extern ".$$typ[0]." ".$$fct[0].";\n";
		print $OUT "#endif\n";
		if (length($$fct[1])>1) {print $OUT "#endif\n";}
		if (length($$typ[2])>1) {print $OUT "#endif\n";}
    }
    print $OUT "} // namespace glux\n";
}


# ------------------------------------------------------------


sub write_h_eof()
{
    local($ext,$OUT) = @_;
    print $OUT "// --------------------------------------------------------\n";
    my $extnfo=$exts{$ext};
    my ($tbl0,$tbl1,$tbl2,$tbl3,$extcond)=@$extnfo;
    foreach $cnd (@$extcond)
    {
	print $OUT "#endif\n";
	print $OUT "// --------------------------------------------------------\n";
    }
    if ($ext=~/WGL/ || $ext=~/GLX/)
    {
	print $OUT "#endif\n";
	print $OUT "// --------------------------------------------------------\n";
    }
    print $OUT "#endif\n";
    print $OUT "// --------------------------------------------------------\n";
}


# ------------------------------------------------------------


sub write_cpp_header()
{
    local($ext,$OUT) = @_;
    my $date=localtime;
    print $OUT "
// --------------------------------------------------------
// Generated by glux perl script ($date)
//
// Sylvain Lefebvre - 2002 - Sylvain.Lefebvre\@imag.fr
// --------------------------------------------------------
#include \"$ext.h\"
// --------------------------------------------------------
";
    if ($ext=~/WGL/)
    {
	  print $OUT "#if defined(_WIN32) || defined(_WIN64)\n";
    }
    elsif ($ext=~/GLX/)
    {
  	  print $OUT "#if !(defined(_WIN32) || defined(_WIN64) || defined __APPLE__ || defined EMSCRIPTEN)\n";
    }
    my $extnfo=$exts{$ext};
    my ($tbl0,$tbl1,$tbl2,$tbl3,$extcond)=@$extnfo;
    foreach $cnd (@$extcond)
    {
	  print $OUT $cnd;
    }
}


# ------------------------------------------------------------


sub write_cpp_globals()
{
    local($ext,$OUT) = @_;
    my $extnfo=$exts{$ext};
    my ($defstbl,$fctstbl,$typstbl,$supltbl)=@$extnfo;
    print $OUT "// --------------------------------------------------------\n";
    my @fcts=@$fctstbl;
    foreach my $typ (@$typstbl)
    {
		$fct=shift @fcts;
		if (length($$fct[1])>1) {print $OUT $$fct[1]."\n";}
		print $OUT "#ifndef __GLUX__GLFCT_".$$fct[0]."\n";
      	print $OUT $$typ[0]." "."glux::".$$fct[0]."=glux_fake_".$$fct[0].";\n";
		print $OUT "#endif\n";
		if (length($$fct[1])>1) {print $OUT "#endif\n";}
    }
}


# ------------------------------------------------------------


sub write_cpp_fake()
{
    local($ext,$OUT) = @_;
    print $OUT "// --------------------------------------------------------\n";

    my $extnfo=$exts{$ext};
    my ($defstbl,$fctstbl,$typstbl,$supltbl)=@$extnfo;
    my @fcts=@$fctstbl;
    foreach my $typ (@$typstbl)
    {
		my $fct=shift @fcts;
		if (length($$fct[1])>1) {print $OUT $$fct[1]."\n";}
		my $proto=$$typ[1];
		$proto=~s/\(.*?\)/APIENTRY glux_fake_$$fct[0]/;
		$proto=~s/typedef//;
		$proto=~s/;//;
		print $OUT "#ifndef __GLUX__GLFCT_".$$fct[0]."\n";
		print $OUT $proto;
		print $OUT "{\n";
#	print $OUT "# ifndef GLUX_NO_OUTPUT\n";
#	print $OUT "  std::cerr << \"FATAL: Extension $ext was not loaded or is not fully available.\" << std::endl << \"=> cannot use $$fct[0].\" << std::endl;\n";
#	print $OUT "# endif\n";
# 	print $OUT "  exit (-1);\n";
		print $OUT "# ifndef GLUX_NO_OUTPUT\n";
		print $OUT "  std::cerr << \"gluX - loading extension $ext (triggered by $$fct[0])\" << std::endl;\n";
		print $OUT "# endif\n";
		print $OUT "  static glux::gluxPlugin\_$ext *plugin = NULL;\n";
		print $OUT "  if (plugin == NULL) { plugin = new glux::gluxPlugin\_$ext(true); plugin->init(); } \n";
		print $OUT "  if (!plugin->isAvailable()) { \n";
		print $OUT "    #ifndef GLUX_NO_OUTPUT\n";
		print $OUT "    std::cerr << \"[FAIL] $ext (required)\" << std::endl;\n";
		print $OUT "    #if defined(_WIN32) || defined(_WIN64)\n";
    	print $OUT "    MessageBox(NULL,\"[FAIL] $ext (required)\",\"gluX - Fatal error\",MB_OK | MB_ICONSTOP);\n";
		print $OUT "    #endif\n";
		print $OUT "    #endif\n";
		print $OUT "    exit (-1);\n";
		print $OUT "  } else {\n";
		print $OUT "# ifndef GLUX_NO_OUTPUT\n";
		print $OUT "    std::cerr << \"[ OK ] $ext (required)\" << std::endl;\n";
		print $OUT "# endif\n";
		print $OUT "  } \n";
		print $OUT "  return ";
		my $call =  $$typ[1];
		$call    =~ s/^.*?\)//;
		$call    =~ s/const//g;
		$call    =~ s/struct//g;
		$call    =~ s/\w*void\s//g;
		$call    =~ s/\w*void\)/\)/g;
		$call    =~ s/\w*unsigned\s//g;
		$call    =~ s/\w*unsigned\)/\)/g;
		$call    =~ s/\(\s*\w+\**\s+/\(/;
		$call    =~ s/\,\s*\w+\**\s+/\,/g;
		$call    =~ s/\[\d+\]//g;
		$call    =~ s/\**//g;
		print $OUT "glux::".$$fct[0].$call;
		print $OUT "}\n";
		print $OUT "#endif\n";
		if (length($$fct[1])>1) {print $OUT "#endif\n";}

    }
}


# ------------------------------------------------------------


sub write_cpp_load()
{
    local($ext,$OUT) = @_;
    print $OUT "// --------------------------------------------------------\n";
    print $OUT "GLUX_PLUGIN_LOAD($ext)\n{\n";
    my $extnfo = $exts{$ext};
    my ($defstbl,$fctstbl,$typstbl,$supltbl) = @$extnfo;
    if (!($ext =~ /WGL/) && !($ext =~ /GLX/) && !($ext =~ /VERSION/))
    {
      if ($#$fctstbl >= 0) {
		print $OUT "if (GLUX_CHECK_EXTENSION_STRING(\"$ext\"))\n   m_bDevel=true;\n";
      } else {
        print $OUT "if (GLUX_CHECK_EXTENSION_STRING(\"$ext\"))\n   return (false);\n";
      }
    }
    else
    {
      if ($#$fctstbl < 0)
      {
        if ($ext =~ /WGL/) {
	    print $OUT "if (GLUX_CHECK_WGL_EXTENSION_STRING(\"$ext\"))\n   return (false);\n";
	} else {
	    print $OUT "if (GLUX_CHECK_EXTENSION_STRING(\"$ext\"))\n   return (false);\n";
	}
      }
    }
    my @fcts=@$fctstbl;
    foreach my $typ (@$typstbl)
    {
	my $fct=shift @fcts;
	if (length($$typ[2])>1) {print $OUT $$typ[2]."\n";}
	if (length($$fct[1])>1) {print $OUT $$fct[1]."\n";}
	print $OUT "
#ifndef __GLUX__GLFCT_".$$fct[0]."
     glux::$$fct[0] = ($$typ[0])GLUX_LOAD_PROC(\"$$fct[0]\");
     if(NULL == glux::$$fct[0])
     {
        glux::$$fct[0]=glux_fake_$$fct[0];
        if (!m_bDevel)
	   return false;
        else
        {\n";
	if (!($ext =~ /WGL/) && !($ext =~ /GLX/))
	{
	    print $OUT "
#          ifndef GLUX_NO_OUTPUT
           std::cerr << \"       WARNING \"
                     << \"- $ext - $$fct[0] not in this driver !\"
                     << std::endl;
#          endif\n"
	}
	else
	{
	    print $OUT "           return (false);\n";
	}
	print $OUT "        }
     }
#endif\n";
	if (length($$fct[1])>1) {print $OUT "#endif\n";}
	if (length($$typ[2])>1) {print $OUT "#endif\n";}
    }
    print $OUT "  \n     return true;\n};\n";
}


# ------------------------------------------------------------


sub write_cpp_eof()
{
    local($ext,$OUT) = @_;
    my $extnfo=$exts{$ext};
    my ($tbl0,$tbl1,$tbl2,$tbl3,$extcond)=@$extnfo;
    foreach $cnd (@$extcond)
    {
	print $OUT "#else\nGLUX_PLUGIN_LOAD($ext) { return (false); }\n#endif\n";
	print $OUT "// --------------------------------------------------------\n";
    }

    print $OUT "// --------------------------------------------------------\n";
    if ($ext=~/WGL/ || $ext=~/GLX/)
    {
	print $OUT "#else\nGLUX_PLUGIN_LOAD($ext) { return (false); }\n#endif\n";
	print $OUT "// --------------------------------------------------------\n";
    }
}


# ------------------------------------------------------------


sub write_glux_header()
{
    local($OUT) = @_;
    print $OUT "// --------------------------------------------------------
// Generated by glux perl script ($date)
//
// Sylvain Lefebvre - 2002 - Sylvain.Lefebvre\@imag.fr
// --------------------------------------------------------
#ifndef __GLUX__
#define __GLUX__
// --------------------------------------------------------
#ifdef __APPLE__
#  define GL_GLEXT_LEGACY
#  define GL_GLEXT_FUNCTION_POINTERS
#endif
#include \"gluxLoader.h\"
#include \"gluxPlugin.h\"
// --------------------------------------------------------\n";
    # includes all extension headers
    local $nb=0;
    foreach my $ext (keys %exts)
    {
#	  print $OUT "#include \"$ext.".h\"\n";
      $nb++;
    }
    print $OUT "// --------------------------------------------------------\n";
    # create extension enumeration
    print $OUT "#define GLUX_NUMBER_OF_EXTENSIONS ".$nb."\n";
    print $OUT "#define GLUX_EXTENSION(n) g_ExtensionNames[n]\n";
    print $OUT "namespace glux {\n";
    print $OUT "  extern const char *g_ExtensionNames[$nb];\n";
    print $OUT "} // namespace glux\n";
    print $OUT "// --------------------------------------------------------\n";
    print $OUT "#if (! defined(GLUX_EXPLICIT_NAMESPACE)) // && ( ! __APPLE__ )\n";
    print $OUT "using namespace glux;\n";
    print $OUT "#endif\n";
    # done
    print $OUT "// --------------------------------------------------------\n";
    print $OUT "#endif\n";
    print $OUT "// --------------------------------------------------------\n";
}


# ------------------------------------------------------------


sub write_glux_cpp()
{
    local($OUT) = @_;
    print $OUT "// --------------------------------------------------------
// Generated by glux perl script ($date)
//
// Sylvain Lefebvre - 2002 - Sylvain.Lefebvre\@imag.fr
// --------------------------------------------------------
#include \"glux.h\"

#ifdef __APPLE__
#undef GL_GLEXT_FUNCTION_POINTERS
#include <OpenGL/gl.h>
#endif

#include <cstdlib>
#include <cstdio>
#include <cstring>
// --------------------------------------------------------\n";
    # create extension enumeration
    local $i=0;
    local $nb=0;
    @allexts=();
    foreach my $ext (keys %exts)
    {
	push(@allexts,$ext);
	$nb++;
    }
    @allexts=sort @allexts;
    print $OUT "const char *glux::g_ExtensionNames[$nb]={\n";
    foreach my $ext (@allexts)
    {
	if ($i < $nb-1)
	{
	    print $OUT "            \"$ext\",\n";
	}
	else
	{
	    print $OUT "            \"$ext\"};\n";
	}
	$i++;
    }
#    foreach my $ext (keys %exts)
#    {
#	print $OUT "#include \"".$ext.".cpp\"\n";
#    }
}


# ------------------------------------------------------------


sub write_glux_ext_defs_header()
{
    local($OUT) = @_;
    print $OUT "// --------------------------------------------------------
// Generated by glux perl script ($date)
//
// Sylvain Lefebvre - 2002 - Sylvain.Lefebvre\@imag.fr
// --------------------------------------------------------
#ifndef __GLUX_EXT_DEFS__
#define __GLUX_EXT_DEFS__
// --------------------------------------------------------
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <stddef.h>
#else
#include <X11/X.h>
#ifdef __APPLE__
// no glx
#else
#include <GL/glx.h>
#endif
#endif
// --------------------------------------------------------
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
// --------------------------------------------------------\n";

    print $OUT "//           Extension defines\n";
    print $OUT "// --------------------------------------------------------\n";
    foreach my $ext (keys %exts)
    {
	my $extnfo=$exts{$ext};
	my ($defstbl,$fctstbl,$typstbl,$supltbl)=@$extnfo;
	if ($ext =~ /WGL/)
	{
	    print $OUT "# if defined(_WIN32) || defined(_WIN64)\n";
	}
	elsif ($ext =~ /GLX/)
	{
	    print $OUT "# if !(defined(_WIN32) || defined(_WIN64) || defined __APPLE__)\n";
	}
	else
	{
	    next;
	}
	foreach my $define (@$defstbl)
	{
	    print $OUT "#ifndef ".$$define[0]."\n";
	    print $OUT "#  define ".$$define[0]." ".$$define[1]."\n";
	    print $OUT "#endif\n";
	}
	print $OUT "#endif\n";
    }
    print $OUT "// --------------------------------------------------------\n";
    print $OUT "// Extensions custom declarations\n";
    print $OUT "// --------------------------------------------------------\n";
    foreach my $ext (keys %exts)
    {
	my $extnfo=$exts{$ext};
	my ($defstbl,$fctstbl,$defstbl,$supltbl)=@$extnfo;
	if ($#$supltbl >= 0)
	{
	    #print $OUT "#ifndef $ext\n";
	    if ($ext =~ /WGL/)
	    {
		  print $OUT "# if defined(_WIN32) || defined(_WIN64)\n";
	    }
	    elsif ($ext =~ /GLX/)
	    {
		  print $OUT "# if !(defined(_WIN32) || defined(_WIN64) || defined __APPLE__)\n";
	    }
	    foreach my $supp (@$supltbl)
	    {
		  print $OUT "  ".$supp;
	    }
	    if ($ext=~/WGL/ || $ext=~/GLX/)
	    {
		  print $OUT "# endif\n";
	    }
	    #print $OUT "#endif\n";
	}
    }
    print $OUT "// --------------------------------------------------------\n";
    print $OUT "#endif\n";
    print $OUT "// --------------------------------------------------------\n";
}


# ------------------------------------------------------------


sub write_glux_load_all_header()
{
    local($OUT) = @_;
    print $OUT "// --------------------------------------------------------
// Generated by glux perl script ($date)
//
// Sylvain Lefebvre - 2002 - Sylvain.Lefebvre\@imag.fr
// --------------------------------------------------------
#ifndef __GLUX_LOAD_ALL__
#define __GLUX_LOAD_ALL__
// --------------------------------------------------------\n";
    print $OUT "#include <glux.h>\n";
    print $OUT "// --------------------------------------------------------\n";
    foreach my $ext (keys %exts)
    {
	if ($ext=~/WGL/)
	{
	    print $OUT "#if defined(_WIN32) || defined(_WIN64)\n";
	}
	elsif ($ext=~/GLX/)
	{
	    print $OUT "#if !(defined(_WIN32) || defined(_WIN64) || defined __APPLE__)\n";
	}

	print $OUT " GLUX_LOAD(".$ext.");\n";

	if ($ext=~/WGL/ || $ext=~/GLX/)
	{
	    print $OUT "#endif\n";
	}
    }
    # done
    print $OUT "// --------------------------------------------------------\n";
    print $OUT "#endif\n";
    print $OUT "// --------------------------------------------------------\n";
}


# ------------------------------------------------------------
