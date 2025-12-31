#pragma once
#include <Core.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Containers/String.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/ClassInterface.hpp>

#include "Assets/AudioFile.hpp"

namespace ME::Audio
{
    constexpr float32 ME_AUDIO_WHOLE = -2.f;
    constexpr uint32 ME_AUDIO_LOOP_INF = 0;

    enum class AudioDeviceType : uint8
    {
        None = 0,
        Output,
        Input
    };

    struct AudioSpecification
    {
        Assets::AudioFile AudioFile;
    };

    class MEAPI Audio : public Core::ClassInterface<Audio>
    {
    public:
        virtual ~Audio() = default;

        virtual void Play() = 0;
        virtual void Stop() = 0;
    public:
        virtual void SetPlayBeginTime(float32 time) = 0;
        virtual void SetPlayEndTime(float32 time) = 0;
        virtual void SetLooped(bool looped) = 0;
        virtual void SetLoopCount(uint32 loopCount) = 0;
        virtual void SetVolume(float32 volume) = 0;

    public:
        static ME::Core::Memory::Reference<ME::Audio::Audio> Create(const AudioSpecification& specification);
    };

    struct AudioDevice
    {
        ME::Core::String Name;
        uint32 ID;
        AudioDeviceType Type;
    };

    class MEAPI AudioEngine : public Core::ClassInterface<AudioEngine>
    {
    public:
        virtual ~AudioEngine() = default;
        virtual void Init() = 0;

        virtual ME::Core::Array<AudioDevice> ListDevices() const = 0;
        virtual ME::Core::Array<AudioDevice> ListInputDevices() const = 0;
        virtual ME::Core::Array<AudioDevice> ListOutputDevices() const = 0;
        virtual const AudioDevice& GetSelectedOutputDevice() const = 0;
        virtual const AudioDevice& GetSelectedInputDevice() const = 0;

    public:
        static ME::Core::Memory::Scope<AudioEngine> Create();

    public:
        static ME::Core::Memory::Reference<ME::Audio::Audio> CreateAudio(ME::Core::String& path);
    };

}
