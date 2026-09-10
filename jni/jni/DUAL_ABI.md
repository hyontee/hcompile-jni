Build targets:
arm64-v8a
armeabi-v7a

The project uses the prebuilt Dobby libraries from the working example for both ARM ABIs.
The recovered internal libplugin.so RVAs are applied only on arm64-v8a because the supplied original ELF is AArch64. The 32-bit build keeps the network hooks active and avoids applying incompatible 64-bit RVAs.

0x00110007 is treated as a 32-bit/version-specific address candidate, not as a raw file offset into the supplied 64-bit ELF.
