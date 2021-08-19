#include <string>
#include <vector>
#include <unordered_map>

#include "WAVparser.h"

#pragma once

struct splitWAV
{
    std::string file_name;
    uint32_t byte_offset;
    uint32_t byte_length;
    WAV_t wav;
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
    std::unordered_map<uint32_t, std::string> labl_identifiers;
    cue_chunk_t cue_chunk;
    std::vector<splitWAV> split_wavs;
    WAV_fmt_t wav_header;

    std::string prefix;
    std::string suffix;

    std::string output_directory;

    void read_wav(const std::string &filename);
    void read_labl(WAV_t &wav);
    void read_cue(WAV_t &wav);

    void output_dir_from_filename(const std::string &filename);

public:
    WAVsplitter();
    WAVsplitter(const std::string &filename);

    void open(const std::string &filename);

    void set_prefix(const std::string &new_prefix);
    const std::string &get_prefix() const;

    void set_suffix(const std::string &new_suffix);
    const std::string &get_suffix() const;

    void set_output_directory(const std::string &new_output_directory);
    const std::string &get_output_directory() const;

    std::vector<splitWAV> &get_splits();

    void split();
};