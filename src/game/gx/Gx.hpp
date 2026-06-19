// gx bindings: the live D3D9 device and a typed facade scripts draw through.
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

#pragma once

#include <cstdint>

#include "offsets/engine/Gx.hpp"

// RawDevice() returns the live D3D9 device. Device9 is a thin typed facade over
// its vtable: every method is an inline vtable call (zero overhead, no facade vtable of our own), so it
// is safe in any path. This is the reusable "aide" every render script draws through; the script keeps
// only its own logic and shaders. Method names mirror IDirect3DDevice9 so the mapping is obvious.
namespace wxl::game::gx
{
    namespace off = wxl::offsets::engine::gx;

    // Standard D3D9 enum values a script passes to the facade. These are public D3D constants (not
    // client offsets), exposed here so a script never includes the internal offsets headers.
    namespace rs    { constexpr unsigned kZEnable = 7, kZWrite = 14, kSrcBlend = 19, kDestBlend = 20,
                                         kCullMode = 22, kZFunc = 23, kAlphaBlend = 27, kStencilEnable = 52; }
    namespace blend { constexpr unsigned kSrcAlpha = 5, kInvSrcAlpha = 6; }
    namespace cmp   { constexpr unsigned kLessEqual = 4; }
    namespace cull  { constexpr unsigned kNone = 1; }
    namespace clear { constexpr unsigned kTarget = 1, kZBuffer = 2, kStencil = 4; }
    namespace prim  { constexpr int kTriangleStrip = 5; }

    // Fetch slot `idx` of `obj`'s vtable as a typed function pointer.
    template <class Fn>
    inline Fn Vtbl(void* obj, unsigned idx)
    {
        return reinterpret_cast<Fn>((*reinterpret_cast<void***>(obj))[idx]);
    }

    // Release a COM object got from a Get* call (IUnknown::Release, vtable slot 2).
    inline void Release(void* obj)
    {
        if (obj) Vtbl<unsigned long(__stdcall*)(void*)>(obj, 2)(obj);
    }

    // The live D3D9 device, or null if the graphics device is not up yet.
    inline void* RawDevice()
    {
        void* g = *reinterpret_cast<void**>(off::kGxDevicePtr);
        if (!g) return nullptr;
        return *reinterpret_cast<void**>(reinterpret_cast<char*>(g) + off::kD3DDeviceField);
    }

    class Device9
    {
    public:
        explicit Device9(void* dev) : dev_(dev) {}
        void*    raw() const { return dev_; }
        explicit operator bool() const { return dev_ != nullptr; }

        long SetRenderState(unsigned state, unsigned value)
        { return Call<long(__stdcall*)(void*, unsigned, unsigned)>(off::vt::kSetRenderState)(dev_, state, value); }

        unsigned GetRenderState(unsigned state)
        { unsigned v = 0; Call<long(__stdcall*)(void*, unsigned, unsigned*)>(off::vt::kGetRenderState)(dev_, state, &v); return v; }

        long SetRenderTarget(unsigned index, void* surface)
        { return Call<long(__stdcall*)(void*, unsigned, void*)>(off::vt::kSetRenderTarget)(dev_, index, surface); }

        long GetRenderTarget(unsigned index, void** outSurface)
        { return Call<long(__stdcall*)(void*, unsigned, void**)>(off::vt::kGetRenderTarget)(dev_, index, outSurface); }

        long SetDepthStencil(void* surface)
        { return Call<long(__stdcall*)(void*, void*)>(off::vt::kSetDepthStencil)(dev_, surface); }

        long GetDepthStencil(void** outSurface)
        { return Call<long(__stdcall*)(void*, void**)>(off::vt::kGetDepthStencil)(dev_, outSurface); }

        long SetPixelShader(void* shader)
        { return Call<long(__stdcall*)(void*, void*)>(off::vt::kSetPixelShader)(dev_, shader); }

        long GetPixelShader(void** outShader)
        { return Call<long(__stdcall*)(void*, void**)>(off::vt::kGetPixelShader)(dev_, outShader); }

        long SetPixelShaderConstantF(unsigned startReg, const float* data, unsigned vec4Count)
        { return Call<long(__stdcall*)(void*, unsigned, const float*, unsigned)>(off::vt::kSetPixelShaderConstantF)(dev_, startReg, data, vec4Count); }

        long SetVertexShader(void* shader)
        { return Call<long(__stdcall*)(void*, void*)>(off::vt::kSetVertexShader)(dev_, shader); }

        long SetTexture(unsigned stage, void* texture)
        { return Call<long(__stdcall*)(void*, unsigned, void*)>(off::vt::kSetTexture)(dev_, stage, texture); }

        long Clear(unsigned rectCount, const void* rects, unsigned flags, unsigned color, float z, unsigned stencil)
        { return Call<long(__stdcall*)(void*, unsigned, const void*, unsigned, unsigned, float, unsigned)>(off::vt::kClear)(dev_, rectCount, rects, flags, color, z, stencil); }

        long DrawIndexedPrimitive(int primType, int baseVertex, unsigned minIndex, unsigned numVerts,
                                  unsigned startIndex, unsigned primCount)
        { return Call<long(__stdcall*)(void*, int, int, unsigned, unsigned, unsigned, unsigned)>(off::vt::kDrawIndexedPrimitive)(dev_, primType, baseVertex, minIndex, numVerts, startIndex, primCount); }

        // Viewport read/write into a 24-byte D3DVIEWPORT9 buffer (save/restore around an RT swap).
        long GetViewport(void* viewport24)
        { return Call<long(__stdcall*)(void*, void*)>(off::vt::kGetViewport)(dev_, viewport24); }

        long SetViewport(const void* viewport24)
        { return Call<long(__stdcall*)(void*, const void*)>(off::vt::kSetViewport)(dev_, viewport24); }

    private:
        template <class Fn>
        Fn Call(unsigned idx) const { return Vtbl<Fn>(dev_, idx); }
        void* dev_;
    };

    // The current device, wrapped. Test it before use: bool(Device()) is false until graphics is up.
    inline Device9 Device() { return Device9(RawDevice()); }

    // --- toolbox helpers (reusable "aides", implemented in the .cpp) ---
    // Compile an HLSL pixel shader (e.g. target "ps_2_0") into a device shader, or null on failure.
    void* CompilePixelShader(Device9 dev, const char* hlsl, const char* target);

    // A render target sized to the back buffer: texture + its level-0 surface.
    struct RenderTarget { void* texture = nullptr; void* surface = nullptr; int width = 0; int height = 0; };

    // Create (once) or keep a back-buffer-sized render target of the given format. False on failure.
    bool EnsureBackbufferTarget(Device9 dev, RenderTarget& rt, uint32_t d3dFormat);

    // Draw a back-buffer-sized textured quad (samples texture stage 0 through the bound pixel shader).
    void DrawFullscreenQuad(Device9 dev);
}
