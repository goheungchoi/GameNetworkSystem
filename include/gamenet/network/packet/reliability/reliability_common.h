#pragma once
#include <deque>
#include <memory>
#include <unordered_map>

#include "core/memory-stream/memory_bit_stream.h"

// in case we decide to change the type of the sequence number to use fewer or
// more bits
using PacketSequenceNumber = uint16_t;
