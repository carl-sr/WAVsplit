#include "WAVsplit.h"

void WAVsplitter::read_wav(const std::string &filename)
{
}

void WAVsplitter::read_labl(const WAV_t &wav)
{
}

void WAVsplitter::read_cue(const WAV_t &wav)
{
}

void WAVsplitter::output_dir_from_filename(const std::string &filename)
{
}

WAVsplitter::WAVsplitter()
{
}

WAVsplitter::WAVsplitter(const std::string &filename)
{
}

void WAVsplitter::open(const std::string &filename)
{
}

void WAVsplitter::set_prefix(const std::string &new_prefix)
{
}

const std::string &WAVsplitter::get_prefix() const
{
}

void WAVsplitter::set_suffix(const std::string &new_suffix)
{
}

const std::string &WAVsplitter::get_suffix() const
{
}

void WAVsplitter::set_output_directory(const std::string &new_output_directory)
{
}

const std::string &WAVsplitter::get_output_directory() const
{
}

std::vector<splitWAV> &WAVsplitter::get_splits() const
{
}

void WAVsplitter::split()
{
}