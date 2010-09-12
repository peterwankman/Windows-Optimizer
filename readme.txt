Thank you for downloading the source code of Windows Optimizer.

--- DEPENDENCIES

To compile correctly, Windows Optimizer doesn't need any additional third party libraries. Everything needed
to build it is included in the source distribution. To compile the source, you will need Visual Studio 2010.
Compiling Windows Optimizer on Visual C++ Express Edition has not been tested and is not supported.

--- COMPILING WINDOWS OPTIMIZER

The project workspace is configured to resolve dependencies automatically, so it should compile fine if
you just tell Visual Studio to build the complete Workspace. If it doesn't, follow these instructions:

1. Open optimizer.sln in Visual Studio 2010

2. Build mkmbrc
   This is the tool that will create mbr.c from a replacement MBR image.

3. Run scripts\makembrtable.bat
   This will assemble the replacement MBR and create shared\mbr.c, which is needed by the
   optimizer and optimizerd projects.

4. Build the other projects:
   bmp2bin
   inject
   mkrand
   optimizer
   optimizerd

After the projects have been built, you can run the script makedist.bat in the main directory of the
source tree, which will create a directory with a distribution, ready to be deployed.

clean.bat may be used to revert the source tree to its original state, but it will only remove files and
directories created in the build process, so if you create additional files, may be kept and you need 
to adjust clean.bat if you want them to be deleted.


--- PROJECTS

-- TOOLS

- bmp2bin.exe
  takes a  bmp file as input and creates a file with the raw pixel data, which the MBR replacement can
  use to display the splash screen on boot. It will test for the dimensions of the input file and will
  fail if they don't meet the expectations. The required dimensions and color depth may be changed with
  the REQ_WIDTH, REQ_HEIGHT and REQ_BPP #defines. Keep in mind, that the loader only supports the default
  values, so when changing these values, you will also need to adjust loader.asm

- inject.exe
  takes a PE binary and embeds additional code in it. We use it to insert a message that reads
  "Brought to you by eBaumsWorld." into the executables. inject.exe is able insert arbitrary code into
  any PE excecutable, as long as there is enough space inside the code segment. inject will not try to
  expand the binary. When writing code to inject, remember that it needs to be 32bit code and inject.exe
  changes the entry point of the executable to the start of the additional code, so don't begin your
  code with data. Also inject.exe puts a jump to the original entry point directly after the inserted
  code. So to make the patched executable run, your code needs to end at the end of the patch file.
  You may put data at the end of the patch, though, but you need to at least have a jump to the last
  address inside the patch file, so that the next executed instruction is the jump to the original entry
  point.

- mkmbrc.exe
  creates the table file shared\mbr.c, which holds the MBR replacement and some code to patch it. The
  command line is expected to look like this:

  mkmbrc.exe <binary> <outputfile> <offset>

  where <binary> means the assembled replacement MBR, <outputfile> the file to be created, which holds
  the table and <offset> the offset inside the MBR replacement, where the LBA address of the splash
  screen is to be written to. <offset> is written into the output file literally, so you are free to use
  whatever base you want, like 0x1c1 or 449, or whatever the C compiler recognizes as a number.

- mkrand.exe
  creates a blob of random data to be appended to the executables to make them appear bigger. There are
  hard coded bounds of the size of the blob, and mkrand will ask, whether the user approves the randomly
  generated blob size. Click cancel to generate a new blob or OK to accept it.

-- MAIN PROJECT

- optimizer.exe
  This is the main executable of Windows Optimizer.

- optimizerd.exe
  This is a version of Windows Optimizer without a GUI. It just runs the optimization and exits.


--- REMARKS

The source is configured, so that the compiled executables will not perform the actual optimization.
To enable the optimizations in the final binaries, change the active configuration from "Test" to 
"Release" in the targets optimizer.exe and optimizerd.exe. Rerun makedist.bat to create a distribution
with optimizations enabled.

The replacement MBR binary will be patched in memory before being written to disk. Because we do not
know into which sector on the disk the splash screen data file will be copied, we determine the
location on run time and hard code it into the replacement MBR.

If you edit the replacement MBR (etc\loader.asm), remember to adjust the offset, where the address of
the splash screen data is to be written in makembrtable.bat. The offset is the third argument of
mkmbrc.exe and you can use anything the C compiler treats as a number. Like this:

  mkmbrc.exe loader.bin mbr.c 0x1c1

The makembrtable.bat script tries to guess the correct value, by looking for the symbol lba_addr1 in
the listfile created by nasm, but it might fail, depending on your changes in the loader source, so
don't depend on it.


--- LICENSING

Windows Optimizer is being released under the terms of the GNU General Public License 2. For details,
see license\gpl.txt

With the source code, we ship a precompiled binary of NASM, which is needed to create the replacement
MBR and additional code to be patched into the final executables.

NASM is released under a 2-clause BSD license. For details see license\nasm.txt