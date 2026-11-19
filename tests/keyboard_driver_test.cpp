#include "../src/drivers/drivers.h"
#include <cassert>
#include <cstdio>
#include <map>

static std::map<BYTE,std::vector<std::vector<BYTE>>> responses;
static std::vector<std::vector<BYTE>> pending;
static std::vector<BYTE> feature;
static int opens = 0;
bool HidDev::Open(const HidColl& c) { c_=c; h_=(HANDLE)1; ++opens; return true; }
bool HidDev::OpenFeatureOnly(const HidColl& c) { return Open(c); }
void HidDev::Close() { h_=INVALID_HANDLE_VALUE; }
void HidDev::Flush() {}
bool HidDev::Write(const std::vector<BYTE>& b, int) {
    if(b.size()==33) {
        assert(b[0]==0 && (b[1]==2 || b[1]==0x20));
        assert(b[2]==(b[1]==0x20?1:0) && b[32]==b[1]+b[2]);
        for(size_t i=3;i<32;++i)assert(b[i]==0);
    } else {
        assert(b.size()==17 && b[0]==8 && (b[5]&0x80));
        BYTE sum=0;for(BYTE v:b)sum+=v;assert(sum==0x55);
        assert(b[1]==1 || b[1]==3 || b[1]==4);
    }
    pending=responses[b[1]];
    return true;
}
bool HidDev::SetFeature(const std::vector<BYTE>& b) {
    assert(b.size()==65 && b[0]==0 && b[1]==0x20 && b[2]==1);
    return true;
}
bool HidDev::GetFeature(BYTE id,std::vector<BYTE>& b) {assert(id==0);b=feature;return !b.empty();}
int HidDev::Read(std::vector<BYTE>& b,int) {
    if(pending.empty()){Sleep(1);return -1;}
    b=pending.front();pending.erase(pending.begin());return (int)b.size();
}
std::wstring Hex4(USHORT v) {wchar_t b[8];swprintf(b,8,L"%04X",v);return b;}
static std::vector<BYTE> Flow(BYTE command,BYTE length,BYTE value=0) {
    std::vector<BYTE> b(17,0);b[0]=8;b[1]=command;b[5]=0x80|length;b[6]=value;
    if(command==1){b[10]=1;b[11]=1;}
    if(command==4){b[7]=1;b[8]=0x10;b[9]=0xA5;}
    BYTE sum=0;for(int i=0;i<16;++i)sum+=b[i];b[16]=(BYTE)(0x55-sum);
    return b;
}
static void FlowResponses(BYTE level=95,BYTE connected=1) {
    responses={{1,{Flow(1,8)}},{3,{Flow(3,1,connected)}},{4,{Flow(4,4,level)}}};
    pending.clear();
}
int main() {
    HidColl radio;radio.vid=0x0C45;radio.pid=0xFDFD;radio.iface=3;
    radio.caps.UsagePage=0xFF60;radio.caps.Usage=0x61;
    radio.caps.OutputReportByteLength=radio.caps.InputReportByteLength=33;
    PollContext ctx;ctx.colls={radio};
    responses={{2,{{2,0,0,0x40}}},{0x20,{{0x20,1},{5,1,0,99},{0x20,1,0,73}}}};
    PollKeyboards(ctx);
    assert(ctx.out.size()==1 && ctx.out[0].percent==73 && ctx.out[0].kind==DeviceKind::Keyboard);
    assert(ctx.IsClaimed(radio.vid,radio.pid));
    for(BYTE unknown:{0,101,255}) {
        ctx={};ctx.colls={radio};responses[0x20]={{0x20,1,0,unknown}};PollKeyboards(ctx);
        assert(ctx.out.size()==1 && ctx.out[0].percent==-1);
    }
    ctx={};ctx.colls={radio};responses.clear();pending.clear();PollKeyboards(ctx);
    assert(ctx.out.empty() && ctx.claimed.empty());
    int before=opens;
    auto unrelated=radio;unrelated.caps.OutputReportByteLength=65;ctx.colls={unrelated};PollKeyboards(ctx);
    unrelated=radio;unrelated.pid=0x1234;ctx.colls={unrelated};PollKeyboards(ctx);
    assert(opens==before);
    HidColl wired;wired.vid=0x0C45;wired.pid=0x8009;wired.caps.UsagePage=0xFF13;wired.caps.FeatureReportByteLength=65;
    feature.assign(65,0);feature[1]=0x20;feature[2]=1;feature[4]=42;
    ctx={};ctx.colls={radio,wired};PollKeyboards(ctx);
    assert(ctx.out.size()==1 && ctx.out[0].percent==42 && ctx.out[0].via==L"USB cable");
    assert(ctx.IsClaimed(radio.vid,radio.pid) && ctx.IsClaimed(wired.vid,wired.pid));
    feature[4]=0xFF;ctx={};ctx.colls={wired};PollKeyboards(ctx);
    assert(ctx.out.size()==1 && ctx.out[0].percent==-1 && ctx.out[0].levelText==L"USB");
    HidColl flow;flow.vid=0x05AC;flow.pid=0x024F;flow.iface=1;
    flow.caps.UsagePage=0xFF02;flow.caps.Usage=2;flow.caps.InputReportByteLength=flow.caps.OutputReportByteLength=17;
    FlowResponses();ctx={};ctx.colls={flow};PollKeyboards(ctx);
    assert(ctx.out.size()==1 && ctx.out[0].percent==95 && ctx.out[0].charging && ctx.out[0].millivolts==4261);
    FlowResponses(0);ctx={};ctx.colls={flow};PollKeyboards(ctx);assert(ctx.out[0].percent==0);
    FlowResponses(95,0);ctx={};ctx.colls={flow};PollKeyboards(ctx);assert(ctx.out.empty() && ctx.IsClaimed(flow.vid,flow.pid));
    FlowResponses();auto corrupt=Flow(4,4,20);corrupt[16]^=1;
    responses[4]={corrupt,Flow(4,4,95)};ctx={};ctx.colls={flow};PollKeyboards(ctx);assert(ctx.out[0].percent==95);
    FlowResponses(101);ctx={};ctx.colls={flow};PollKeyboards(ctx);assert(ctx.out[0].percent==-1);
    FlowResponses();responses[1][0][10]=9;ctx={};ctx.colls={flow};PollKeyboards(ctx);assert(ctx.out.empty());
    FlowResponses();before=opens;flow.caps.UsagePage=1;ctx={};ctx.colls={flow};PollKeyboards(ctx);assert(opens==before);
    puts("Keyboard tests passed: AJAZZ handshake/USB/unknown and Lofree identity/checksum/charging/offline/range/interface filters.");
}
