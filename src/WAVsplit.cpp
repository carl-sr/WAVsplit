#include "WAVsplit.h"

void WAVsplitter::read_wav(const std::string &filename)
{
    WAV_t wav(filename);
    wav_header = wav.header;
    read_labl(wav);
    read_cue(wav);

    // create splitWAV structs
    split_wavs.reserve(cue_chunk.data.size());
    for (auto &i : cue_chunk.data)
    {
        // lookup string name by identifier
        split_wavs.push_back({labl_identifiers[i.identifier], i.sample_start});

        // assign header data to each WAV_t
        wav.header = wav_header;
    }

    // find duplicate cue point names
    std::unordered_map<std::string, int> names;
    for (auto &i : split_wavs)
    {
        // does this key already exist
        // unique keys that are inserted are given -1
        // duplicate keys begin a counter for occurences at 1
        if (names.find(i.file_name) != names.end())
        {
            if (names[i.file_name] == -1)
                names[i.file_name] = 1;
            else
                names[i.file_name]++;
        }
        else
        {
            names[i.file_name] = -1;
        }
    }

    // rename duplicates
    // cue, cue, cue -> cue_0, cue_1, cue_2 ...
    for (auto i = split_wavs.rbegin(); i != split_wavs.rend(); i++)
    {
        if (names[i->file_name] != -1)
            i->file_name += "_" + std::to_string(names[i->file_name]--);
    }

    // calculate byte lengths
    for (auto i = split_wavs.rbegin(); i != split_wavs.rend(); i++)
    {
        if (i == split_wavs.rbegin())
            i->byte_length = wav.sample_size() * wav.samples.size() - i->byte_offset;
        else
            i->byte_length = (i - 1)->byte_offset - i->byte_offset;
    }

    // populate WAV_t objects
    for (auto &i : split_wavs)
    {
        // find the correct byte sections in the source wav file
        std::vector<uint8_t>::iterator start = wav.get_data().begin() + i.byte_offset;
        std::vector<uint8_t>::iterator end = start + i.byte_length;
        i.wav.get_data().assign(start, end);
        // refresh samples vector with new raw data
        i.wav.load_data();
    }

    for (auto &i : split_wavs)
        printf("%s:\tbyte offset: %d,\tbyte length: %d,\tsizeof data: %d\n", i.file_name.c_str(), i.byte_offset, i.byte_length, i.wav.get_data().size());
}

void WAVsplitter::read_labl(WAV_t &wav)
{
    const std::vector<std::unique_ptr<RIFF_chunk_t>> &riff_lists = wav.get_riff().get_root_chunk().get_subchunks();

    // find all list chunks
    for (auto &i : riff_lists)
    {
        RIFF_chunk_list_t *list = dynamic_cast<RIFF_chunk_list_t *>(i.get());
        if (list != nullptr)
        {
            // find the list chunk with form type 'adtl' (associated data list)
            if (strcmp(list->get_form_type(), "adtl") != 0)
                continue;

            // this is the correct list chunk - find all of the 'labl' or 'note' chunks
            const std::vector<std::unique_ptr<RIFF_chunk_t>> &adtl_chunks = list->get_subchunks();
            for (auto &j : adtl_chunks)
            {
                RIFF_chunk_data_t *adtl = dynamic_cast<RIFF_chunk_data_t *>(j.get());
                if (adtl != nullptr && (strcmp(adtl->get_identifier(), "labl") == 0 || strcmp(adtl->get_identifier(), "note") == 0))
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
    prefix = new_prefix;
}

const std::string &WAVsplitter::get_prefix() const
{
    return prefix;
}

void WAVsplitter::set_suffix(const std::string &new_suffix)
{
    suffix = new_suffix;
}

const std::string &WAVsplitter::get_suffix() const
{
    return suffix;
}

void WAVsplitter::set_output_directory(const std::string &new_output_directory)
{
    output_directory = new_output_directory;
    if (*output_directory.rbegin() != '/')
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