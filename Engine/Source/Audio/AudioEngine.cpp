#include "AudioEngine.hpp"
#include "Utility/AssetLoader.hpp"

namespace ME::Audio
{
    ME::Core::Memory::Reference<ME::Audio::Audio> AudioEngine::CreateAudio(ME::Core::String& path)
    {
        Utility::AssetLoadResult result = Utility::AssetLoader::Load(path, false, Utility::AssetFileFormats::WAV);
        AudioSpecification specs = {};
        specs.AudioFile = result.Audio;
        return Audio::Create(specs);    
    }
}
