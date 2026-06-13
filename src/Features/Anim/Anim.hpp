#pragma once

#include <cstdint>

// 271+ external .anim support. A 271+ .anim is a single chunk [u32 'AFM2'][u32 size][payload], where the
// payload is byte-for-byte the raw 264 .anim layout and size == filesize-8. The 264 loader resolves M2Track
// offsets from the loaded buffer base, but 271+ stores them relative to the chunk data (file+8), so the
// 8-byte header shifts every keyframe and animations read garbage. At read-completion, before the rebase, the
// header is stripped in place (payload moved back 8 bytes, size -= 8) so offsets line up. Files without the
// AFM2 magic are already raw and left untouched.
namespace wraith::features::anim
{
    // AFM2 chunk magic, 'A''F''M''2' little-endian (bytes 41 46 4D 32).
    constexpr uint32_t kMagicAFM2 = 0x324D4641;
    // Chunk header size: u32 magic + u32 size.
    constexpr uint32_t kAFM2HeaderSize = 8;

    void Install();
}
