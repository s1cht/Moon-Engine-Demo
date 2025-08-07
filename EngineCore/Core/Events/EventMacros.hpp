#pragma once

#define FORMAT_EVENT_STRING(eventName, args) ME::Core::Containers::String(TEXT("Event ")) + ME::Core::Containers::String(eventName) + TEXT(" fired with data: ") + args
#define EVENT_CLASS_GETSTRING(args)\
    virtual ME::Core::Containers::String GetString() override { return FORMAT_EVENT_STRING(GetName(), args); }

#define EVENT_CLASS_TYPE(type)\
    static EventType GetStaticType() { return EventType::##type; }\
    virtual EventType GetEventType() const override { return GetStaticType(); }\
    virtual const uchar* GetName() const override { return TEXT(#type); }

#define EVENT_CLASS_CATEGORY(category)\
    virtual EVENT_CATEGORY GetEventCategory() const override { return category; }

#define EVENT_CALLBACK_FUNCTION using EventCallbackFunc = std::function<void(ME::Core::Event&)>