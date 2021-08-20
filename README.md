# WAVsplit
Split WAV files based on stored cue points

## Build

```shell
make
```

This will create an executable file called `wavsplit` in the `build/apps/` directory.

`wavsplit` takes a single argument:

```shell
wavsplit file.wav
```

`observe.wav` is a sample WAV file with cue points. Running the shell command `wavsplit observe.wav` will split the WAV data along the cue points into individual files in the observe directory.

Individual wav files are made based on cue points within the file. If no `cue ` chunks are found, nothing happens.

## Internals

The `cue ` chunk (`cue_chunk_t`) stores the file's individual cue points. These points are read into a struct (`cue_point_t`):

```cpp
struct cue_chunk_t
{
    uint32_t cue_points;
    std::vector<cue_point_t> data;
};

struct cue_point_t
{
    uint32_t identifier;
    uint32_t position;
    uint32_t data_chunk_id;
    uint32_t chunk_start;
    uint32_t block_start;
    uint32_t sample_start;
};
```

The `labl` chunk stores text identifiers for each cue point which are read and assigned based on cue point identifiers.

Access to the WAV file and the underlying RIFF data is coordinated with [WAVparser](https://github.com/rami-hansen/WAVparser) using the `WAV_t` class.