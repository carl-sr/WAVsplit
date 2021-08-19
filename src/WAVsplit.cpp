#include "WAVsplit.h"

void WAVsplitter::read_wav(const std::string &filename)
{
    WAV_t wav(filename);
    wav_header = wav.header;
    read_labl(wav);
    read_cue(wav);
}

void WAVsplitter::read_labl(WAV_t &wav)
{
    const std::vector<std::unique_ptr<RIFF_chunk_t>> &riff_lists = wav.get_riff().get_root_chunk().get_subchunks();

    // find all list chunks
    for(auto &i : riff_lists)
    {
        RIFF_chunk_list_t *list = dynamic_cast<RIFF_chunk_list_t *>(i.get());
        if(list != nullptr)
        {
            // find the list chunk with form type 'adtl' (associated data list)
            if(strcmp(list->get_form_type(), "adtl") != 0)
                continue;

            // this is the correct list chunk - find all of the 'labl' or 'note' chunks
            const std::vector<std::unique_ptr<RIFF_chunk_t>> &adtl_chunks = list->get_subchunks();
            for(auto &j : adtl_chunks)
            {
                RIFF_chunk_data_t *adtl = dynamic_cast<RIFF_chunk_data_t *>(j.get());
                if(adtl != nullptr && (strcmp(adtl->get_identifier(), "labl") == 0 || strcmp(adtl->get_identifier(), "note") == 0))
                {
                    // this is an 'adtl' or 'note' chunk
                    uint32_t adtl_id;
                    memcpy(&adtl_id, &adtl->get_data().front(), sizeof(adtl_id));
                    char *identifier = reinterpret_cast<char *>(&adtl->get_data().front()) + 4;
                    labl_identifiers[adtl_id] = std::string(identifier, strlen(identifier));
                }
            }
        }
    }
    // for(const auto &i : labl_identifiers)
    // {
    //     printf("%08x: %s\n", i.first, i.second.c_str());
    // }
}

void WAVsplitter::read_cue(WAV_t &wav)
{
    RIFF_chunk_data_t *cue_data = dynamic_cast<RIFF_chunk_data_t *>(wav.get_riff().get_chunk_with_id("cue "));
    if (cue_data != nullptr)
    {
        std::vector<uint8_t> cue_v = cue_data->get_data();

        // copy length
        memcpy(&cue_chunk.cue_points, &cue_v.front(), sizeof(cue_chunk.cue_points));
        cue_v.erase(cue_v.begin(), cue_v.begin() + sizeof(cue_chunk.cue_points));

        // grab each cue chunk
        for (int i = 0; i < cue_chunk.cue_points; i++)
        {
            cue_point_t cue_point;
            memcpy(&cue_point, &cue_v.front(), sizeof(cue_point));
            cue_v.erase(cue_v.begin(), cue_v.begin() + sizeof(cue_point));
            cue_chunk.data.push_back(cue_point);
        }
    }
    // for (auto i : cue_chunk.data)
    // {
    //     printf("id: %d\n", i.identifier);
    //     printf("pos: %d\n", i.position);
    //     printf("data chunk id: %d\n", i.data_chunk_id);
    //     printf("chunk start: %d\n", i.chunk_start);
    //     printf("block start: %d\n", i.block_start);
    //     printf("sample start: %d\n", i.sample_start);
    // }
}

void WAVsplitter::output_dir_from_filename(const std::string &filename)
{
    std::string dir = filename.substr(filename.rfind('/') + 1);
    dir = dir.substr(0, dir.find('.'));

    set_output_directory(dir);
}

WAVsplitter::WAVsplitter()
{
}

WAVsplitter::WAVsplitter(const std::string &filename)
{
    open(filename);
}

void WAVsplitter::open(const std::string &filename)
{
    read_wav(filename);
    output_dir_from_filename(filename);
}

void WAVsplitter::set_prefix(const std::string &new_prefix)
{
}

const std::string &WAVsplitter::get_prefix() const
{
    return prefix;
}

void WAVsplitter::set_suffix(const std::string &new_suffix)
{
}

const std::string &WAVsplitter::get_suffix() const
{
    return suffix;
}

void WAVsplitter::set_output_directory(const std::string &new_output_directory)
{
    output_directory = new_output_directory;
    if(*output_directory.rbegin() != '/')
        output_directory.append("/");
}

const std::string &WAVsplitter::get_output_directory() const
{
    return output_directory;
}

std::vector<splitWAV> &WAVsplitter::get_splits()
{
    return split_wavs;
}

void WAVsplitter::split()
{
}