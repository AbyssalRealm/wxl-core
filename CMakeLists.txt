# WarcraftXL: the injected client DLL (WarcraftXL.dll) + the offline PE patcher (wxl-patcher.exe).
# Copyright (C) 2026 WarcraftXL. GPLv3 (see source headers).

cmake_minimum_required(VERSION 3.20)
project(WarcraftXL LANGUAGES CXX)

# The DLL injects into the 32-bit client. Configure with:  cmake -A Win32 -B build
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(WARNING "WarcraftXL targets the 32-bit client; configure with -A Win32")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # feeds the IDE the real C++20 flags

set(WXL_DEFS WIN32_LEAN_AND_MEAN NOMINMAX _CRT_SECURE_NO_WARNINGS)

# --- Automatic, recursive source discovery (no hand-written paths) ---------------------------------
# CONFIGURE_DEPENDS re-globs at build time, so dropping a file (or a whole script) needs no edit here.
# Each binary takes its own slice: the DLL excludes the patcher/host trees; the patcher takes only its.
file(GLOB_RECURSE WXL_CORE_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
list(FILTER WXL_CORE_SRC EXCLUDE REGEX "/src/(patcher|host|gpu)/")

# =============================================================================
# Target 1: WarcraftXL.dll  (32-bit, injected) = SDK + runtime scripts
#   runtime scripts live in scripts/*/src/*.cpp and compile straight into the target so their
#   self-registering static ctors are never stripped.
# =============================================================================
file(GLOB_RECURSE WXL_RUNTIME_SCRIPT CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/scripts/*/src/*.cpp")

# Vendored ImGui (core + dx9/win32 backends). Compiled in but DORMANT: nothing creates a context or
# binds an activation key yet, so it adds zero behavior until a script wires it up.
file(GLOB WXL_IMGUI CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui/*.cpp")

add_library(WarcraftXL SHARED ${WXL_CORE_SRC} ${WXL_RUNTIME_SCRIPT} ${WXL_IMGUI})
set_target_properties(WarcraftXL PROPERTIES OUTPUT_NAME "WarcraftXL" PREFIX "")
target_include_directories(WarcraftXL PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/minhook/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/flatbuffers/include")

add_subdirectory(deps/minhook)
target_link_libraries(WarcraftXL PRIVATE minhook d3d9 d3dcompiler)
target_compile_definitions(WarcraftXL PRIVATE ${WXL_DEFS})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${WXL_CORE_SRC} ${WXL_RUNTIME_SCRIPT})

# =============================================================================
# Target 2: wxl-patcher.exe  = the offline PE patcher + patcher scripts
#   patcher scripts live in scripts/*/patcher/*.cpp. The same drop-in/self-register model.
# =============================================================================
file(GLOB        WXL_PATCHER_CORE   CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/patcher/*.cpp")
file(GLOB_RECURSE WXL_PATCHER_SCRIPT CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/scripts/*/patcher/*.cpp")

add_executable(wxl-patcher ${WXL_PATCHER_CORE} ${WXL_PATCHER_SCRIPT})
set_target_properties(wxl-patcher PROPERTIES OUTPUT_NAME "wxl-patcher")
target_include_directories(wxl-patcher PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_compile_definitions(wxl-patcher PRIVATE ${WXL_DEFS})

# =============================================================================
# Target 3: d3d9.dll  = the D3D9On12 proxy
#   The client loads d3d9.dll from its own folder first, so this runs the engine on D3D12 via On12 and
#   owns the shared D3D12 device + queue. 32-bit, to load in the client.
# =============================================================================
file(GLOB WXL_GPU_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/gpu/*.cpp")

add_library(d3d9 SHARED ${WXL_GPU_SRC})
set_target_properties(d3d9 PROPERTIES OUTPUT_NAME "d3d9" PREFIX "")
target_include_directories(d3d9 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(d3d9 PRIVATE d3d12)
target_link_options(d3d9 PRIVATE "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/src/gpu/Proxy.def")
target_compile_definitions(d3d9 PRIVATE ${WXL_DEFS})
