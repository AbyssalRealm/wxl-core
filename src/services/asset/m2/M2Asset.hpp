// M2Asset: the Tier-1 M2 conversion primitives (version-agnostic, parameterized, pure byte).
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
#include <span>

#include "services/asset/Asset.hpp"
#include "services/asset/Resolver.hpp"

namespace wxl::asset
{
    class M2Asset : public Asset
    {
    public:
        explicit M2Asset(std::span<const uint8_t> source);

        // --- probes (read-only, drive a handler's Claims/recipe) ---
        bool     IsContainer() const;  // outer container magic present
        uint32_t InnerVersion() const; // inner model version, 0 if not a model

        // --- Tier-1 primitives ---
        // Unwrap the container down to the inner self-contained model. False if not a container.
        bool UnwrapContainer();

        // Compact each source record layout onto the client stride, in place (forward slide; the dst
        // stride is <= the src stride, so a record is never overwritten before it is read).
        void CompactCameras();
        void CompactParticles();
        void CompactRibbons();

        // Mask every sequence's blendTime to its low u16. The source packs in|out into that u32; read
        // whole by the client it is a huge duration and transitions never complete.
        void MaskSequenceBlendTimes();

        // Remap one sequence id and repoint the sequence lookup. MECHANISM ONLY: the set of
        // (fromId -> toId) pairs is policy and belongs to the module recipe, not here.
        void RemapSequenceId(uint16_t fromId, uint16_t toId);

        // Resolve the externalized texture FileDataIDs and inline them back as filenames on the model
        // tail, so the native loader finds its textures. No-op when the model carries no id table.
        bool ReinlineTextures(const ResolveCtx& rc);
    };
}
