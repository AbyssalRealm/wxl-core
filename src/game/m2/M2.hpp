// M2 game bindings: typed wrappers over the client's model functions, curated by hand.
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

#include "game/Binding.hpp"
#include "offsets/game/M2.hpp"

// Curated M2 bindings. A module writes wxl::game::m2::ResolveTexture(h) instead of casting an address.
// The wrappers are inline typed calls (zero overhead); RegisterCatalog() adds the names to the
// enumerable catalog for tooling and the future scripting bridge. This is the worked template for the
// other formats' binding sets.
namespace wxl::game::m2
{
    namespace off = wxl::offsets::game::m2;

    // Resolve a texture handle to the internal texture object a sampler bind expects.
    inline void* ResolveTexture(void* handle)
    {
        return Native<off::M2_TexResolveFn>(off::kTexResolve)(handle, 0, 0);
    }

    // Bind a resolved texture to a device sampler selector.
    inline void BindSampler(void* device, uint32_t selector, void* resolvedTexture)
    {
        Native<off::M2_SamplerBindFn>(off::kSamplerBind)(device, nullptr, selector, resolvedTexture);
    }

    // Push the alpha-test reference to the device.
    inline void PushAlphaRef(float ref)
    {
        Native<off::M2_PushAlphaRefFn>(off::kPushAlphaRef)(ref);
    }

    // Add the M2 bindings to the enumerable catalog (cold, at startup).
    inline void RegisterCatalog()
    {
        Register({ "M2::ResolveTexture", off::kTexResolve,   "void*(handle)" });
        Register({ "M2::BindSampler",    off::kSamplerBind,  "void(device, selector, tex)" });
        Register({ "M2::PushAlphaRef",   off::kPushAlphaRef, "void(float ref)" });
    }
}
