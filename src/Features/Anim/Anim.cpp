#include "Features/Anim/Anim.hpp"

#include "Engine/Offsets.hpp"
#include "Core/Hook.hpp"
#include "Core/Logger.hpp"

#include <windows.h>
#include <cstdint>
#include <cstring>

using namespace wraith;

namespace
{
    using AnimLoadCompleteFn = void(__cdecl*)(void* node);
    AnimLoadCompleteFn g_animLoadCompleteOriginal = nullptr;

    // The I/O record holds the just-read .anim buffer and its size: the exact base and size the native rebase
    // consumes. The buffer was sized to the full 271+ file, so stripping the 8-byte header leaves it large
    // enough; every offset the .m2 declares lands inside the payload.
    void StripAFM2(void* record)
    {
        auto* recBytes = static_cast<uint8_t*>(record);
        auto* buffer = *reinterpret_cast<uint8_t**>(recBytes + offsets::AnimRecord_Buffer);
        auto* sizeField = reinterpret_cast<uint32_t*>(recBytes + offsets::AnimRecord_Size);
        if (!buffer || *sizeField < features::anim::kAFM2HeaderSize) return;
        if (*reinterpret_cast<uint32_t*>(buffer) != features::anim::kMagicAFM2) return;  // already raw .anim

        uint32_t payloadSize = *sizeField - features::anim::kAFM2HeaderSize;
        memmove(buffer, buffer + features::anim::kAFM2HeaderSize, payloadSize);
        *sizeField = payloadSize;
    }

    // SEH-guarded: derefs the I/O record and raw file bytes; a malformed .anim must not crash the loader.
    void StripAFM2Guarded(void* record)
    {
        if (!record) return;
        __try { StripAFM2(record); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }

    // .anim read-completion: the buffer is filled but not yet offset-rebased. Strip a 271+ AFM2 header in
    // place so the native rebase treats the payload as raw (offsets relative to offset 0), then proceed.
    void __cdecl AnimLoadCompleteDetour(void* node)
    {
        void* record = *reinterpret_cast<void**>(static_cast<uint8_t*>(node) + offsets::AnimNode_Record);
        StripAFM2Guarded(record);
        g_animLoadCompleteOriginal(node);
    }
}

namespace wraith::features::anim
{
    void Install()
    {
        hook::Install("CM2::AnimLoadComplete", offsets::CM2_AnimLoadComplete,
                      &AnimLoadCompleteDetour, reinterpret_cast<void**>(&g_animLoadCompleteOriginal));
        WLOG_INFO("Anim: installed");
    }
}
