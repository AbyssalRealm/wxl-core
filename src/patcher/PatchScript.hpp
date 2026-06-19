// PatchScript: the base a patcher script subclasses to declare static PE edits.
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

#include <span>

#include "patcher/PeImage.hpp"

// A patcher script subclasses PatchScript and is registered once by its file-scope ctor (the same
// self-registration as the runtime/host scripts). The patcher runs every registered script against the
// PE. This is how the dozens of separate third-party `.exe` patchers collapse into drop-in scripts.
namespace wxl::patcher
{
    class PatchScript
    {
    public:
        virtual ~PatchScript() = default;

        // Short identifier, for the patcher log.
        virtual const char* name() const = 0;

        // Apply this script's edits to the PE. Return false to abort the whole patch run.
        virtual bool Apply(PeImage& pe) const = 0;

    protected:
        PatchScript(); // auto-registers
    };

    namespace registry
    {
        void Add(PatchScript* script);
        std::span<PatchScript* const> Scripts();
    }
}
