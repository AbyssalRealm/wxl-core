// camera bindings: the live world view / projection matrices and camera position.
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
#include "offsets/engine/Camera.hpp"

// The live matrices feed a 3D gizmo, screen<->world projection, and cursor ray picking.
// Pointers alias the engine globals directly; valid only in-world.
namespace wxl::game::camera
{
    namespace off = wxl::offsets::engine::camera;

    // float[16], row-major (D3D row-vector). World -> view.
    inline const float* View()       { return reinterpret_cast<const float*>(off::kView); }
    // float[16], row-major.
    inline const float* Projection() { return reinterpret_cast<const float*>(off::kProjection); }
    // float[16], row-major. View * Projection.
    inline const float* ViewProj()   { return reinterpret_cast<const float*>(off::kViewProj); }

    inline void Position(float out[3])
    {
        const float* p = reinterpret_cast<const float*>(off::kCameraPos);
        out[0] = p[0]; out[1] = p[1]; out[2] = p[2];
    }

    inline void RegisterCatalog()
    {
        Register({ "Camera::View",       off::kView,       "const float[16] (world->view)" });
        Register({ "Camera::Projection", off::kProjection, "const float[16]" });
        Register({ "Camera::ViewProj",   off::kViewProj,   "const float[16]" });
    }
}
