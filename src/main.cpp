#include <iostream>

#include "./WAVsplit.h"

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " [input_file.wav]" << std::endl;
        return 1;
    }
    WAVsplitter split(argv[1]);
    split.split();
    return 0;
}