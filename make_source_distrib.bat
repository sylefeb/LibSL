
set PATH=%PATH%;C:\WINDOWS\SYSTEM32;C:\cygwin\bin;C:\cygwin64\bin;E:\cygwin64\bin;E:\cygwin\bin

svn export http://www.antexel.com/svn-libsl LibSL-src-distrib --username sylefeb

cd LibSL-src-distrib

del *.bat

cd ..
