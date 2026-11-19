// Render the production widget layout into PNGs for visual review, without attaching to Explorer.
#include "../src/widget.cpp"
#include <gdiplus.h>
#include <cstdio>
int main() {
    ULONG_PTR token;
    Gdiplus::GdiplusStartupInput startup;
    Gdiplus::GdiplusStartup(&token, &startup, nullptr);
    CLSID png = {0x557cf406, 0x1a04, 0x11d3, {0x9a,0x73,0x00,0x00,0xf8,0x1e,0xf3,0x2e}};
    for (bool light : {true, false}) {
        g_light = light;
        g_items.clear();
        int index=0;
        const int percent[]={80,50,62,100,94};
        for (auto kind : {DeviceKind::Headset, DeviceKind::Mouse, DeviceKind::Keyboard, DeviceKind::Keyboard, DeviceKind::Gamepad}) {
            WidgetItem item{}; item.kind = kind; item.percent = percent[index++];
            g_items.push_back(item);
        }
        int w; auto pieces = Layout(w);
        Canvas canvas(w, 40);
        int x = Px(8);
        for (auto& p : pieces) { canvas.Draw(p.m, x, (40-p.m.h)/2, p.color, p.alpha); x += p.m.w + p.gapAfter; }
        // Composite the premultiplied pixels over the taskbar shade.
        const int bg = light ? 238 : 32;
        for (auto& p : canvas.px) {
            int inv = 255-(p>>24);
            int r = ((p>>16)&255) + bg*inv/255;
            int g = ((p>>8)&255) + bg*inv/255;
            int b = (p&255) + bg*inv/255;
            p = 0xFF000000u | (r<<16) | (g<<8) | b;
        }
        Gdiplus::Bitmap bitmap(w, 40, w*4, PixelFormat32bppARGB, (BYTE*)canvas.px.data());
        auto result = bitmap.Save(light ? L"build/widget-light.png" : L"build/widget-dark.png", &png, nullptr);
        if (result != Gdiplus::Ok) return 1;
    }
    Gdiplus::GdiplusShutdown(token);
    puts("Rendered production widget layout for light and dark themes at 96 DPI.");
}
