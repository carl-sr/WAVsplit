#include <string>
#include <vector>
#include <unordered_map>

#include "WAVparser.h"

#pragma once

struct splitWAV
{
    WAV_t wav;
    std::string fileName;
    int byteOffset;
    int byteLength;
};

// cue point structs - contain sample offsets for timestamps
struct cue_point_t
{
    uint32_t identifier;
    uint32_t position;
    uint32_t data_chunk_id;
    uint32_t chunk_start;
    uint32_t block_start;
    uint32_t sample_start;
};

struct cue_chunk_t
{
    uint32_t cue_points;
    std::vector<cue_point_t> data;
};



class WAVsplitter
{
private:
public:
};