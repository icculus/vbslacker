@echo off
cls
echo This is only going to work with cygwin-32 and a miracle.
echo  Desperately seeking makefiles.
echo  Attempting to build testlib program...and everything else.
cd ..\..\BASIClib
rm -f *.o
gcc -Wall -c *.c

REM nasm -f win32 -o OnEventsAsm.o OnEvents.asm

mv *.o ../tools/testlib
cd ..\tools\testlib
gcc -I../../BASIClib -DDEBUG -g -Wall -c *.c
gcc -lm -o testlib.exe *.o
rm -f *.o


rem ugh. End of maketest.bat ...


