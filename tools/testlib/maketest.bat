@echo off
cls
echo This is only going to work with cygwin-32 and a miracle.
echo  Desperately seeking makefiles.
echo  Attempting to build testlib program...and everything else.
cd ..\..\BASIClib
rm -f *.o
gcc -Wall -DDEBUG -DWIN32 -c -g *.c
gcc -Wall -DDEBUG -DWIN32 -c -g -o OnEventsAsm.o OnEvents.S

mv *.o ../tools/testlib
cd ..\tools\testlib
gcc -I../../BASIClib -DWIN32 -DDEBUG -g -Wall -c *.c
gcc -g -lm -o testlib.exe *.o
rm -f *.o


rem ugh. End of maketest.bat ...


