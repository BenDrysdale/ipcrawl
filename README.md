ipcrawl by dmuz @ AngryPacket
=============================

Crawl through ip address space getting DNS names.
Added to GitHub as I could not find another source for this great little program.

How to compile and use
======================

gcc ipcrawl.c -o ipcrawl
chmod -x ipcrawl
./ipcrawl -q -v -s <seconds> -f <outfile> <startip> <endip>

Options
=======

<startip>     - first address in range to scan
<endip>       - last address of range to scan
-f <outfile>  - log output to file
-s <seconds>  - seconds to wait between every 50 lookups
-q            - quiet (no console output)
-v            - verbose mode
