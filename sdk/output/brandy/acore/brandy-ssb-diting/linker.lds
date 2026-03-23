






OUTPUT_ARCH(riscv)
ENTRY(Reset_Handler)



MEMORY
{
    BOOTUP_RAM : ORIGIN = (0x10000), LENGTH = (348)
    VERSION : ORIGIN = ((0x10000) + (348)), LENGTH = (88)
    CHANGE_ID : ORIGIN = (((0x10000) + (348)) + (88)), LENGTH = (16)
    PROGRAM : ORIGIN = ((((0x10000) + (348)) + (88)) + (16)), LENGTH = (((27 + 5) * 4096) - (348) - (88) - 16)

    SYSTEM_CLOCKS : ORIGIN = ((((0x20000000 + 0x70000) - (0x400 + 0x200 + 0x200)) - 0xfc) - 0x30), LENGTH = 0x30





    DTCM : ORIGIN = 0x20000000, LENGTH = 0x70000
    PRESERVE_RAM : ORIGIN = ((0x87000000) + 0x4860), LENGTH = 0xfc
    RSA_PUBLIC_KEY : ORIGIN = ((0x20000000 + 0x70000) - (0x400 + 0x200 + 0x200)), LENGTH = 0x400
    RSA_SIGNATURE : ORIGIN = (((0x20000000 + 0x70000) - (0x400 + 0x200 + 0x200)) + 0x400), LENGTH = 0x200
    RSA_RESULT : ORIGIN = ((((0x20000000 + 0x70000) - (0x400 + 0x200 + 0x200)) + 0x400) + 0x200), LENGTH = 0x200



}

SECTIONS
{

    . = 0;
    __CODE_START__ = ORIGIN(BOOTUP_RAM);
    startup :
    {
        KEEP (*(.text.entry))
    } > BOOTUP_RAM

    .flash_version :
    {
       . = ALIGN(8);
        KEEP (*(.flash_version))
        KEEP (*(.flash_version*))
    } > VERSION
    .change_id :
    {
        KEEP (*(.change_id))
        KEEP (*(.change_id*))
    } > CHANGE_ID


    .text :
    {
        . = ALIGN(64);
        KEEP (*(.text.trap_excp))
        . = ALIGN(64);
        KEEP (*(.text.trap_vector))
        . = ALIGN(64);
        KEEP (*(.isr_vector))
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        *(.srodata)
        *(.srodata*)
        KEEP (*(.gdb_debug))
        . = ALIGN(8);
    } > PROGRAM

    __program_size__ =.;

    .stacks (NOLOAD): ALIGN(16)
    {
        g_system_stack_begin = .;
        . += 0x1400;
        __stack_top__ = .;
        . += 0x1000;
        __irq_stack_top__ = .;
        . += 0x400;
        __excp_stack_top__ = .;
        g_system_stack_end = .;
    } > DTCM
    g_system_stack_size = g_system_stack_end - g_system_stack_begin;
    g_stack_system = g_system_stack_size;


    .ramtext :
    {
        . = ALIGN(8);
        *(.ramtext)
        *(.ramtext*)
        KEEP (*(.flashtext))
        KEEP (*(.flashtext*))
        . = ALIGN(8);
        __ramtext_end__ = .;
    } > PROGRAM


    .data :
    {
        FILL(0xFF)
        __data_load__ = LOADADDR(.data);
        __data_begin__ = .;
        *(.data)
        *(.data*)
        *(.got)
        *(.got*)
        *(.igot)
        *(.igot*)
        _gp_ = . + 0x800;
        *(.sdata)
        *(.sdata*)
        __data_end__ = .;
        . = ALIGN(8);
    } > DTCM AT > PROGRAM
    __data_size__ = __data_end__ - __data_begin__;
    __global_pointer$ = _gp_;



    .bss (NOLOAD) :
    {
        . = ALIGN(8);
        __bss_begin__ = .;
        *(.bss)
        *(.bss*)
        *(.sbss)
        *(.sbss*)
        *(COMMON)
        . = ALIGN(8);
        __bss_end__ = . ;
    } > DTCM
    __bss_size__ = __bss_end__ - __bss_begin__;


    .heap (NOLOAD):
    {
        . = ALIGN(8);
        g_intheap_begin = ABSOLUTE(.);
        g_intheap_size = (ORIGIN(DTCM) + LENGTH(DTCM)) - g_intheap_begin;
    } > DTCM


    .sys_clocks (NOLOAD):
    {
        . = ALIGN(8);
        KEEP(*(sys_clocks))
    } > SYSTEM_CLOCKS
    .rsa_public_key_mem (NOLOAD):
    {
        . = ALIGN(8);
        KEEP(*(rsa_public_key_mem))
    } > RSA_PUBLIC_KEY

    .rsa_signature_mem (NOLOAD):
    {
        . = ALIGN(8);
        KEEP(*(rsa_signature_mem))
    } > RSA_SIGNATURE

    .rsa_result_mem (NOLOAD):
    {
        . = ALIGN(8);
        KEEP(*(rsa_result_mem))
    } > RSA_RESULT

    .preserve (NOLOAD):
    {
        . = ALIGN(8);
        *(preserve)
    } > PRESERVE_RAM

}
