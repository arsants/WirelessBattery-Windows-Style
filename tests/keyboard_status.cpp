#include "../src/drivers/drivers.h"
#include <cstdio>
int main() {
    PollContext ctx;ctx.colls=EnumHid();PollKeyboards(ctx);PollBluetooth(ctx);
    for(const auto& s:ctx.out) {
        printf("%ls [%ls] percent=%d connected=%d charging=%d mv=%d text=%ls key=%ls\n",
            s.name.c_str(),s.via.c_str(),s.percent,s.connected,s.charging,s.millivolts,s.levelText.c_str(),s.key.c_str());
    }
}
