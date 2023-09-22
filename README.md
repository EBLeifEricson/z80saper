# z80saper
Minesweeper clone for CP/M

This is a port of [Igor Reshetnikov's "Saper"](https://minesweepergame.com/download/game.php?id=110), which itself is a clone of Minesweeper for CP/M. Saper was originally written in Pascal for the CRISS CP/M computer. I took the liberty of porting it to C (using z88dk), translating it to English, and making it compatible with generic Z80 and 8080 CP/M machines.

Z80Saper also adds a few extra features, such as a "Custom" difficulty which lets you specify a grid size & mine count, and a timer to help gauge your performance. It also has an improved random number generator, and allows you to specify your clock speed in order to get an accurate timer across different hardware configurations.

## Installation
If using the prebuilt binary, simply copy "MINES.COM" to your CP/M machine (in whatever way is convenient). It should work with no other modifications.

If you are building from source, you must use z88dk. The exact build command I used (which also enables 8080 support) was: ```zcc +cpm -clib 8080 mines.c -o mines.com```
