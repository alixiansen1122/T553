/**
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: appmanger_module module.
 * Author: CompanyName
 * Create: 2025-09-04
 */
#ifndef OHOS_ACELITE_CALENDAR_MODULE_H
#define OHOS_ACELITE_CALENDAR_MODULE_H

#include <string>
#include "acelite_config.h"
#include "jsi.h"
#include "js_async_work.h"
#include "non_copyable.h"
#include "calendar_manager.h"
#include "calendar_timer.h"

namespace OHOS {
namespace ACELite {

// Calendar module callback structure
struct CalendarCallback {
    JSIValue callback;        // Asynchronous callback function
    JSIValue jsCtx;           // JS context
    CalendarAccount account;  // Calendar account
    CalendarEventArray eventarray;
    bool enableReminder;
    uint64_t color;
};

class CalendarModule final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(CalendarModule);
    CalendarModule() = default;
    ~CalendarModule() = default;

    // Module initialization
    static void Init(JSIValue exports);

    // Calendar management APIs
    static JSIValue CreateCalendar(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue DeleteCalendar(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetCalendar(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);

    // Calendar event operation APIs
    static JSIValue AddEvent(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue AddEvents(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue DeleteEvent(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue DeleteEvents(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue UpdateEvent(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue UpdateEvents(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetEvent(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetEvents(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetAccount(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue SetConfig(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetConfig(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);

    // Asynchronous operation callback functions
    static void CreateCalendarCallback(void *data);
    static void DeleteCalendarCallback(void *data);
    static void GetCalendarCallback(void *data);
    static void EventOperationCallback(void *data);
    static void AddEventCallback(void *data);
    static void AddEventsCallback(void *data);
    static void DeleteEventCallback(void *data);
    static void GetEventsCallback(void *data);
    static void DeleteEventsCallback(void *data);
    static void GetEventCallback(void *data);
    static void UpdateEventCallback(void *data);
    static void UpdateEventsCallback(void *data);

    static const char *const MODULE_NAME;
    static const char *const CREATE_CALENDAR;
    static const char *const DELETE_CALENDAR;
    static const char *const GET_CALENDAR;
    static const char *const ADD_EVENT;
    static const char *const ADD_EVENTS;
    static const char *const DELETE_EVENT;
    static const char *const DELETE_EVENTS;
    static const char *const UPDATE_EVENT;
    static const char *const UPDATE_EVENTS;
    static const char *const GET_EVENT;
    static const char *const GET_EVENTS;
    static const char *const GET_ACCOUNT;
    static const char *const GET_CONFIG;
    static const char *const SET_CONFIG;

private:
    // Helper function: Convert JS object to CalendarAccount
    static bool ConvertToCalendarAccount(const JSIValue jsAccount, CalendarAccount &account);
    // Helper function: Convert CalendarEvent to JS object
    static JSIValue ConvertToJSEvent(const CalendarEvent &event);
    // Helper function: Convert CalendarAccount to JS object
    static JSIValue ConvertToJSAccount(const CalendarAccount &account);
    // Release callback data
    static void FreeCallbackData(CalendarCallback *callbackData);

    static CalendarCallback *callbackData_;
};

}  // namespace ACELite
}  // namespace OHOS

#endif  // OHOS_ACELITE_CALENDAR_MODULE_H
