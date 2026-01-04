#include "Win32Audio.hpp"
#include "Application/Application.hpp"

namespace ME::Audio
{
#if defined(PLATFORM_WINDOWS)
    ME::Core::Memory::Reference<Audio> Audio::Create(const AudioSpecification& specification)
    {
        return ME::Core::Memory::MakeReference<Win32Audio>(specification);
    }

    Win32Audio::Win32Audio(const AudioSpecification& specification)
        : m_AudioFile(), m_Volume(1), m_BeginTime(0.f), m_EndTime(0.f), m_Looped(false), m_LoopCount(0), m_BufferUpdateRequired(true)
    {
        Init(specification);
    }

    Win32Audio::~Win32Audio()
    {
    }

    void Win32Audio::Play()
    {
        if (m_BufferUpdateRequired)
        {
            XAUDIO2_BUFFER buffer{};
            buffer.Flags = XAUDIO2_END_OF_STREAM;
            buffer.pAudioData = reinterpret_cast<uint8*>(m_AudioFile.Data);
            buffer.AudioBytes = static_cast<UINT32>(m_AudioFile.Size);
            buffer.PlayBegin = GetSampleByTime(m_BeginTime);
            buffer.PlayLength = ME_AUDIO_LOOP_INF - m_BeginTime < ME::Core::Math::Epsilon ? 0 : GetSampleByTime(m_EndTime - m_BeginTime);
            buffer.LoopBegin = buffer.PlayBegin;
            buffer.LoopLength = buffer.PlayLength;
            buffer.LoopCount = m_Looped ? (m_LoopCount == ME_AUDIO_LOOP_INF ? XAUDIO2_LOOP_INFINITE : ME::Core::Math::Clamp(0u, 254u, m_LoopCount)) : 0;

            m_Source->Discontinuity();
            m_Source->SubmitSourceBuffer(&buffer, nullptr);
            m_BufferUpdateRequired = false;
        }
        m_Source->Start(0);
    }

    void Win32Audio::Stop()
    {
        m_Source->Stop();
    }

    void Win32Audio::SetPlayBeginTime(float32 time)
    {
        if (fabs(m_BeginTime - time) < ME::Core::Math::Epsilon)
            return;
        m_BeginTime = time;
        m_BufferUpdateRequired = true;
    }

    void Win32Audio::SetPlayEndTime(float32 time)
    {
        if (fabs(m_EndTime - time) < ME::Core::Math::Epsilon)
            return;
        m_EndTime = ME::Core::Math::Clamp(m_BeginTime, m_EndTime, time);
        m_BufferUpdateRequired = true;
    }

    void Win32Audio::SetLooped(bool looped)
    {
        if (m_Looped == looped)
            return;
        m_Looped = looped;
        m_BufferUpdateRequired = true;
    }

    void Win32Audio::SetLoopCount(uint32 loopCount)
    {
        if (m_LoopCount == loopCount)
            return;
        m_LoopCount = loopCount;
        m_BufferUpdateRequired = true;
    }

    void Win32Audio::SetVolume(float32 volume)
    {
        m_Volume = volume;
        m_Source->SetVolume(m_Volume);
    }

    void Win32Audio::Init(const AudioSpecification& specification)
    {
        m_AudioFile = specification.AudioFile;
        IXAudio2SourceVoice* tempSource = nullptr;
        WAVEFORMATEX format = {};
        format.cbSize = 0;
        format.nChannels = m_AudioFile.AudioInfo.ChannelCount;
        format.nSamplesPerSec = m_AudioFile.AudioInfo.Frequency;
        format.wBitsPerSample = m_AudioFile.AudioInfo.BitPerSample;
        format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
        format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
        format.wFormatTag = WAVE_FORMAT_PCM;

        HRESULT result = Application::GetAudioEngine()->As<Win32AudioEngine>()->GetAudioEngine()->CreateSourceVoice(&tempSource, &format, 0, 2, 
            nullptr, nullptr, nullptr);
        if (FAILED(result))
        {
            ME_ERROR("Failed to create audio! Win32 error: {0}", HRESULT_FROM_WIN32(result));
            return;
        }

        m_Source.reset(tempSource);
        tempSource = nullptr;

        SetVolume(1.f);
        m_BeginTime = 0.f;
        m_EndTime = m_AudioFile.AudioInfo.Length;
        m_Looped = false;
    }

    uint32 Win32Audio::GetSampleByTime(float32 time) const
    {
        return static_cast<uint32>(roundf(time * m_AudioFile.AudioInfo.SamplesPerSecond));
    }
#endif
}
