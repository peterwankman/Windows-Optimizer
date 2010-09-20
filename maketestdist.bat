@echo off
cd scripts
call config.bat
call assemble.bat
cd ..

md optimizer%VER%

copy projects\%conf%\optimizer.exe optimizer%VER%\optimizer%VER%.exe
copy projects\%conf%\optimizerd.exe optimizer%VER%\optimizerd%VER%.exe

projects\%conf%\bmp2bin etc\bmp\magic-600-24.bmp magic.tmp
copy /b etc\stage2.bin /b +magic.tmp optimizer%VER%\magic.bin
del magic.tmp

copy license\gpl.txt optimizer%VER%\license.txt