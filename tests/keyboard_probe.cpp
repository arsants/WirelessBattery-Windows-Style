// Diagnostic: only documented read-only battery/online commands, no configuration writes.
#include "../src/hid.h"
#include <cstdio>
static void Dump(const char* label, const std::vector<BYTE>& b) {
    printf("%s (%zu):", label, b.size());
    for (BYTE v : b) printf(" %02X", v);
    puts(""); fflush(stdout);
}
int main() {
    for (const auto& c : EnumHid()) {
        if (c.vid == 0x0C45 && (c.pid == 0x8009 || c.pid == 0x800A)) {
            printf("AJAZZ USB %04X:%04X IF:%d PAGE:%04X USAGE:%04X IN:%u OUT:%u FEAT:%u\n",c.vid,c.pid,c.iface,c.page(),c.usage(),c.inLen(),c.outLen(),c.featLen());
            if (c.page()==0xFF13 && c.featLen()==65) {
                HidDev wired;
                if(wired.OpenFeatureOnly(c)) {
                    std::vector<BYTE> req(65,0),r;req[1]=0x20;req[2]=1;
                    Dump("USB BATTERY TX",req);
                    printf("USB SET_FEATURE: %d\n",(int)wired.SetFeature(req));Sleep(50);
                    if(wired.GetFeature(0,r))Dump("USB BATTERY RX",r);else printf("USB GET_FEATURE error %lu\n",GetLastError());
                }
            }
        }
        bool ajazz = c.vid == 0x0C45 && c.pid == 0xFDFD && c.page() == 0xFF60 && c.outLen() == 33;
        if (c.vid == 0x0C45 && c.pid == 0xFDFD && c.page() == 0xFFFF) {
            printf("AJAZZ passive status IF:%d IN:%u\n",c.iface,c.inLen());
            HidDev passive;
            if (passive.Open(c)) {
                std::vector<BYTE> status;
                ULONGLONG end=GetTickCount64()+1200;
                while(GetTickCount64()<end)if(passive.Read(status,50)>0)Dump("STATUS",status);
            }
        }
        bool flow = c.vid == 0x05AC && c.pid == 0x024F && c.page() == 0xFF02 && c.usage() == 2 && c.outLen() == 17;
        if (!ajazz && !flow) continue;
        printf("%s VID:%04X PID:%04X IF:%d IN:%u OUT:%u FEAT:%u\n", ajazz ? "AJAZZ" : "Lofree", c.vid,c.pid,c.iface,c.inLen(),c.outLen(),c.featLen());
        HidDev d; if (!d.Open(c)) { printf("open failed %lu\n",GetLastError()); continue; }
        d.Flush();
        if (flow) {
            // Official HUB performs this device-identification challenge before online/battery.
            std::vector<BYTE> hello(17,0), reply;
            hello[0]=8;hello[1]=1;hello[5]=0x88;
            hello[6]=0x12;hello[7]=0x34;hello[8]=0x56;hello[9]=0x78;
            BYTE sum=0;for(int i=0;i<16;++i)sum+=hello[i];hello[16]=(BYTE)(0x55-sum);
            Dump("HELLO TX",hello);d.Write(hello);
            ULONGLONG end=GetTickCount64()+700;
            while(GetTickCount64()<end)if(d.Read(reply,50)>0)Dump("HELLO RX",reply);
        }
        for (int round=0; round<3; ++round) {
            for (int cmd : ajazz ? std::vector<int>{2,0x20} : std::vector<int>{3,4}) {
                std::vector<BYTE> request(c.outLen(),0), reply;
                if (ajazz) {request[1]=cmd; request[2]=cmd==0x20?1:0; request[32]=request[1]+request[2];}
                else {request[0]=8; request[1]=cmd; request[5]=0x80; BYTE sum=0; for (int i=0;i<16;++i)sum+=request[i];request[16]=(BYTE)(0x55-sum);}
                Dump("TX",request);
                if (!d.Write(request)) {printf("write failed %lu\n",GetLastError());continue;}
                ULONGLONG end=GetTickCount64()+700;
                bool got=false;
                while (GetTickCount64()<end) if (d.Read(reply,50)>0) {Dump("RX",reply);got=true;}
                if (!got) puts("RX: timeout");
            }
            Sleep(200);
        }
    }
}
