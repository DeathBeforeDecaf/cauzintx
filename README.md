# cauzinTX
Program to encode one or more input files and create a PostScript output file for printing on a 300 DPI (or better) laser printer.  The proprietary two dimensional barcode printed with the PostScript file is used as input for the Cauzin Softstrip optical reader.	Only the direct render format mode has been implemented, and the only tests have been performed with an HP Laserjet capable of 600dpi.

> Target Platform: DOS and MacOS
> - MS Visual C++ 1.50
> - Apple clang version 13.1.6

A prebuilt binary compiled with MSVC should be available in the /bin directory.

If you want to print a PostScript file from the MacOS command line, try: `lp -d printer_name -o raw filename.ps`

Many thanks to FozzTexx and all the other contributers who have identified available documentation for the proprietary format and have provided information about the Cauzin SoftStrip system.
