// Rainy 75 / Evision discovery only. Battery requests are disabled after an
// input-repeat incident on a Rainy 75 Pro. Do not add active HID probing here.
#include "../src/drivers/drivers.h"
#include <cstdio>

int main(int argc, char**) {
    // Old query flags must fail before even enumerating devices.
    if (argc != 1) {
        fputs("Battery queries are disabled: observed keyboard input interference.\n", stderr);
        return 2;
    }
    bool found = false;
    for (const auto& c : EnumHid()) {
        if (c.vid != 0x320F || (c.pid != 0x5088 && c.pid != 0x5055) ||
            c.page() != 0xFF1C || c.usage() != 0x92) continue;
        found = true;
        printf("%04X:%04X IF=%d IN=%u OUT=%u product=%ls\n",
               c.vid,c.pid,c.iface,c.inLen(),c.outLen(),HidProduct(c).c_str());
    }
    if (!found) puts("No matching Rainy/Evision vendor collection found.");
    return found ? 0 : 1;
}
