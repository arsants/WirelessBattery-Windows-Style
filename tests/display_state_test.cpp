// Exercise the production result/cache and display logic without touching the tray.
#include "../src/main.cpp"
#include <cassert>
#include "../src/drivers/bluetooth_keyboard.h"

static std::vector<WidgetItem> rendered;
namespace widget {
bool Create(HINSTANCE, HWND, UINT, bool) { return false; }
void Destroy() {}
bool Alive() { return false; }
void SetItems(const std::vector<WidgetItem>& items, const std::wstring&) { rendered = items; }
void SetLightTheme(bool) {}
bool Reposition() { return false; }
}
std::vector<DeviceStatus> PollAllDevices() { return {}; }
std::wstring DumpHidCollections() { return {}; }

int main() {
    g_useWidget = true; // Capture layout through the stub; no Shell_NotifyIcon calls.
    DeviceStatus mouse;
    mouse.key = L"logi:C548:2"; mouse.name = L"MX Master 3S";
    mouse.kind = DeviceKind::Mouse; mouse.percent = 50; mouse.graceSec = 1800;
    std::vector<DeviceStatus> results{mouse};
    OnResults(results);
    assert(rendered.size() == 1 && rendered[0].percent == 50 && !rendered[0].dim);
    results.clear();
    OnResults(results);
    auto& entry = g_entries.at(mouse.key);
    assert(entry.st.percent == -1 && entry.lastPct == 50 && !entry.st.asleep);
    assert(StatusText(entry) == L"50% (last known)");
    assert(rendered[0].percent == 50 && !rendered[0].dim);
    OnResults(results); // Consecutive misses must not invent sleep either.
    assert(!g_entries.at(mouse.key).st.asleep && !rendered[0].dim);
    mouse.percent = 49; results = {mouse}; OnResults(results);
    assert(rendered[0].percent == 49 && !rendered[0].dim);
    assert(StatusText(g_entries.at(mouse.key)) == L"49%");
    mouse.percent = -1; mouse.asleep = true; results = {mouse}; OnResults(results);
    assert(rendered[0].percent == 49 && rendered[0].dim);
    assert(StatusText(g_entries.at(mouse.key)).find(L"asleep") != std::wstring::npos);
    g_entries.at(mouse.key).lastSeen = GetTickCount64() - 1800001;
    results.clear(); OnResults(results);
    assert(g_entries.empty() && rendered.empty());
    mouse.key = L"unknown"; mouse.asleep = false; results = {mouse}; OnResults(results);
    assert(rendered[0].percent == -1 && rendered[0].dim);
    mouse.levelText = L"USB"; results = {mouse}; OnResults(results);
    assert(rendered[0].text == L"USB" && !rendered[0].dim);
    assert(StatusText(g_entries.at(mouse.key)) == L"USB");
    // A single WLMouse identity updates in place on receiver -> cable -> receiver.
    g_entries.clear();
    mouse = {};
    mouse.key = L"wlmouse:A882"; mouse.name = L"WLMouse Beast X";
    mouse.kind = DeviceKind::Mouse; mouse.percent = 93; mouse.graceSec = 1800;
    mouse.via = L"2.4 GHz receiver"; results = {mouse}; OnResults(results);
    const auto receiverUid = g_entries.at(mouse.key).uid;
    mouse.percent = 87; mouse.charging = true; mouse.via = L"USB cable";
    results = {mouse}; OnResults(results);
    assert(g_entries.size() == 1 && rendered.size() == 1 && rendered[0].percent == 87);
    assert(g_entries.at(mouse.key).uid == receiverUid);
    assert(StatusText(g_entries.at(mouse.key)) == L"87%, charging");
    mouse.percent = 88; mouse.charging = false; mouse.via = L"2.4 GHz receiver";
    results = {mouse}; OnResults(results);
    assert(g_entries.size() == 1 && rendered.size() == 1 && rendered[0].percent == 88);
    assert(g_entries.at(mouse.key).uid == receiverUid);
    assert(StatusText(g_entries.at(mouse.key)) == L"88%");
    // The real Rainy Bluetooth name used to fall back to the Bluetooth glyph.
    g_entries.clear();
    DeviceStatus rainy;
    rainy.key = L"bt:rainy-container"; rainy.name = L"Rainy 75-1";
    rainy.via = L"Bluetooth"; rainy.percent = 91;
    NormalizeRainy75Bluetooth(rainy);
    results = {rainy}; OnResults(results);
    assert(rendered.size() == 1 && rendered[0].kind == DeviceKind::Keyboard);
    assert(rendered[0].percent == 91 && !rendered[0].dim && !rendered[0].charging);
    assert(g_entries.at(rainy.key).st.name == L"WOBKEY Rainy 75");
    const auto rainyUid = g_entries.at(rainy.key).uid;
    rainy.percent = 90; results = {rainy}; OnResults(results);
    assert(rendered.size() == 1 && rendered[0].percent == 90);
    assert(g_entries.at(rainy.key).uid == rainyUid);
    results.clear(); OnResults(results); // Disconnected BT must not linger as live.
    assert(g_entries.empty() && rendered.empty());
    results = {rainy}; OnResults(results); // Reconnect with the same identity.
    assert(rendered.size() == 1 && g_entries.at(rainy.key).uid == rainyUid);
    puts("Display state passed: cached level stays normal, explicit sleep dims, refresh recovers, expired entries disappear.");
}
