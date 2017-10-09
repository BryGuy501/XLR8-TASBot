# XLR8-TASBot
NES TASBot For Use on an Alorium XLR8 Board

I used an Alorium XLR8 Board with a 64MHz image loaded on it, as well as a SPI SD card breakout board to connect to and control an NES. I made my own run for Super Mario Bros. (the +Duck Hunt cart), but it could hypothetically work with any NES game.

The .ino sketch is to be loaded onto the XLR8 board.

The .bin files are to be loaded onto an SD card and connected to the XLR8.

smb_dh.bin is the run I created, essentially a "human theory" TAS.

smb_wr.bin is the world record run created by Happylee, I adapted it to run on the +DuckHunt cart.

frame2bin.pl is used to create the binary read by the XLR8. To create my binaries, I use BizHawk to make a TAS video which results in a text file that is the list of game instructions. frame2bin.pl parses this file and creates 8 bit controller instructions for the XLR8.

fm22bin.pl is the frame2bin.pl file I adapted to take the fm2 format as input rather than the BizHawk format.
