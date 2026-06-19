// ui bindings: show / hide the entire game interface (single engine flag).
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
#include "offsets/engine/Ui.hpp"

// Hides the FrameXML layer for a clean editor view while the 3D world keeps drawing.
// Restore to shown before anything that expects the UI (e.g. a reload).
namespace wxl::game::ui
{
    namespace off = wxl::offsets::engine::ui;

    // The render-enable flag, or null before the UI root exists.
    inline int* Flag()
    {
        void* root = *reinterpret_cast<void**>(off::kUiRootPtr);
        return root ? reinterpret_cast<int*>(reinterpret_cast<char*>(root) + off::kUiEnabledFlag) : nullptr;
    }

    inline bool IsShown()      { int* f = Flag(); return f ? (*f != 0) : true; }
    inline void Show(bool on)  { int* f = Flag(); if (f) *f = on ? 1 : 0; }

    inline void RegisterCatalog()
    {
        Register({ "UI::Show", off::kUiRootPtr, "void(bool) via interface-root flag" });
    }
}
