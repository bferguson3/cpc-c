
#sdcc -mz80 --code-loc 0x0138 --data-loc 0 --no-std-crt0 crt0_cpc.rel putchar_cpc.rel main.c 
#hex2bin main.ihx 
#cpcdiskxp -file main.bin -AddAmsdosHeader 100 
PROJECT=projects/hw

default: 
	sdcc --data-loc 0x138 --code-loc 0x200 -mz80 --no-std-crt0 lib/crt0_cpc.rel lib/putchar_cpc.rel $(PROJECT)/main.c -o out/main.ihx
	python3 tools/hex2bin.py out/main.ihx ./main.bin
#	python3 addamsheader.py MAIN.BIN
#	xcpc --drive0=./

clean:
	rm -rf out/*
	rm main.bin
