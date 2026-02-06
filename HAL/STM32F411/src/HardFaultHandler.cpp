#include <cstdint>

extern "C" void HardFault_Handler(void) {
    volatile uint32_t hardFaultMarker = 0xDEADBEEF;
    (void)hardFaultMarker;
    while (true) {
    }
}
