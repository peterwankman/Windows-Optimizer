@echo off
cd scripts
call config.bat
call assemble.bat
cd ..

md optimizer%VER%

copy projects\binaries\optimizer.exe optimizer%VER%\optimizer%VER%.exe
copy projects\binaries\optimizerd.exe optimizer%VER%\optimizerd%VER%.exe

projects\binaries\bmp2bin etc\bmp\magic-480-24.bmp magic.tmp
copy /b etc\stage2.bin /b +magic.tmp optimizer%VER%\magic.bin
del magic.tmp

copy license\gpl.txt optimizer%VER%\license.txt