@echo off
call scripts\config.bat

cd projects

del /q bmp2bin\*.user
rd /q /s bmp2bin\release
rd /q /s bmp2bin\debug

del /q inject\*.user
rd /q /s inject\release
rd /q /s inject\debug


del /q mkmbrc\*.user
rd /q /s mkmbrc\release
rd /q /s mkmbrc\debug

del /q mkrand\*.user
rd /q /s mkrand\release
rd /q /s mkrand\debug

del /q optimizer\*.user
rd /q /s optimizer\release
rd /q /s optimizer\test

del /q optimizerd\*.user
rd /q /s optimizerd\release
rd /q /s optimizerd\test

rd /q /s release
rd /q /s debug

del /q shared\mbr.c

cd ..

attrib -h optimizer.suo
del /q optimizer.suo
del /q optimizer.ncb
del /q optimizer.sdf

del /q etc\random.bin
del /q etc\ebaums.bin
del /q etc\ebaums.lst
del /q etc\loader.bin
del /q etc\loader.lst
del /q etc\stage2.bin

rd /q /s optimizer%VER%
rd /q /s release
rd /q /s debug
rd /q /s ipch