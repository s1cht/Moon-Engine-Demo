#pragma once
#include "Audio/AudioEngine.hpp"
#include "Win32Platform.hpp"

namespace ME::Audio
{
#if defined(PLATFORM_WINDOWS)
    class MEAPI Win32AudioEngine : public AudioEngine
    {
    public:
        Win32AudioEngine();
        ~Win32AudioEngine() override;

    public:
        void Init() override;

    public:
        ME::Core::Array<AudioDevice> ListDevices() const override;
        ME::Core::Array<AudioDevice> ListInputDevices() const override;
        ME::Core::Array<AudioDevice> ListOutputDevices() const override;

        const AudioDevice& GetSelectedOutputDevice() const override;
        const AudioDevice& GetSelectedInputDevice() const override;

    public:
        ME::Core::Memory::Scope<IXAudio2>& GetAudioEngine() { return m_Engine; }

    private:
        void SelectDefaultDevice();

    private:
        ME::Core::Memory::Scope<IXAudio2> m_Engine;
        ME::Core::Memory::Scope<IXAudio2MasteringVoice> m_Device;
        ME::Core::Memory::Scope<IMMDeviceEnumerator> m_Devices;
        AudioDevice m_SelectedOutputDevice;

    };
#endif
}

