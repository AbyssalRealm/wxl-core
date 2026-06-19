// ADT game bindings: typed wrappers over the client's terrain functions, curated by hand.
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
#include "offsets/game/ADT.hpp"

// Curated ADT bindings. A module writes wxl::game::adt::GetChunk(pos) instead of casting an address.
// The wrappers are inline typed calls (zero overhead); RegisterCatalog() adds the names to the
// enumerable catalog for tooling and the future scripting bridge.
namespace wxl::game::adt
{
    namespace off = wxl::offsets::game::adt;

    // Chunk lookup (pos) -> runtime chunk object, or null when that chunk is not parsed yet.
    inline void* GetChunk(float* pos)
    {
        return Native<off::Map_GetChunkFn>(off::kGetChunk)(pos);
    }

    // Count placed-object children still loading that overlap the chunk box.
    inline int NearObjectCount(void* chunk, int* progressOut, int total)
    {
        return Native<off::Map_NearObjectCountFn>(off::kNearObjectCount)(chunk, progressOut, total);
    }

    // Read a tile-slot pointer from the X-major tile grid. Null when out of range.
    inline void* TileSlot(uint32_t tileX, uint32_t tileY)
    {
        if (tileX >= off::kTileGridDim || tileY >= off::kTileGridDim)
            return nullptr;
        return *reinterpret_cast<void**>(off::kTileSlots + (tileX * off::kTileGridDim + tileY) * off::kTileSlotStride);
    }

    // Add the ADT bindings to the enumerable catalog (cold, at startup).
    inline void RegisterCatalog()
    {
        Register({ "ADT::GetChunk",        off::kGetChunk,        "void*(float* pos)" });
        Register({ "ADT::NearObjectCount", off::kNearObjectCount, "int(chunk, int* progress, int total)" });
    }
}
