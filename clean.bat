@echo off
call scripts\config.bat

cd projects

del /q bmp2bin\*.user
rd /q /s bmp2bin\intermediate

del /q inject\*.user
rd /q /s inject\intermediate

del /q mkmbrc\*.user
rd /q /s mkmbrc\intermediate

del /q mkrand\*.user
rd /q /s mkrand\intermediate

del /q optimizer\*.user
del /q optimizer\*.pch
del /q optimizer\*.aps
rd /q /s optimizer\intermediate

del /q optimizerd\*.user
del /q optimizerd\*.aps
rd /q /s optimizerd\intermediate

del /q shared\mbr.c

del /q Binaries\*.*

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
rd /q /s ipch