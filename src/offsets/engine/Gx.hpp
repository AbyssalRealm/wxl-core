// Graphics device addresses, D3D9 vtable indices, and render-state ids.
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
#include <cstddef>

// INTERNAL to the core. The graphics device and the render-pipeline hook points the gx bindings and
// the render events are built from. Modules never include this; they use wxl::game::gx / wxl::events.
namespace wxl::offsets::engine::gx
{
    // The graphics device singleton; the live D3D9 device sits at +kD3DDeviceField inside it.
    constexpr uintptr_t kGxDevicePtr    = 0x00C5DF88; // -> graphics device object
    constexpr size_t    kD3DDeviceField = 0x397C;     // graphics device -> IDirect3DDevice9*

    // Cached render-target surfaces on the graphics-device object.
    constexpr size_t    kBackBufferField   = 0x3B3C; // cached back-buffer surface
    constexpr size_t    kDepthSurfaceField = 0x3B40; // cached world depth surface

    // M2 triangle-batch draw (this-in-ECX). The hook reads the current model so the per-draw event
    // can name which model is rendering.
    constexpr uintptr_t kDrawTriangleBatch      = 0x008203B0;
    constexpr size_t    kDrawBatchCtxModelField = 0x60; // draw context -> current model

    // End of the world 3D-render callback: the world -> UI boundary, the slot for post effects
    // (after all world geometry, before the UI).
    constexpr uintptr_t kWorldRenderEpilogue = 0x004FB074;

    // IDirect3DDevice9 vtable indices used by the gx facade.
    namespace vt
    {
        constexpr unsigned kRelease                = 2;  // COM / shader object release
        constexpr unsigned kGetBackBuffer          = 18;
        constexpr unsigned kCreateTexture          = 23;
        constexpr unsigned kStretchRect            = 34;
        constexpr unsigned kSetRenderTarget        = 37;
        constexpr unsigned kGetRenderTarget        = 38;
        constexpr unsigned kSetDepthStencil        = 39;
        constexpr unsigned kGetDepthStencil        = 40;
        constexpr unsigned kBeginScene             = 41;
        constexpr unsigned kEndScene               = 42;
        constexpr unsigned kClear                  = 43;
        constexpr unsigned kSetViewport            = 47;
        constexpr unsigned kGetViewport            = 48;
        constexpr unsigned kSetRenderState         = 57;
        constexpr unsigned kGetRenderState         = 58;
        constexpr unsigned kSetTexture             = 65;
        constexpr unsigned kSetSamplerState        = 69;
        constexpr unsigned kDrawPrimitiveUP        = 83;
        constexpr unsigned kSetFVF                 = 89;
        constexpr unsigned kCreateVertexShader     = 91;
        constexpr unsigned kSetVertexShader        = 92;
        constexpr unsigned kSetVertexShaderConstantF = 94;
        constexpr unsigned kCreatePixelShader      = 106;
        constexpr unsigned kSetPixelShader         = 107;
        constexpr unsigned kGetPixelShader         = 108;
        constexpr unsigned kSetPixelShaderConstantF = 109;
        constexpr unsigned kDrawIndexedPrimitive   = 0x148 / 4;
    }

    // Engine-internal shader-constant upload (the device's own constant path), addressed as a vtable
    // byte-offset on the graphics-device object. shaderType 0 = vertex, 4 = pixel.
    constexpr int kVtSetShaderConstant = 0x118 / 4; // byte 0x118

    // Engine-internal shader-constant uploader: native this-in-ECX; declared with a dummy second
    // parameter so the trampoline keeps the trailing arguments on the stack.
    using Gx_SetShaderConstantFn = void(__fastcall*)(void* device, void* edx, uint32_t shaderType, uint32_t startReg, const float* data, uint32_t vec4Count);
}
