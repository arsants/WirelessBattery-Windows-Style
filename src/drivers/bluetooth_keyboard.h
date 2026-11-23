#pragma once
#include "../devices.h"
#include <cwctype>

// Rainy 75 Standard and Pro share Bluetooth names; the name cannot identify Pro.
// Accept only this family and its three Bluetooth slots, not Rainy 75 V2/RT/98.
inline bool IsRainy75BluetoothName(std::wstring name) {
    while (!name.empty() && iswspace(name.back())) name.pop_back();
    for (auto& ch : name) ch = towlower(ch);
    for (const wchar_t* base : { L"rainy 75", L"rainy75" }) {
        std::wstring model = base;
        if (name == model) return true;
        for (wchar_t slot = L'1'; slot <= L'3'; ++slot)
            if (name == model + L"-" + slot) return true;
    }
    return false;
}

inline void NormalizeRainy75Bluetooth(DeviceStatus& s) {
    if (!IsRainy75BluetoothName(s.name)) return;
    s.name = L"WOBKEY Rainy 75";
    s.kind = DeviceKind::Keyboard;
    // Keep the Windows container identity. Shared USB VID/PID is not a unique
    // keyboard identity, and different Bluetooth containers must stay separate.
}
