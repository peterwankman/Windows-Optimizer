@echo off
cd scripts
call config.bat
call assemble.bat
cd ..

md optimizer%VER%
projects\Binaries\mkrand
copy a.out etc\random.bin
del a.out

copy projects\binaries\optimizer.exe optimizer.tmp
projects\binaries\inject etc\ebaums.bin projects\binaries\optimizer.exe optimizer.tmp
copy /b optimizer.tmp /b +etc\random.bin optimizer%VER%\optimizer%VER%.exe
del optimizer.tmp

copy projects\binaries\optimizerd.exe optimizerd.tmp
projects\binaries\inject etc\ebaums.bin projects\binaries\optimizerd.exe optimizerd.tmp
copy /b optimizerd.tmp /b +etc\random.bin optimizer%VER%\optimizerd%VER%.exe
del optimizerd.tmp

projects\binaries\bmp2bin etc\bmp\magic-480-24.bmp magic.tmp
copy /b etc\stage2.bin /b +magic.tmp optimizer%VER%\magic.bin
del magic.tmp

copy license\gpl.txt optimizer%VER%\license.txt