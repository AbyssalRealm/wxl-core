// Render-pipeline detours that publish the render events.
// Copyright (C) 2026 WarcraftXL
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include "runtime/RenderHooks.hpp"

#include "core/Hook.hpp"
#include "core/Logger.hpp"
#include "events/Event.hpp"
#include "game/gx/Gx.hpp"
#include "offsets/engine/Gx.hpp"

#include <windows.h>

// Only safe detours are used: two device-vtable pointer swaps (DrawIndexedPrimitive, EndScene) and one
// function-entry hook (the M2 batch draw). No mid-function inline patch -> the world render pass is left
// intact. The world->UI post-fx slot is served from the EndScene hook instead (minor UI overlap).
namespace
{
    namespace off = wxl::offsets::engine::gx;
    namespace ev  = wxl::events;
    namespace gx  = wxl::game::gx;

    // The model currently drawing, captured between a batch-draw enter and its DrawIndexedPrimitive.
    void* g_curModel = nullptr;
    // Re-entrancy guard: a subscriber re-issues the draw through the hooked vtable, so do not re-emit.
    bool  g_inM2Emit = false;

    using DrawBatchFn = void (__fastcall*)(void* ctx, void* edx);
    using DIPFn       = long (__stdcall*)(void*, int, int, unsigned, unsigned, unsigned, unsigned);
    using EndSceneFn  = long (__stdcall*)(void*);

    DrawBatchFn g_origDrawBatch = nullptr;
    DIPFn       g_origDIP       = nullptr;
    EndSceneFn  g_origEndScene  = nullptr;

    // Batch draw: record which model is drawing so the per-draw event can name it.
    void __fastcall hkDrawBatch(void* ctx, void* edx)
    {
        g_curModel = *reinterpret_cast<void**>(
            reinterpret_cast<char*>(ctx) + off::kDrawBatchCtxModelField);
        g_origDrawBatch(ctx, edx);
        g_curModel = nullptr;
    }

    // DrawIndexedPrimitive: after the native draw, publish the M2 batch with its draw parameters so a
    // subscriber can re-issue it. Guarded so the subscriber's own re-issue does not recurse.
    long __stdcall hkDIP(void* dev, int pt, int bv, unsigned mi, unsigned nv, unsigned si, unsigned pc)
    {
        long r = g_origDIP(dev, pt, bv, mi, nv, si, pc);
        if (g_curModel && !g_inM2Emit)
        {
            g_inM2Emit = true;
            ev::M2BatchDrawArgs a{ dev, g_curModel, pt, bv, mi, nv, si, pc };
            ev::Emit(ev::Event::OnM2BatchDraw, &a);
            g_inM2Emit = false;
        }
        return r;
    }

    // EndScene: publish end-of-frame (list rebuild + post-fx composite), then run the native call.
    long __stdcall hkEndScene(void* dev)
    {
        ev::EndSceneArgs a{ dev };
        ev::Emit(ev::Event::OnEndScene, &a);
        return g_origEndScene(dev);
    }

    void SwapVtbl(void** vtbl, unsigned idx, void* hook, void** origOut)
    {
        DWORD old;
        VirtualProtect(&vtbl[idx], sizeof(void*), PAGE_EXECUTE_READWRITE, &old);
        *origOut = vtbl[idx];
        vtbl[idx] = hook;
        VirtualProtect(&vtbl[idx], sizeof(void*), old, &old);
    }
}

namespace wxl::runtime::render
{
    void Install()
    {
        void* dev = gx::RawDevice();
        if (!dev) { WLOG_WARN("render: device not up, hooks skipped"); return; }
        void** vtbl = *reinterpret_cast<void***>(dev);

        SwapVtbl(vtbl, off::vt::kDrawIndexedPrimitive, reinterpret_cast<void*>(&hkDIP),
                 reinterpret_cast<void**>(&g_origDIP));
        SwapVtbl(vtbl, off::vt::kEndScene, reinterpret_cast<void*>(&hkEndScene),
                 reinterpret_cast<void**>(&g_origEndScene));

        wxl::core::hook::Install("M2DrawBatch", off::kDrawTriangleBatch,
                                 reinterpret_cast<void*>(&hkDrawBatch),
                                 reinterpret_cast<void**>(&g_origDrawBatch));
        wxl::core::hook::EnableAll();

        WLOG_INFO("render: hooks installed (DIP, EndScene, DrawBatch)");
    }
}
