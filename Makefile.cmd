echo off
cls
clang -Wall -c pile.c 
clang -Wall pp.c pile.o -o bin\pp.exe
pause
