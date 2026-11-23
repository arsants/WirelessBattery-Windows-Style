#include "../src/drivers/bluetooth_keyboard.h"
#include <cassert>
#include <cstdio>
int main() {
    for (const wchar_t* name : { L"Rainy 75-1", L"Rainy 75-2", L"Rainy 75-3", L"Rainy75-1", L"RAINY 75-1 ", L"Rainy 75" }) {
        DeviceStatus s;
        s.name=name; s.key=L"bt:container-A"; s.via=L"Bluetooth"; s.percent=91;
        NormalizeRainy75Bluetooth(s);
        assert(s.name==L"WOBKEY Rainy 75" && s.kind==DeviceKind::Keyboard);
        assert(s.key==L"bt:container-A" && s.via==L"Bluetooth" && s.percent==91);
        assert(!s.charging && !s.asleep && s.graceSec==0);
        auto other=s; other.key=L"bt:container-B";
        NormalizeRainy75Bluetooth(other);
        assert(other.key!=s.key); // Do not merge two keyboards/profiles by model name.
    }
    for (const wchar_t* name : { L"Rainy 75 V2", L"Rainy 75 RT", L"Rainy 98", L"Rainy 75-4", L"Rainy 75-10", L"Not Rainy 75-1", L"Logitech G PRO X 2", L"AK820 5.1-1", L"" }) {
        DeviceStatus s; s.name=name; s.key=L"bt:other"; s.kind=DeviceKind::Headset;
        NormalizeRainy75Bluetooth(s);
        assert(s.name==name && s.kind==DeviceKind::Headset && s.key==L"bt:other");
    }
    puts("Bluetooth keyboard identity tests passed");
}
