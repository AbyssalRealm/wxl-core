// doodad bindings: enumerate placed map doodads near the player and read / move their transform.
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
#include <windows.h>

#include "game/Binding.hpp"
#include "offsets/game/ADT.hpp"
#include "offsets/game/Doodad.hpp"

// Walks the placed-doodad list of the player's current chunk and reads or moves a doodad's world placement. 
// Pointer reads are range-guarded and the walk is bounded, so a stale/odd list degrades to a short list instead of a crash.
namespace wxl::game::doodad
{
    namespace off  = wxl::offsets::game::doodad;
    namespace aoff = wxl::offsets::game::adt;

    namespace detail
    {
        // Coarse user-space pointer sanity (the client is a 32-bit fixed-base image).
        inline bool Plausible(const void* p)
        {
            const uintptr_t a = reinterpret_cast<uintptr_t>(p);
            return a > 0x10000 && a < 0x7FFF0000;
        }

        // [p, p+n) is committed and accessible with one of the given page protections. Used to keep an
        // imperfect list walk from ever dereferencing an unmapped address.
        inline bool Accessible(const void* p, size_t n, DWORD allow)
        {
            if (!Plausible(p)) return false;
            MEMORY_BASIC_INFORMATION mbi;
            if (VirtualQuery(p, &mbi, sizeof mbi) == 0) return false;
            if (mbi.State != MEM_COMMIT) return false;
            if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) return false;
            if ((mbi.Protect & allow) == 0) return false;
            const uintptr_t end = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
            return reinterpret_cast<uintptr_t>(p) + n <= end;
        }
        inline bool Readable(const void* p, size_t n)
        {
            return Accessible(p, n, PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);
        }
        inline bool Writable(const void* p, size_t n)
        {
            return Accessible(p, n, PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);
        }

        inline float* F(void* d, size_t o) { return reinterpret_cast<float*>(reinterpret_cast<char*>(d) + o); }
        // Guarded pointer read: returns null if the slot is not mapped (instead of crashing).
        inline void* P(void* d, size_t o)
        {
            void* slot = reinterpret_cast<char*>(d) + o;
            return Readable(slot, sizeof(void*)) ? *reinterpret_cast<void**>(slot) : nullptr;
        }

        // A finite world coordinate within the map bounds (rejects NaN / inf / absurd via self-compare).
        inline bool SaneCoord(float v) { return v == v && v > -64000.0f && v < 64000.0f; }
        // The candidate points at a doodad whose position block is mapped and reads as a sane location.
        inline bool SaneDoodad(void* d)
        {
            return Readable(d, off::kPosZ + sizeof(float)) && SaneCoord(*F(d, off::kPosX))
                && SaneCoord(*F(d, off::kPosY)) && SaneCoord(*F(d, off::kPosZ));
        }
    }

    // A still-live doodad: its position block is mapped and reads as a sane world location.
    inline bool IsValid(void* d) { return detail::SaneDoodad(d); }

    // The CM2 render instance the doodad draws through (doodad+0x34), or null while the model is still
    // loading. The live world matrix the renderer reads each frame lives on this instance, not the doodad.
    inline void* Instance(void* d) { return detail::P(d, off::kInstance); }

    // The model file path of a placed doodad: instance(+0x34) -> CM2Model(+0x2c) -> inline path(+0x3c).
    // Copies the bare filename (or full path if no separator) into out; returns false if not yet loaded.
    inline bool ModelName(void* d, char* out, size_t cap)
    {
        if (!out || cap == 0) return false;
        out[0] = '\0';
        void* inst = Instance(d);
        if (!inst) return false;
        void* model = detail::P(inst, off::kInstModel);
        if (!model) return false;
        const char* path = reinterpret_cast<const char*>(model) + off::kModelFullPath;
        if (!detail::Readable(path, 1)) return false;
        // Find the last path separator so we show just the file name.
        const char* name = path;
        for (const char* s = path; detail::Readable(s, 1) && *s && (s - path) < 512; ++s)
            if (*s == '\\' || *s == '/') name = s + 1;
        size_t i = 0;
        for (; i + 1 < cap && detail::Readable(name + i, 1) && name[i]; ++i) out[i] = name[i];
        out[i] = '\0';
        return i > 0;
    }

    // The runtime chunk under a world position, or null.
    inline void* ChunkAt(float pos[3]) { return Native<aoff::Map_GetChunkFn>(aoff::kGetChunk)(pos); }

    // Append one chunk's placed doodads to out[] starting at index n; returns the new count. Walks the
    // intrusive list (head @ +0xCC, next @ node + linkOff + 4), every read guarded so a bad list stops
    // instead of crashing. Only CMapDoodads are in this list, so creatures never appear.
    inline int EnumerateChunk(void* chunk, void** out, int n, int maxCount)
    {
        if (!detail::Readable(chunk, off::kChunkDoodadHead + sizeof(void*))) return n;
        const uint32_t linkOff =
            *reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(chunk) + off::kChunkDoodadLinkOff);
        if (linkOff > 0x400) return n; // a real TSLink offset is small; reject a garbage field
        uintptr_t node = reinterpret_cast<uintptr_t>(detail::P(chunk, off::kChunkDoodadHead));
        for (int i = 0; (node & 1) == 0 && node != 0 && i < 8192 && n < maxCount; ++i)
        {
            void* d = detail::P(reinterpret_cast<void*>(node), off::kNodeDoodad);
            if (detail::SaneDoodad(d)) out[n++] = d;
            node = reinterpret_cast<uintptr_t>(detail::P(reinterpret_cast<void*>(node), linkOff + 4));
        }
        return n;
    }

    // Doodads of the chunk under a single world position.
    inline int EnumerateAt(float pos[3], void** out, int maxCount)
    {
        return EnumerateChunk(ChunkAt(pos), out, 0, maxCount);
    }

    // Doodads of the 3x3 grid of chunks around a center, so a cursor pick finds doodads in the
    // neighbouring chunks too. step ~= one chunk (33.33 yd). Returns the count.
    inline int EnumerateAround(float center[3], float step, void** out, int maxCount)
    {
        void* seen[16]; int seenN = 0, n = 0;
        for (int gy = -1; gy <= 1; ++gy)
            for (int gx = -1; gx <= 1; ++gx)
            {
                float q[3] = { center[0] + gx * step, center[1] + gy * step, center[2] };
                void* chunk = ChunkAt(q);
                if (!detail::Plausible(chunk)) continue;
                bool dup = false;
                for (int k = 0; k < seenN; ++k) if (seen[k] == chunk) dup = true;
                if (dup) continue;
                if (seenN < 16) seen[seenN++] = chunk;
                n = EnumerateChunk(chunk, out, n, maxCount);
                if (n >= maxCount) return n;
            }
        return n;
    }

    inline void Position(void* d, float out[3])
    {
        if (!detail::Readable(d, off::kScale + sizeof(float))) { out[0] = out[1] = out[2] = 0.0f; return; }
        out[0] = *detail::F(d, off::kPosX);
        out[1] = *detail::F(d, off::kPosY);
        out[2] = *detail::F(d, off::kPosZ);
    }

    inline float Scale(void* d)
    {
        return detail::Readable(d, off::kScale + sizeof(float)) ? *detail::F(d, off::kScale) : 1.0f;
    }

    // The cursor-pick target: the world bounding-sphere center, falling back to the origin if that
    // field reads unmapped or implausibly far from the origin.
    inline void Center(void* d, float out[3])
    {
        float pos[3];
        Position(d, pos);
        if (detail::Readable(d, off::kCenterZ + sizeof(float)))
        {
            const float c0 = *detail::F(d, off::kCenterX);
            const float c1 = *detail::F(d, off::kCenterY);
            const float c2 = *detail::F(d, off::kCenterZ);
            const float dx = c0 - pos[0], dy = c1 - pos[1], dz = c2 - pos[2];
            if (detail::SaneCoord(c0) && detail::SaneCoord(c1) && detail::SaneCoord(c2) &&
                dx * dx + dy * dy + dz * dz < 300.0f * 300.0f)
            {
                out[0] = c0; out[1] = c1; out[2] = c2; return;
            }
        }
        out[0] = pos[0]; out[1] = pos[1]; out[2] = pos[2];
    }

    // The doodad's world-space bounding box (min, max). Returns false if the block is not mapped.
    inline bool BBox(void* d, float mn[3], float mx[3])
    {
        if (!detail::Readable(d, off::kBBoxMaxZ + sizeof(float))) return false;
        mn[0] = *detail::F(d, off::kBBoxMinX); mn[1] = *detail::F(d, off::kBBoxMinY); mn[2] = *detail::F(d, off::kBBoxMinZ);
        mx[0] = *detail::F(d, off::kBBoxMaxX); mx[1] = *detail::F(d, off::kBBoxMaxY); mx[2] = *detail::F(d, off::kBBoxMaxZ);
        return true;
    }

    // The live world matrix the renderer reads each frame: instance(+0x34) + 0xb4 (16 floats, row-major
    // D3D row-vector, translation in row 3 = m[12..14]). False if the model is not loaded / not mapped.
    inline bool WorldMatrix(void* d, float m[16])
    {
        void* inst = Instance(d);
        if (!inst) return false;
        const float* src = detail::F(inst, off::kInstWorldMatrix);
        if (!detail::Readable(src, 16 * sizeof(float))) return false;
        for (int i = 0; i < 16; ++i) m[i] = src[i];
        return true;
    }

    // Replace the whole world transform from a gizmo-edited matrix. The render-critical write is the CM2
    // instance matrix (+0xb4); the doodad-side staging copies (+0xd8 / +0x6c) are mirrored so the editor's
    // source of truth and any later save stay consistent. Without the instance write nothing moves.
    inline void SetWorldMatrix(void* d, const float m[16])
    {
        void* inst = Instance(d);
        if (inst && detail::Writable(detail::F(inst, off::kInstWorldMatrix), 16 * sizeof(float)))
            for (int i = 0; i < 16; ++i) detail::F(inst, off::kInstWorldMatrix)[i] = m[i];

        if (detail::Writable(detail::F(d, off::kWorldMatrix), 16 * sizeof(float)))
            for (int i = 0; i < 16; ++i) detail::F(d, off::kWorldMatrix)[i] = m[i];
        if (detail::Writable(detail::F(d, off::kPosX), 3 * sizeof(float)))
        {
            *detail::F(d, off::kPosX) = m[12];
            *detail::F(d, off::kPosY) = m[13];
            *detail::F(d, off::kPosZ) = m[14];
        }
    }

    // Move a doodad (translate only): rewrite the translation row of the live instance matrix, mirror it into
    // the doodad staging fields. The instance write is what actually moves the model.
    inline void SetPosition(void* d, const float p[3])
    {
        void* inst = Instance(d);
        if (inst && detail::Writable(detail::F(inst, off::kInstTransX), 3 * sizeof(float)))
        {
            *detail::F(inst, off::kInstTransX) = p[0];
            *detail::F(inst, off::kInstTransY) = p[1];
            *detail::F(inst, off::kInstTransZ) = p[2];
        }
        if (detail::Writable(detail::F(d, off::kPosX), 3 * sizeof(float)))
        {
            *detail::F(d, off::kPosX) = p[0];
            *detail::F(d, off::kPosY) = p[1];
            *detail::F(d, off::kPosZ) = p[2];
        }
        if (detail::Writable(detail::F(d, off::kWorldMatrixTransX), 3 * sizeof(float)))
        {
            *detail::F(d, off::kWorldMatrixTransX) = p[0];
            *detail::F(d, off::kWorldMatrixTransY) = p[1];
            *detail::F(d, off::kWorldMatrixTransZ) = p[2];
        }
    }

    inline void RegisterCatalog()
    {
        Register({ "Doodad::ChunkAt", aoff::kGetChunk, "void*(float pos[3]) - chunk holding doodads" });
    }
}
