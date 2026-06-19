// world bindings: selection GUIDs and GUID -> object resolution.
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
#include "offsets/game/Unit.hpp"

// The current selection and the GUID lookup, as typed calls.
namespace wxl::game::world
{
    namespace off = wxl::offsets::game::unit;

    inline unsigned long long MouseoverGuid()
    { return *reinterpret_cast<unsigned long long*>(off::kMouseoverGuid); }

    inline unsigned long long TargetGuid()
    { return *reinterpret_cast<unsigned long long*>(off::kTargetGuid); }

    inline unsigned long long ActivePlayerGuid()
    { return Native<off::ActivePlayerGuidFn>(off::kActivePlayerGuid)(); }

    // Resolve a GUID to an object filtered by type mask, or null.
    inline void* GetObject(unsigned long long guid, unsigned typeMask)
    { return Native<off::GetObjectFn>(off::kGetObjectByGuid)(guid, typeMask, "wxl", 0); }

    constexpr unsigned kTypeMaskUnit   = off::kTypeMaskUnit;
    constexpr unsigned kTypeMaskPlayer = off::kTypeMaskPlayer;
}
