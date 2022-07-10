MEMORY {
  ram : ORIGIN = 0x0000, LENGTH = 0x10000
  user_ram (rw) : ORIGIN = 0x0803, LENGTH = 0x8000
}
SECTIONS { INCLUDE c.ld }

__rc0 = 0x00;
INCLUDE imag-regs.ld
ASSERT(__rc0 == 0x00, "Inconsistent zero page map.")
ASSERT(__rc31 == 0x1f, "Inconsistent zero page map.")

/* Top of stack would be 0x10000, but wrap around. */
__stack = 0;

OUTPUT_FORMAT { FULL(ram) SHORT(_start) }
