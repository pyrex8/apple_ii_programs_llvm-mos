TARGET = $(notdir $(shell pwd))

all:
	mos-common-clang -o $(TARGET) -O2 $(TARGET).c
	llvm-objcopy -O binary --only-section=.text $(TARGET).elf $(TARGET).bin
	llvm-objdump -d --print-imm-hex $(TARGET).elf
	rm -f $(TARGET) *.elf

clean:
	rm -f *.bin
