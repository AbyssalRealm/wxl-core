// Unit / object lookup addresses and runtime object field offsets.
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

// INTERNAL to the core. The object/unit query entries and field offsets the unit/world bindings wrap.
namespace wxl::offsets::game::unit
{
    // --- selection globals (u64 GUIDs) ---
    constexpr uintptr_t kMouseoverGuid = 0x00BD07A0;
    constexpr uintptr_t kTargetGuid    = 0x00BD07B0;

    // --- entries ---
    // Resolve a GUID to an object (guid, typemask, tag, flag). tag is a debug string, flag is 0.
    constexpr uintptr_t kGetObjectByGuid = 0x004D4DB0;
    // Active player GUID ().
    constexpr uintptr_t kActivePlayerGuid = 0x004D3790;
    // Reaction of self toward other (this-in-ECX): 0..1 hostile, 2..3 neutral, 4+ friendly.
    constexpr uintptr_t kUnitReaction = 0x007251C0;

    // --- object field offsets ---
    constexpr size_t kUnitModelField   = 0xB4; // unit object -> body model
    constexpr size_t kModelParentField = 0x48; // model -> parent model (0 = root)

    // --- type masks ---
    constexpr uint32_t kTypeMaskUnit   = 0x08;
    constexpr uint32_t kTypeMaskPlayer = 0x10;

    // --- signatures ---
    using GetObjectFn        = void*(__cdecl*)(unsigned long long guid, unsigned typemask,
                                               const char* tag, int flag);
    using ActivePlayerGuidFn = unsigned long long(__cdecl*)();
    using ReactionFn         = int(__fastcall*)(void* self, void* edx, void* other);
}
