cd ..\etc

nasm -o ebaums.bin -f bin -l ebaums.lst ebaums.asm
nasm -o loader.bin -f bin -l loader.lst loader.asm

cd ..\scripts
