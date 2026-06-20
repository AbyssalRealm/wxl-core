// Model load / animation / batch-alpha / ribbon entry addresses, signatures, and object field offsets.
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

// INTERNAL to the core. Client addresses and runtime object field offsets. This is the private
// SOURCE the game-binding catalog is curated from; modules never include it, they call wxl::game.
namespace wxl::offsets::game::m2
{
    // --- load / setup ---
    // Model init: parses a model file and builds the runtime model.
    constexpr uintptr_t kInit = 0x0083CF00;
    // Skin-profile finalizer: runs once after the skin sub-arrays resolve and before the shader passes
    // size their batch blocks. The point to rebuild the material contract a modern skin omits.
    constexpr uintptr_t kFinalizeSkin = 0x00837A40;

    // Version-gate branches in the loader. The stock loader accepts only one inner version; these are
    // the two compare branches that reject higher inner versions.
    constexpr uintptr_t kVersionGateInit = 0x0083CF51; // version-too-high branch
    constexpr uintptr_t kVersionGateAnim = 0x0083C745; // anim-parse version branch

    // .skin filename builder (pathStem, profileIndex, outBuf): copies the path, strips the extension,
    // appends the two-digit skin suffix. outBuf is a fixed-size engine buffer.
    constexpr uintptr_t kBuildSkinPath = 0x00835A80;
    // Skin-profile loader (model, profileIndex): builds the path, opens+maps the file, parses it,
    // attaches the profile synchronously, and schedules an idempotent async finalize.
    constexpr uintptr_t kLoadSkinProfile = 0x0083CB40;
    // Size of the engine sibling-file path buffer the builders write into.
    constexpr uint32_t  kSkinPathBufSize = 0x108;

    // --- external animation ---
    // External-anim read-completion callback (node): runs once after the bytes are read and before the
    // per-sequence track offsets are rebased.
    constexpr uintptr_t kAnimLoadComplete = 0x0083D840;
    // External-anim loader (model, seqIdx): resolves the sequence alias chain, builds the path, opens
    // the file, allocates a buffer, and schedules the async read whose completion rebases the tracks.
    constexpr uintptr_t kSequenceLoad = 0x0083DA10;
    // .anim filename builder (pathStem, id, subId, outBuf): copies the stem, strips the extension,
    // appends the id-subId anim suffix.
    constexpr uintptr_t kBuildAnimPath = 0x00835A20;
    // Per-sequence track de-relocator (model, seqIdx, buffer, size): validates the buffer and rebases
    // sequence seqIdx's track inner slots against it, then updates the sequence flags.
    constexpr uintptr_t kPerSeqDeReloc = 0x0083C6E0;
    // M2 buffer allocator (size, name, line): allocates size+0x10, returns a 16-aligned pointer carrying a
    // back-shift byte at [ptr-1]. This is the allocator the .m2 load buffer (model+0x150) uses, so a
    // replacement buffer must come from here for the model destructor's matching free to be valid.
    constexpr uintptr_t kAnimBufferAlloc = 0x0083DE50;
    constexpr uintptr_t kBufferAlloc     = 0x0083DE50; // alias: same allocator, used for buffer swaps
    constexpr uintptr_t kBufferFree      = 0x0083DE90; // free a kBufferAlloc pointer (recovers base via [ptr-1])
    using M2_BufferAllocFn = void*(__cdecl*)(uint32_t size, const char* tag, int line);
    using M2_BufferFreeFn  = void (__cdecl*)(void* ptr);

    // I/O record field offsets used by the rebase: the buffer base and its byte size.
    constexpr size_t kOffRecordBuffer = 0x04;
    constexpr size_t kOffRecordSize   = 0x08;
    // Load-node field: the I/O record pointer.
    constexpr size_t kOffNodeRecord   = 0x08;

    // --- per-batch alpha ---
    // Shared per-batch alpha/material/cull setter: chooses the alpha-test reference from the blend mode
    // and pushes it to the device.
    constexpr uintptr_t kSetupBatchAlpha = 0x0081FE90;
    // Pushes the alpha-test reference to the device.
    constexpr uintptr_t kPushAlphaRef = 0x00873BA0;

    // SetupBatchAlpha draw-context fields (this = the draw context): the instance being drawn and the live
    // material the caller set. Instance -> model is kOffInstModel below.
    constexpr size_t kOffDrawCtxInstance = 0x60; // draw context -> render instance
    constexpr size_t kOffDrawCtxMaterial = 0x98; // draw context -> live material record
    // Material record: the blend mode (1 = alpha key).
    constexpr size_t kOffMaterialBlend   = 0x02;

    // --- bone palette (per-frame skinning matrices; the bone-physics hook point) ---
    // Per-instance bone-palette build (instance, ...): fills the bone matrices for one instance from
    // the current pose, each frame, before the batch draw uploads the palette to the vertex shader.
    constexpr uintptr_t kBuildBonePalette = 0x0082F0F0;

    // --- track evaluators (sampled per bone / per light each frame from the bone-palette build) ---
    // Vec3 track evaluator (model, runtimeBone, track, out, baseValue): samples a translation/scale
    // track for the current animation index and writes the interpolated vec3 into out.
    constexpr uintptr_t kTrackEvalVec3 = 0x0082B0A0;
    // Quaternion track evaluator (model, runtimeBone, track, out, baseValue): samples a rotation track
    // for the current animation index and writes the interpolated quaternion into out.
    constexpr uintptr_t kTrackEvalQuat = 0x00828680;

    // --- ribbon ---
    // Ribbon-emitter de-relocator: pointer-fixes each ribbon emitter's sub-array offsets.
    constexpr uintptr_t kRibbonDeRelocate = 0x0083A460;
    // Ribbon emitter draw (emitter, stateBlock): builds the strip and binds one texture per layer.
    constexpr uintptr_t kRibbonDraw = 0x00980B70;
    // Resolve a texture handle to the internal texture object the sampler bind expects.
    constexpr uintptr_t kTexResolve = 0x004B6CB0;
    // Bind a texture to a sampler selector (device, selector, resolvedTexture).
    constexpr uintptr_t kSamplerBind = 0x00685F50;
    // Sampler selectors for the engine bind path: s0 = 0x15, consecutive. The native ribbon loop binds
    // only s0; the extra layers of a multi-texture ribbon are bound to s1/s2 so they survive one pass.
    constexpr uint32_t kSamplerSelS1 = 0x16;
    constexpr uint32_t kSamplerSelS2 = 0x17;

    // --- runtime instance object fields ---
    constexpr size_t kOffInstModel       = 0x2C; // -> runtime model
    constexpr size_t kOffInstBonePalette = 0x98; // -> bone matrices, row-major 4x4
    constexpr size_t kBonePaletteStride  = 0x40; // one bone matrix

    // --- runtime model object fields ---
    constexpr size_t kOffModelFlags    = 0x08;  // bit 2 selects the sibling-file open flag
    constexpr size_t kOffModelPathStem = 0x3C;  // model path stem (no extension)
    constexpr size_t kOffModelHeader   = 0x150; // -> raw .m2 file buffer (parsed in place -> becomes the header)
    constexpr size_t kOffModelFileSize = 0x16C; // byte size of the .m2 file buffer at +0x150
    constexpr size_t kOffModelSkin     = 0x170; // -> live parsed skin profile (valid at/after skin finalize)

    // --- parsed file-header fields ---
    constexpr size_t kOffHdrGlobalFlags = 0x10; // bit 0x20 = model carries physics
    constexpr size_t kOffHdrBoneCount   = 0x2C;
    constexpr size_t kOffHdrBoneArray   = 0x30; // -> bone records (post-fixup data ptr)

    // --- bone record fields (in the header bone array) ---
    constexpr size_t   kBoneStride     = 0x58;
    constexpr size_t   kOffBoneFlags   = 0x04; // bone flags
    constexpr size_t   kOffBoneParent  = 0x08; // int16 parent index (0xFFFF = root)
    constexpr size_t   kOffBonePivot   = 0x4C; // pivot (bone origin in bind space)
    constexpr uint32_t kBoneBillboardMask = 0x78; // spherical + cylindrical-lock bits

    // --- track object fields read by the evaluators ---
    constexpr size_t kOffTrackTimestampsCount = 0x04;
    constexpr size_t kOffTrackTimestampsPtr   = 0x08;
    constexpr size_t kOffTrackValuesCount     = 0x0C;
    constexpr size_t kOffTrackValuesPtr       = 0x10;
    constexpr size_t kTrackTimestampStride    = 0x04; // one timestamp = u32 ms
    constexpr size_t kTrackVec3Stride         = 0x0C; // one vec3 value = 3 floats
    constexpr size_t kTrackQuatStride         = 0x08; // one compressed quat = 4 int16
    // Runtime-bone field: the current animation index used to pick the per-animation inner slot.
    constexpr size_t kOffRuntimeBoneAnimIdx   = 0x44;

    // --- ribbon-emitter object fields ---
    constexpr size_t kOffRibbonLayerCount   = 0x118; // draw-loop bound
    constexpr size_t kOffRibbonTexHandlePtr = 0x12C; // -> per-layer texture-handle array (stride 4)

    // --- typed views over the objects above ---
    // The constants are the curated landmarks; these structs give named, typed access to the same fields,
    // with every member offset checked against a constant at compile time (a wrong padding fails the build).
    // Only RE'd fields are named; the gaps are explicit padding. Pointers are 4 bytes on the 32-bit client.
#pragma pack(push, 1)
    /** @brief I/O record read by the per-sequence rebase: the loaded buffer base and its byte size. */
    struct IoRecord
    {
        uint8_t  _pad00[kOffRecordBuffer];
        void*    buffer;           // kOffRecordBuffer (loaded bytes base)
        uint32_t size;             // kOffRecordSize (byte count)
    };
    static_assert(offsetof(IoRecord, buffer) == kOffRecordBuffer, "IoRecord.buffer");
    static_assert(offsetof(IoRecord, size)   == kOffRecordSize,   "IoRecord.size");

    /** @brief Async load node: holds the I/O record pointer. */
    struct LoadNode
    {
        uint8_t  _pad00[kOffNodeRecord];
        void*    record;           // kOffNodeRecord -> IoRecord
    };
    static_assert(offsetof(LoadNode, record) == kOffNodeRecord, "LoadNode.record");

    /** @brief SetupBatchAlpha draw context: the instance being drawn and the live material. */
    struct DrawContext
    {
        uint8_t  _pad00[kOffDrawCtxInstance];
        void*    instance;         // kOffDrawCtxInstance -> M2Instance
        uint8_t  _pad64[kOffDrawCtxMaterial - (kOffDrawCtxInstance + sizeof(void*))];
        void*    material;         // kOffDrawCtxMaterial -> Material
    };
    static_assert(offsetof(DrawContext, instance) == kOffDrawCtxInstance, "DrawContext.instance");
    static_assert(offsetof(DrawContext, material) == kOffDrawCtxMaterial, "DrawContext.material");

    /** @brief Live material record: the blend mode the draw uses to pick the alpha-test reference. */
    struct Material
    {
        uint8_t  _pad00[kOffMaterialBlend];
        uint16_t blend;            // kOffMaterialBlend (1 = alpha key)
    };
    static_assert(offsetof(Material, blend) == kOffMaterialBlend, "Material.blend");

    /** @brief Runtime instance: the model it draws and the per-frame bone-matrix palette. */
    struct M2Instance
    {
        uint8_t  _pad00[kOffInstModel];
        void*    model;            // kOffInstModel -> M2Model
        uint8_t  _pad30[kOffInstBonePalette - (kOffInstModel + sizeof(void*))];
        float    bonePalette[1];   // kOffInstBonePalette (bone matrices, row-major 4x4, kBonePaletteStride each)
    };
    static_assert(offsetof(M2Instance, model)       == kOffInstModel,       "M2Instance.model");
    static_assert(offsetof(M2Instance, bonePalette) == kOffInstBonePalette, "M2Instance.bonePalette");

    /** @brief Runtime model: flags, path stem, the parsed .m2 buffer, its size, and the live skin profile. */
    struct M2Model
    {
        uint8_t  _pad00[kOffModelFlags];
        uint32_t flags;            // kOffModelFlags (bit 2 selects the sibling-file open flag)
        uint8_t  _pad0c[kOffModelPathStem - (kOffModelFlags + sizeof(uint32_t))];
        char     pathStem[kOffModelHeader - kOffModelPathStem]; // kOffModelPathStem (inline path stem, no extension)
        void*    header;           // kOffModelHeader -> raw .m2 file buffer (parsed in place)
        uint8_t  _pad154[kOffModelFileSize - (kOffModelHeader + sizeof(void*))];
        uint32_t fileSize;         // kOffModelFileSize (byte size of the buffer at kOffModelHeader)
        void*    skin;             // kOffModelSkin -> live parsed skin profile
    };
    static_assert(offsetof(M2Model, flags)    == kOffModelFlags,    "M2Model.flags");
    static_assert(offsetof(M2Model, pathStem) == kOffModelPathStem, "M2Model.pathStem");
    static_assert(offsetof(M2Model, header)   == kOffModelHeader,   "M2Model.header");
    static_assert(offsetof(M2Model, fileSize) == kOffModelFileSize, "M2Model.fileSize");
    static_assert(offsetof(M2Model, skin)     == kOffModelSkin,     "M2Model.skin");

    /** @brief Parsed file header: the global flags and the post-fixup bone array. */
    struct M2FileHeader
    {
        uint8_t  _pad00[kOffHdrGlobalFlags];
        uint32_t globalFlags;      // kOffHdrGlobalFlags (bit 0x20 = model carries physics)
        uint8_t  _pad14[kOffHdrBoneCount - (kOffHdrGlobalFlags + sizeof(uint32_t))];
        uint32_t boneCount;        // kOffHdrBoneCount
        void*    boneArray;        // kOffHdrBoneArray -> M2Bone records (post-fixup data ptr)
    };
    static_assert(offsetof(M2FileHeader, globalFlags) == kOffHdrGlobalFlags, "M2FileHeader.globalFlags");
    static_assert(offsetof(M2FileHeader, boneCount)   == kOffHdrBoneCount,   "M2FileHeader.boneCount");
    static_assert(offsetof(M2FileHeader, boneArray)   == kOffHdrBoneArray,   "M2FileHeader.boneArray");

    /** @brief Bone record in the header bone array (stride kBoneStride). */
    struct M2Bone
    {
        uint8_t  _pad00[kOffBoneFlags];
        uint32_t flags;            // kOffBoneFlags
        int16_t  parent;           // kOffBoneParent (0xFFFF = root)
        uint8_t  _pad0a[kOffBonePivot - (kOffBoneParent + sizeof(int16_t))];
        float    pivot[3];         // kOffBonePivot (bone origin in bind space)
    };
    static_assert(offsetof(M2Bone, flags)  == kOffBoneFlags,  "M2Bone.flags");
    static_assert(offsetof(M2Bone, parent) == kOffBoneParent, "M2Bone.parent");
    static_assert(offsetof(M2Bone, pivot)  == kOffBonePivot,  "M2Bone.pivot");

    /** @brief Track object read by the evaluators: the timestamp and value sub-arrays (count + ptr each). */
    struct M2Track
    {
        uint8_t   _pad00[kOffTrackTimestampsCount];
        uint32_t  timestampsCount; // kOffTrackTimestampsCount
        void*     timestampsPtr;   // kOffTrackTimestampsPtr
        uint32_t  valuesCount;     // kOffTrackValuesCount
        void*     valuesPtr;       // kOffTrackValuesPtr
    };
    static_assert(offsetof(M2Track, timestampsCount) == kOffTrackTimestampsCount, "M2Track.timestampsCount");
    static_assert(offsetof(M2Track, timestampsPtr)   == kOffTrackTimestampsPtr,   "M2Track.timestampsPtr");
    static_assert(offsetof(M2Track, valuesCount)     == kOffTrackValuesCount,     "M2Track.valuesCount");
    static_assert(offsetof(M2Track, valuesPtr)       == kOffTrackValuesPtr,       "M2Track.valuesPtr");

    /** @brief Runtime bone state: the current animation index used to pick the per-animation inner slot. */
    struct RuntimeBone
    {
        uint8_t  _pad00[kOffRuntimeBoneAnimIdx];
        uint32_t animIndex;        // kOffRuntimeBoneAnimIdx
    };
    static_assert(offsetof(RuntimeBone, animIndex) == kOffRuntimeBoneAnimIdx, "RuntimeBone.animIndex");

    /** @brief Ribbon emitter: the draw-loop layer count and the per-layer texture-handle array pointer. */
    struct RibbonEmitter
    {
        uint8_t  _pad00[kOffRibbonLayerCount];
        uint32_t layerCount;       // kOffRibbonLayerCount (draw-loop bound)
        uint8_t  _pad11c[kOffRibbonTexHandlePtr - (kOffRibbonLayerCount + sizeof(uint32_t))];
        void**   texHandles;       // kOffRibbonTexHandlePtr -> per-layer texture-handle array (stride 4)
    };
    static_assert(offsetof(RibbonEmitter, layerCount) == kOffRibbonLayerCount,   "RibbonEmitter.layerCount");
    static_assert(offsetof(RibbonEmitter, texHandles) == kOffRibbonTexHandlePtr, "RibbonEmitter.texHandles");
#pragma pack(pop)

    // --- signatures ---
    // Model init / skin finalize: native this-in-ECX; declared with a dummy second parameter so the
    // trampoline routes the model into the this-register.
    using M2_InitFn         = int(__fastcall*)(void* model);
    using M2_FinalizeSkinFn = void(__fastcall*)(void* model);
    // Anim read-completion callback (node on stack).
    using M2_AnimLoadCompleteFn = void(__cdecl*)(void* node);
    // Per-batch alpha setter: native this-in-ECX.
    using M2_SetupBatchAlphaFn = void(__fastcall*)(void* drawContext);
    // Alpha-test reference push (ref on stack).
    using M2_PushAlphaRefFn = void(__cdecl*)(float ref);
    // Ribbon de-relocator (base, fileSize, ctx, ribbons): rebases each ribbon's sub-array offsets.
    using M2_RibbonDeRelocateFn = int(__cdecl*)(int base, unsigned int fileSize, int ctx, unsigned int* ribbons);
    // Ribbon emitter draw: native this-in-ECX.
    using M2_RibbonDrawFn = int(__fastcall*)(void* emitter, void* edx, void* stateBlock);
    // Texture-handle resolver (handle, ...).
    using M2_TexResolveFn = void*(__cdecl*)(void* handle, int a, int b);
    // Sampler bind: native this-in-ECX.
    using M2_SamplerBindFn = void(__fastcall*)(void* device, void* edx, uint32_t selector, void* tex);
}
