@echo off
call assemble.bat

set offset=1c1

for /f "tokens=2 delims= " %%i in ('find "lba_addr1" ..\etc\loader.lst') do (
  if %%i NEQ ..\ETC\LOADER.LST (
    set offset=%%i
  )
)

cd ..\projects\Binaries
mkmbrc ..\..\etc\loader.bin ..\shared\mbr.c 0x%offset% ..\..\etc\stage2.bin
cd ..\..\scripts