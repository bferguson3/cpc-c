
#sdcc -mz80 --code-loc 0x0138 --data-loc 0 --no-std-crt0 crt0_cpc.rel putchar_cpc.rel main.c 
#hex2bin main.ihx 
#cpcdiskxp -file main.bin -AddAmsdosHeader 100 
PROJECT=projects/hw

default: 
	sdcc --verbose -mz80 --no-std-crt0 --code-loc 0x4200 --data-loc 0x4100 $(PROJECT)/main.c -o out/main.ihx
	python3 tools/hex2bin.py out/main.ihx ./main.bin
	python3 tools/addamsheader.py main.bin 0x4100 0x4200
	rm main.bin
#	xcpc --drive0=./

clean:
	rm -rf out/*
	rm -rf main.bin
