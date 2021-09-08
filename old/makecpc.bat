@ECHO OFF 
rem zcc +cpc -lndos -lm -subtype=dsk -create-app -o appcpc main.c
del main.bin 
sdcc -mz80 --code-loc 0x0138 --data-loc 0 --no-std-crt0 crt0_cpc.rel putchar_cpc.rel main.c 
hex2bin main.ihx 
cpcdiskxp -file main.bin -AddAmsdosHeader 100 
del *.ihx 
del *.lk 
del *.lst 
del *.map 
del *.noi 
del main.rel 
del *.sym 
del *.asm
@ECHO ON
