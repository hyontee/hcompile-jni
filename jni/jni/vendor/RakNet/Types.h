#ifndef RAKNET_LEGACY_TYPES_H
#define RAKNET_LEGACY_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <limits.h>

/*
 * Compatibility header for the legacy RakNet cryptography sources bundled
 * with this project.  The original SHA1/BigTypes sources expect these fixed
 * width aliases and a couple of endian/inline helpers.
 */
namespace cat {
    typedef int8_t   s8;
    typedef uint8_t  u8;
    typedef int16_t  s16;
    typedef uint16_t u16;
    typedef int32_t  s32;
    typedef uint32_t u32;
    typedef int64_t  s64;
    typedef uint64_t u64;
}

#ifndef INLINE
#  if defined(_MSC_VER)
#    define INLINE __forceinline
#  else
#    define INLINE inline __attribute__((always_inline))
#  endif
#endif

#ifndef ROL32
#  define ROL32(value, bits) (static_cast<uint32_t>((static_cast<uint32_t>(value) << (bits)) | (static_cast<uint32_t>(value) >> (32 - (bits)))))
#endif

/* Keep legacy preprocessor checks used by SHA1/DataBlockEncryptor. */
#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#  ifndef HOST_ENDIAN_IS_BIG
#    define HOST_ENDIAN_IS_BIG 1
#  endif
#  ifndef BIG_ENDIAN
#    define BIG_ENDIAN 4321
#  endif
#else
#  ifndef LITTLE_ENDIAN
#    define LITTLE_ENDIAN 1234
#  endif
#endif

#endif /* RAKNET_LEGACY_TYPES_H */
