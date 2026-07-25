#include <panic.h>

void panic(void) {
    if (failed_to_load) {
        panic("System process failure!");
    }
}
