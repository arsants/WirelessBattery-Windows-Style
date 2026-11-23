// Reads Bluetooth battery properties already held by Windows. No HID probing.
#include "../src/drivers/drivers.h"
#include <cstdio>
int main() {
    PollContext ctx;
    PollBluetooth(ctx);
    for (const auto& s : ctx.out)
        printf("%ls [%ls] percent=%d kind=%d connected=%d key=%ls\n",
               s.name.c_str(),s.via.c_str(),s.percent,(int)s.kind,s.connected,s.key.c_str());
}
