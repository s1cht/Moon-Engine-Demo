#include "Win32AudioEngine.hpp"
#include <functiondiscoverykeys_devpkey.h>

namespace ME::Audio
{
#if defined(PLATFORM_WINDOWS)
    ME::Core::Memory::Scope<AudioEngine> AudioEngine::Create()
    {
        return ME::Core::Memory::MakeScope<Win32AudioEngine>();
    }

    Win32AudioEngine::Win32AudioEngine()
        : m_Engine(nullptr), m_Device(nullptr), m_Devices(nullptr), m_SelectedOutputDevice()
    {
        Init();
    }

    Win32AudioEngine::~Win32AudioEngine()
    {
        m_Devices->Release();
        m_Engine->Release();
    }

    void Win32AudioEngine::Init()
    {
        IXAudio2* tempEngine = nullptr;

        HRESULT result = XAudio2Create(&tempEngine, 0, XAUDIO2_ANY_PROCESSOR);
        if (FAILED(result))
        {
            ME_ERROR("Failed to create audio engine! Win32 error: {}", result);
            return;
        }

        m_Engine.reset(tempEngine);
        tempEngine = nullptr;

        IMMDeviceEnumerator* deviceEnumerator = nullptr;
        result = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                         reinterpret_cast<void**>(&deviceEnumerator));
        if (FAILED(result))
        {
            ME_ERROR("Failed to create device enumerator! Win32 error: {}", result);
            return;
        }

        m_Devices.reset(deviceEnumerator);
        deviceEnumerator = nullptr;

        SelectDefaultDevice();

        IXAudio2MasteringVoice* tempDevice = nullptr;
        result = m_Engine->CreateMasteringVoice(&tempDevice, m_SelectedOutputDevice.ID);
        if (FAILED(result))
        {
            ME_ERROR("Failed to select default output device! Win32 error: {}", result);
            return;
        }

        m_Device.reset(tempDevice);
        tempDevice = nullptr;
    }

    ME::Core::Array<AudioDevice> Win32AudioEngine::ListDevices() const
    {
        return {};
    }

    ME::Core::Array<AudioDevice> Win32AudioEngine::ListInputDevices() const
    {
        return {};
    }

    ME::Core::Array<AudioDevice> Win32AudioEngine::ListOutputDevices() const
    {
        return {};
    }

    const AudioDevice& Win32AudioEngine::GetSelectedOutputDevice() const
    {
        return {};
    }

    void Win32AudioEngine::SelectDefaultDevice()
    {
        IMMDevice* device = nullptr;

        HRESULT hResult = m_Devices->GetDefaultAudioEndpoint(eRender, eConsole, &device);
        if (FAILED(hResult))
            return;

        LPWSTR deviceInfo = nullptr;

        hResult = device->GetId(&deviceInfo);
        if (FAILED(hResult))
            return;

        IPropertyStore* properties = nullptr;
        hResult = device->OpenPropertyStore(STGM_READ, &properties);
        if (FAILED(hResult))
            return;

        PROPVARIANT deviceId;
        PROPVARIANT deviceName;
        PropVariantInit(&deviceId);
        PropVariantInit(&deviceName);

        hResult = properties->GetValue(PKEY_Device_InstanceId, &deviceId);
        if (FAILED(hResult) || deviceId.vt == VT_EMPTY)
            return;

        hResult = properties->GetValue(PKEY_Device_FriendlyName, &deviceName);
        if (FAILED(hResult) || deviceId.vt == VT_EMPTY)
            return;

        m_SelectedOutputDevice = {};
        m_SelectedOutputDevice.Type = AudioDeviceType::Output;
        m_SelectedOutputDevice.ID = deviceId.intVal;
        m_SelectedOutputDevice.Name = Core::WideStringToString(deviceName.pwszVal);

        CoTaskMemFree(&deviceInfo);
        deviceInfo = nullptr;
        PropVariantClear(&deviceId);
        PropVariantClear(&deviceName);
        properties->Release();
        device->Release();
    }

    const AudioDevice& Win32AudioEngine::GetSelectedInputDevice() const
    {
        return {};
    }
#endif
}
