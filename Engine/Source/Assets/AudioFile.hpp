#pragma once
#include <Core.hpp>

namespace ME::Assets
{
    enum class AudioFormat : uint8
    {
        None = 0,
        WAV,
    };

    struct AudioFileData
    {
        uint32 Frequency;
        uint16 ChannelCount;
        uint16 BitPerSample;
        uint32 SamplesPerSecond;
        uint64 SampleCount;
        float32 Length;
    };

    struct AudioFile
    {
        void* Data = nullptr;
        SIZE_T Size = 0;
        AudioFormat Format;
        AudioFileData AudioInfo;
    };
}
