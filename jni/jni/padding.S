#if defined(__aarch64__)
    .section .bonus_padding,"a"
    .balign 16
    .global brbonus_padding_start
brbonus_padding_start:
    .space 1350000
    .global brbonus_padding_end
brbonus_padding_end:
#elif defined(__arm__)
    .section .bonus_padding,"a"
    .balign 16
    .global brbonus_padding_start
brbonus_padding_start:
    .space 1350000
    .global brbonus_padding_end
brbonus_padding_end:
#endif
