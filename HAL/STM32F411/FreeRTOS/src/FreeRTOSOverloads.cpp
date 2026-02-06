#include "FreeRTOS.h"
#include <cstddef>
#include <new>

void* operator new(std::size_t size) {
    if (size == 0) {
        size = 1;
    }
    if (void* ptr = pvPortMalloc(size)) {
        return ptr;
    }
    return nullptr;
}

void* operator new[](std::size_t size) {
    return operator new(size);
}

void operator delete(void* ptr) noexcept {
    if (ptr) {
        vPortFree(ptr);
    }
}

void operator delete[](void* ptr) noexcept {
    operator delete(ptr);
}

void operator delete(void* ptr, std::size_t) noexcept {
    operator delete(ptr);
}

void operator delete[](void* ptr, std::size_t) noexcept {
    operator delete(ptr);
}

void* operator new(std::size_t size, const std::nothrow_t&) noexcept {
    return operator new(size);
}

void* operator new[](std::size_t size, const std::nothrow_t&) noexcept {
    return operator new(size);
}

void operator delete(void* ptr, const std::nothrow_t&) noexcept {
    operator delete(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t&) noexcept {
    operator delete(ptr);
}
