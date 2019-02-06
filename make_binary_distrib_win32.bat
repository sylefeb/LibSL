
set PATH=%PATH%;C:\WINDOWS\SYSTEM32

mkdir LibSL-bin-win32
cd LibSL-bin-win32

copy ..\*.cmake    .
copy ..\Licence*.txt .
copy ..\CREDITS.txt  .
copy ..\README.txt  .

mkdir src
cd src
mkdir libs
mkdir LibSL
xcopy ..\..\src\libs\* libs\ /E
xcopy ..\..\src\LibSL\*.h LibSL\ /E
xcopy ..\..\src\LibSL\*.inl LibSL\ /E
cd ..

mkdir lib
xcopy ..\lib\*.lib lib\ /E

cd    src
mkdir tools
cd    tools
mkdir autobindshader
cd ..
xcopy ..\..\src\tools\autobindshader\* tools\autobindshader\ /E
cd ..

cd ..
