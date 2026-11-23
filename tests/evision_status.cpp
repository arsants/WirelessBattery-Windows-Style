// Rainy 75 / Evision diagnostic. Enumeration only by default.
// Protocol reference: https://github.com/zzixxxx/Rainy75Desktop (battery.py).
// No keymap, lighting, pairing, firmware, or configuration commands.
#include "../src/drivers/drivers.h"
#include <cstdio>
#include <cstring>

int main(int argc, char** argv) {
    bool query = false, receiver = false;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--battery")) query = true;
        else if (!strcmp(argv[i], "--receiver")) receiver = true;
        else { puts("Usage: evision_status.exe [--battery [--receiver]]"); return 2; }
    }
    bool found = false;
    for (const auto& c : EnumHid()) {
        if (c.vid != 0x320F || (c.pid != 0x5088 && c.pid != 0x5055) ||
            c.page() != 0xFF1C || c.usage() != 0x92) continue;
        found = true;
        printf("%04X:%04X IF=%d IN=%u OUT=%u product=%ls\n",
               c.vid,c.pid,c.iface,c.inLen(),c.outLen(),HidProduct(c).c_str());
        fflush(stdout);
        if (!query) continue;
        if (c.pid == 0x5088 && !receiver) {
            puts("Receiver queries disabled. Use USB cable mode for diagnostics."); continue;
        }
        if (c.inLen() != 64 || c.outLen() != 64) {
            puts("Unexpected report lengths; no query sent."); continue;
        }
        LASTINPUTINFO input{sizeof(input)};
        if (!GetLastInputInfo(&input) || DWORD(GetTickCount()-input.dwTime) < 1000) {
            puts("Input active or idle state unknown; no query sent."); return 1;
        }
        HidDev d;
        if (!d.Open(c)) { printf("Open error: %lu\n",GetLastError()); return 1; }
        d.Flush();
        // Byte 1 is a checksum (1A + 06 = 20), not a transaction sequence.
        std::vector<BYTE> req(64,0),r;
        req[0]=4; req[1]=0x20; req[3]=0x1A; req[4]=6;
        if (!d.Write(req)) { printf("Write error: %lu\n",GetLastError()); return 1; }
        // Send once. Never retransmit while the user might be typing.
        ULONGLONG end=GetTickCount64()+15000;
        while(GetTickCount64()<end) {
            if(d.Read(r,100)<0) continue;
            printf("Reply:"); for(BYTE b:r)printf(" %02X",b);puts("");fflush(stdout);
            if(r.size()>=10 && r[0]==4 && r[3]==0x1A && r[8]>=1 && r[8]<=100) {
                printf("Battery candidate=%u%% raw state=%u (charging unverified)\n",r[8],r[9]); return 0;
            }
        }
        puts("No valid battery reply; no percentage inferred."); return 1;
    }
    if (!found) puts("No matching Rainy/Evision vendor collection found.");
    return found ? 0 : 1;
}
