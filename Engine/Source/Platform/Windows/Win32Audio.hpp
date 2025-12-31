#pragma once
#include "Win32Platform.hpp"
#include "Audio/AudioEngine.hpp"

namespace ME::Audio
{
#if defined(PLATFORM_WINDOWS)
    class MEAPI Win32Audio : public Audio
    {
    public:
        Win32Audio(const AudioSpecification& specification);
        ~Win32Audio() override;
    public:
        void Play() override;
        void Stop() override;

    public:
        void SetPlayBeginTime(float32 time) override;
        void SetPlayEndTime(float32 time) override;
        void SetLooped(bool looped) override;
        void SetLoopCount(uint32 loopCount) override;
        void SetVolume(float32 volume) override;

    private:
        void Init(const AudioSpecification& specification);

        uint32 GetSampleByTime(float32 time) const;

    private:
        ME::Core::Memory::Scope<IXAudio2SourceVoice> m_Source;
        Assets::AudioFile m_AudioFile;

    private:
        float32 m_Volume;
        float32 m_BeginTime;
        float32 m_EndTime;
        bool m_Looped;
        uint32 m_LoopCount;
        bool m_BufferUpdateRequired;
    };
#endif
}
