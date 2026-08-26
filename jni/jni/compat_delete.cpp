#include <stddef.h>

void operator delete[](void* ptr, unsigned int) noexcept
{
    ::operator delete[](ptr);
}
