// WMO game bindings: typed wrappers over the client's map-object functions, curated by hand.
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
#include "offsets/game/WMO.hpp"

// Curated WMO bindings. A module writes wxl::game::wmo::GroupResident(m, i, f) instead of casting an
// address. The wrappers are inline typed calls (zero overhead); RegisterCatalog() adds the names to
// the enumerable catalog for tooling and the future scripting bridge.
namespace wxl::game::wmo
{
    namespace off = wxl::offsets::game::wmo;

    // Resolve a material's texture-name offsets. The native does not bounds-check materialIndex.
    inline void ResolveMaterialTexture(void* model, int materialIndex)
    {
        Native<off::Wmo_ResolveMaterialTextureFn>(off::kResolveMaterialTexture)(model, nullptr, materialIndex);
    }

    // Query the resident state of a group, optionally forcing residency.
    inline unsigned int GroupResident(void* model, unsigned int groupIndex, unsigned int force)
    {
        return Native<off::Wmo_GroupResidentFn>(off::kGroupResidentAccessor)(model, nullptr, groupIndex, force);
    }

    // Root buffer pointer, or null on a null root.
    inline void* RootBuffer(void* root)
    {
        if (!root)
            return nullptr;
        return *reinterpret_cast<void**>(reinterpret_cast<char*>(root) + off::kOffRootBuffer);
    }

    // Group count (the group-array bound), or 0 on a null root.
    inline uint32_t GroupCount(void* root)
    {
        if (!root)
            return 0;
        return *reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(root) + off::kOffGroupCount);
    }

    // Group runtime object at index i, or null when out of range or on a null root.
    inline void* GroupAt(void* root, uint32_t i)
    {
        if (!root || i >= GroupCount(root))
            return nullptr;
        char* base = reinterpret_cast<char*>(root) + off::kOffGroupArray;
        return *reinterpret_cast<void**>(base + i * 4);
    }

    // Add the WMO bindings to the enumerable catalog (cold, at startup).
    inline void RegisterCatalog()
    {
        Register({ "WMO::ResolveMaterialTexture", off::kResolveMaterialTexture,  "void(model, int materialIndex)" });
        Register({ "WMO::GroupResident",          off::kGroupResidentAccessor,   "uint(model, groupIndex, force)" });
    }
}
