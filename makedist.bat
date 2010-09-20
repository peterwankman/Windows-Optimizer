@echo off
cd scripts
call config.bat
call assemble.bat
cd ..

md optimizer%VER%
projects\%conf%\mkrand
copy a.out etc\random.bin
del a.out

copy projects\%conf%\optimizer.exe optimizer.tmp
projects\%conf%\inject etc\ebaums.bin projects\%conf%\optimizer.exe optimizer.tmp
copy /b optimizer.tmp /b +etc\random.bin optimizer%VER%\optimizer%VER%.exe
del optimizer.tmp

copy projects\%conf%\optimizerd.exe optimizerd.tmp
projects\%conf%\inject etc\ebaums.bin projects\%conf%\optimizerd.exe optimizerd.tmp
copy /b optimizerd.tmp /b +etc\random.bin optimizer%VER%\optimizerd%VER%.exe
del optimizerd.tmp

projects\%conf%\bmp2bin etc\bmp\magic-600-24.bmp magic.tmp
copy /b etc\stage2.bin /b +magic.tmp optimizer%VER%\magic.bin
del magic.tmp

copy license\gpl.txt optimizer%VER%\license.txt