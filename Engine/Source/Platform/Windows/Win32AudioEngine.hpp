#pragma once
#include "Audio/AudioEngine.hpp"
#include "Win32Platform.hpp"

namespace ME::Audio
{
#if defined(PLATFORM_WINDOWS)
    class MEAPI Win32AudioEngine : public AudioEngine
    {
        struct ComDeleter
        {
            constexpr void operator()(IUnknown* ptr) const noexcept
            {
                ptr->Release();
                ptr = nullptr;
            }
        };

        struct VoiceDeleter
        {
            constexpr void operator()(IXAudio2Voice* ptr) const noexcept
            {
                ptr->DestroyVoice();
                ptr = nullptr;
            }
        };

    public:
        template <typename T>
        using COMPtr = ME::Core::Memory::Scope<T, ComDeleter>;

        template <typename T>
        using VoicePtr = ME::Core::Memory::Scope<T, VoiceDeleter>;

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
        COMPtr<IXAudio2>& GetAudioEngine() { return m_Engine; }

    private:
        void SelectDefaultDevice();

    private:
        COMPtr<IXAudio2> m_Engine;
        COMPtr<IMMDeviceEnumerator> m_Devices;

        VoicePtr<IXAudio2MasteringVoice> m_Device;
        AudioDevice m_SelectedOutputDevice;

    };
#endif
}

