// World camera matrix globals and the per-frame matrix builder address.
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

// INTERNAL to the core. The world camera matrices, as static globals (the image is fixed-base, no
// ASLR, so they are read directly). float[16], row-major, D3D row-vector convention. They are valid
// only in-world: at the login / loading screen they sit at identity with the camera at the origin.
namespace wxl::offsets::engine::camera
{
    constexpr uintptr_t kProjection = 0x00ADF628;
    constexpr uintptr_t kView       = 0x00ADF5E8; // world -> view
    constexpr uintptr_t kViewProj   = 0x00ADF460;
    constexpr uintptr_t kCameraPos  = 0x00CD8F5C; // vec3

    // Per-frame builder that recomputes the view/projection from the camera state.
    constexpr uintptr_t kBuildCameraMatrices = 0x00795400;
}
