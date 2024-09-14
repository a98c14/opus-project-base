#pragma once

#include <base/defines.h>

typedef uint64 BitField64;

internal void   bitfield_clear(BitField64* field);
internal void   bitfield_set(BitField64* field, uint8 index);
internal bool32 bitfield_is_set(BitField64 field, uint8 index);
internal bool32 bitfield_is_activated(BitField64 prev, BitField64 next, uint8 index);
internal int32  bitfield_bitcount(BitField64 v);