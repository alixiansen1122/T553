/**
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: appmanger_module module.
 * Author: CompanyName
 * Create: 2025-09-04
 */
#include <cstring>
#include <climits>
#include "calendar_manager.h"
#include "event_id_manager.h"
#include "ability_env.h"
#include "calendar_utils.h"
#include "ace_log.h"
#include "calendar_module.h"
namespace OHOS {
namespace ACELite {
// Global variable for calendar file path
std::string g_canlendarFilePath = "/user/app/calendar.json";

// Module constant definitions
const char *const CalendarModule::MODULE_NAME = "calendar";
const char *const CalendarModule::CREATE_CALENDAR = "createCalendar";
const char *const CalendarModule::DELETE_CALENDAR = "deleteCalendar";
const char *const CalendarModule::GET_CALENDAR = "getCalendar";
const char *const CalendarModule::ADD_EVENT = "addEvent";
const char *const CalendarModule::ADD_EVENTS = "addEvents";
const char *const CalendarModule::DELETE_EVENT = "deleteEvent";
const char *const CalendarModule::DELETE_EVENTS = "deleteEvents";
const char *const CalendarModule::UPDATE_EVENT = "updateEvent";
const char *const CalendarModule::UPDATE_EVENTS = "updateEvents";
const char *const CalendarModule::GET_EVENT = "getEvent";
const char *const CalendarModule::GET_EVENTS = "getEvents";
const char *const CalendarModule::GET_ACCOUNT = "getAccount";
const char *const CalendarModule::GET_CONFIG = "getconfig";
const char *const CalendarModule::SET_CONFIG = "setconfig";

// Static member variable for callback data
CalendarCallback *CalendarModule::callbackData_ = nullptr;

// Module initialization
void CalendarModule::Init(JSIValue exports)
{}

void InitCalendarModule(JSIValue exports)
{
    // Register Calendar module API functions
    JSI::SetModuleAPI(exports, CalendarModule::CREATE_CALENDAR, CalendarModule::CreateCalendar);
    JSI::SetModuleAPI(exports, CalendarModule::DELETE_CALENDAR, CalendarModule::DeleteCalendar);
    JSI::SetModuleAPI(exports, CalendarModule::GET_CALENDAR, CalendarModule::GetCalendar);
}

// Helper function: Convert JS object to CalendarAccount
bool CalendarModule::ConvertToCalendarAccount(const JSIValue jsAccount, CalendarAccount &account)
{
    if (!JSI::ValueIsObject(jsAccount)) {
        return false;
    }

    // Extract name property
    JSIValue nameVal = JSI::GetNamedProperty(jsAccount, "name");
    if (JSI::ValueIsString(nameVal)) {
        account.name = JSI::ValueToString(nameVal);
    } else {
        JSI::ReleaseValue(nameVal);
        return false;
    }
    JSI::ReleaseValue(nameVal);

    // Extract type property
    JSIValue typeVal = JSI::GetNamedProperty(jsAccount, "type");
    if (JSI::ValueIsString(typeVal)) {
        account.type = JSI::ValueToString(typeVal);
    } else {
        JSI::ReleaseValue(typeVal);
        ace_free(static_cast<void*>(account.name));
        return false;
    }
    JSI::ReleaseValue(typeVal);

    // Extract displayName property
    JSIValue displayNameVal = JSI::GetNamedProperty(jsAccount, "displayName");
    if (JSI::ValueIsString(displayNameVal)) {
        account.displayName = JSI::ValueToString(displayNameVal);
    } else {
        JSI::ReleaseValue(displayNameVal);
        ace_free(static_cast<void*>(account.name));
        ace_free(static_cast<void*>(account.type));
        return false;
    }
    JSI::ReleaseValue(displayNameVal);

    return true;
}

// Helper function: Convert CalendarEvent to JS object
JSIValue CalendarModule::ConvertToJSEvent(const CalendarEvent &event)
{
    JSIValue jsEvent = JSI::CreateObject();

    JSI::SetNamedProperty(jsEvent, "id", JSI::CreateNumber(event.id));
    JSI::SetNamedProperty(jsEvent, "type", JSI::CreateNumber(event.type));
    JSI::SetNamedProperty(jsEvent, "startTime", JSI::CreateNumber(event.startTime));
    JSI::SetNamedProperty(jsEvent, "endTime", JSI::CreateNumber(event.endTime));
    JSI::SetNamedProperty(jsEvent, "bundlename", JSI::CreateString(event.bundlename));
    JSI::SetNamedProperty(jsEvent, "isAllDay", JSI::CreateBoolean(event.isAllDay));
    JSI::SetNamedProperty(jsEvent, "hasReminder", JSI::CreateBoolean(event.hasReminder));
    JSI::SetNamedProperty(jsEvent, "isRepeat", JSI::CreateBoolean(event.isRepeat));
    JSI::SetNamedProperty(jsEvent, "recurrenceFrequency", JSI::CreateNumber(event.recurrenceFrequency));
    JSI::SetNamedProperty(jsEvent, "eventContent", JSI::CreateString(event.eventContent));

    return jsEvent;
}

// Helper function: Convert CalendarAccount to JS object
JSIValue CalendarModule::ConvertToJSAccount(const CalendarAccount &account)
{
    JSIValue jsAccount = JSI::CreateObject();

    if (account.name) {
        JSI::SetNamedProperty(jsAccount, "name", JSI::CreateString(account.name));
    }
    if (account.type) {
        JSI::SetNamedProperty(jsAccount, "type", JSI::CreateString(account.type));
    }
    if (account.displayName) {
        JSI::SetNamedProperty(jsAccount, "displayName", JSI::CreateString(account.displayName));
    }

    return jsAccount;
}

// Free callback data
void CalendarModule::FreeCallbackData(CalendarCallback *callbackData)
{
    if (callbackData) {
        if (callbackData->account.name) {
            ace_free(callbackData->account.name);
            callbackData->account.name = nullptr;
        }
        if (callbackData->account.type) {
            ace_free(callbackData->account.type);
            callbackData->account.type = nullptr;
        }
        if (callbackData->account.displayName) {
            ace_free(callbackData->account.displayName);
            callbackData->account.displayName = nullptr;
        }
        delete callbackData;
    }
}

// Create calendar
JSIValue CalendarModule::CreateCalendar(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    if (argsNum < ARGC_TWO || !JSI::ValueIsObject(args[0]) || !JSI::ValueIsFunction(args[1])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid arguments for createCalendar");
        return JSI::CreateUndefined();
    }

    // Create callback data
    CalendarCallback *callbackData = new CalendarCallback();
    if (!callbackData) {
        return JSI::CreateUndefined();
    }
    callbackData->account.name = nullptr;
    callbackData->account.displayName = nullptr;
    callbackData->account.type = nullptr;
    if (!callbackData) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to allocate memory for callback data");
        return JSI::CreateUndefined();
    }
    // Save callback function and context
    callbackData->callback = JSI::AcquireValue(args[1]);
    callbackData->jsCtx = JSI::AcquireValue(thisVal);

    // Convert calendar account information
    if (!ConvertToCalendarAccount(args[0], callbackData->account)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to convert to CalendarAccount");
        FreeCallbackData(callbackData);
        JSI::ReleaseValue(callbackData->callback);
        JSI::ReleaseValue(callbackData->jsCtx);
        return JSI::CreateUndefined();
    }
    // Dispatch asynchronous work
    JsAsyncWork::DispatchAsyncWork(CreateCalendarCallback, static_cast<void *>(callbackData));

    return JSI::CreateUndefined();
}

// Asynchronous callback for creating calendar
void CalendarModule::CreateCalendarCallback(void *data)
{
    CalendarCallback *callbackData = static_cast<CalendarCallback *>(data);
    if (!callbackData || !JSI::ValueIsFunction(callbackData->callback)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid callback data");
        FreeCallbackData(callbackData);
        JSI::ReleaseValue(callbackData->callback);
        JSI::ReleaseValue(callbackData->jsCtx);
        return;
    }
    // Call calendar management tool to create user
    CalendarError error = CALENDAR_INVALID_PARAMETER;
    const char *result = CreateOrAppendUser(GetCanlendarFilePath(),
        callbackData->account.name,
        callbackData->account.type,
        callbackData->account.displayName,
        &error);
    // Prepare callback parameters
    JSIValue args[ARGC_TWO];
    JSIValue addEvent;
    JSIValue addEvents;
    JSIValue deleteEvent;
    JSIValue deleteEvents;
    JSIValue updateEvent;
    JSIValue updateEvents;
    JSIValue getEvent;
    JSIValue getEvents;
    JSIValue getAccount;
    JSIValue getConfig;
    JSIValue setConfig;
    JSIValue calendarObj;
    if (error == CALENDAR_SUCCESS && result != nullptr) {
        // Success: first parameter is null, second is calendar object
        args[0] = JSI::CreateNull();
        // Create calendar object
        calendarObj = JSI::CreateObject();
        addEvent = JSI::CreateFunction(CalendarModule::AddEvent);
        addEvents = JSI::CreateFunction(CalendarModule::AddEvents);
        deleteEvent = JSI::CreateFunction(CalendarModule::DeleteEvent);
        deleteEvents = JSI::CreateFunction(CalendarModule::DeleteEvents);
        updateEvent = JSI::CreateFunction(CalendarModule::UpdateEvent);
        updateEvents = JSI::CreateFunction(CalendarModule::UpdateEvents);
        getEvent = JSI::CreateFunction(CalendarModule::GetEvent);
        getEvents = JSI::CreateFunction(CalendarModule::GetEvents);
        getAccount = JSI::CreateFunction(CalendarModule::GetAccount);
        getConfig = JSI::CreateFunction(CalendarModule::GetConfig);
        setConfig = JSI::CreateFunction(CalendarModule::SetConfig);
        JSI::SetNamedProperty(calendarObj, ADD_EVENT, addEvent);
        JSI::SetNamedProperty(calendarObj, ADD_EVENTS, addEvents);
        JSI::SetNamedProperty(calendarObj, DELETE_EVENT, deleteEvent);
        JSI::SetNamedProperty(calendarObj, DELETE_EVENTS, deleteEvents);
        JSI::SetNamedProperty(calendarObj, UPDATE_EVENT, updateEvent);
        JSI::SetNamedProperty(calendarObj, UPDATE_EVENTS, updateEvents);
        JSI::SetNamedProperty(calendarObj, GET_EVENT, getEvent);
        JSI::SetNamedProperty(calendarObj, GET_EVENTS, getEvents);
        JSI::SetNamedProperty(calendarObj, GET_ACCOUNT, getAccount);
        JSI::SetNamedProperty(calendarObj, GET_CONFIG, getConfig);
        JSI::SetNamedProperty(calendarObj, SET_CONFIG, setConfig);
        // Store calendar account information;
        args[1] = calendarObj;
        if (callbackData_) {
            FreeCallbackData(callbackData_);
            callbackData_ = nullptr;
        }
        callbackData_ = callbackData;
        JSI::CallFunction(callbackData->callback, callbackData->jsCtx, args, ARGC_TWO);
        JSI::ReleaseValueList(args[0],
            args[1],
            callbackData->callback,
            callbackData->jsCtx,
            addEvent,
            addEvents,
            deleteEvent,
            deleteEvents,
            updateEvent,
            updateEvents,
            getConfig,
            setConfig,
            getEvent,
            getEvents,
            getAccount,
            ARGS_END);
    } else {
        // Failure: first parameter is error message, second is null
        args[0] = JSI::CreateString(GetCalendarErrorString(error));
        args[1] = JSI::CreateNull();
        JSI::CallFunction(callbackData->callback, callbackData->jsCtx, args, ARGC_TWO);
        JSI::ReleaseValueList(callbackData->callback, callbackData->jsCtx, args[0], args[1], ARGS_END);
        FreeCallbackData(callbackData);
    }
}

// Delete calendar
JSIValue CalendarModule::DeleteCalendar(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    if (callbackData_) {
        FreeCallbackData(callbackData_);
        callbackData_ = nullptr;
    }
    return JSI::CreateUndefined();
}

// Asynchronous callback for deleting calendar
void CalendarModule::DeleteCalendarCallback(void *data)
{
    // Implement actual logic and callback handling for deleting calendar
}

// Get default calendar
JSIValue CalendarModule::GetCalendar(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    if (argsNum < 1 || !JSI::ValueIsFunction(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid arguments for getCalendar");
        return JSI::CreateUndefined();
    }
    // Create callback data
    CalendarCallback *callbackData = new CalendarCallback();
    if (!callbackData) {
        return JSI::CreateUndefined();
    }
    callbackData->account.name = nullptr;
    callbackData->account.displayName = nullptr;
    callbackData->account.type = nullptr;
    if (!callbackData) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to allocate memory for callback data");
        return JSI::CreateUndefined();
    }
    // Save callback function and context
    callbackData->callback = JSI::AcquireValue(args[0]);
    callbackData->jsCtx = JSI::AcquireValue(thisVal);

    OHOS::ACELite::JsAsyncWork::DispatchAsyncWork(GetCalendarCallback, static_cast<void *>(callbackData));

    return JSI::CreateUndefined();
}

// Asynchronous callback for getting default calendar
void CalendarModule::GetCalendarCallback(void *data)
{
    // Implement actual logic and callback handling for getting default calendar
    CalendarCallback *callbackData = static_cast<CalendarCallback *>(data);
    if (!callbackData || !JSI::ValueIsFunction(callbackData->callback)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid callback data");
        FreeCallbackData(callbackData);
        return;
    }
    CalendarError error = CALENDAR_INVALID_PARAMETER;
    CalendarAccount *account = GetUserInfo(GetCanlendarFilePath(), "default", &error);
    if (account) {
        callbackData->account.type = account->type;
        callbackData->account.name = account->name;
        callbackData->account.displayName = account->displayName;
        free(account);
    }
    // Prepare callback parameters
    JSIValue args[ARGC_TWO];
    JSIValue addEvent;
    JSIValue addEvents;
    JSIValue deleteEvent;
    JSIValue deleteEvents;
    JSIValue updateEvent;
    JSIValue updateEvents;
    JSIValue getEvent;
    JSIValue getEvents;
    JSIValue getAccount;
    JSIValue getConfig;
    JSIValue setConfig;
    if (error == CALENDAR_SUCCESS) {
        // Success: first parameter is null, second is calendar object
        args[0] = JSI::CreateNull();
        // Create calendar object
        JSIValue calendarObj = JSI::CreateObject();
        addEvent = JSI::CreateFunction(CalendarModule::AddEvent);
        addEvents = JSI::CreateFunction(CalendarModule::AddEvents);
        deleteEvent = JSI::CreateFunction(CalendarModule::DeleteEvent);
        deleteEvents = JSI::CreateFunction(CalendarModule::DeleteEvents);
        updateEvent = JSI::CreateFunction(CalendarModule::UpdateEvent);
        updateEvents = JSI::CreateFunction(CalendarModule::UpdateEvents);
        getEvent = JSI::CreateFunction(CalendarModule::GetEvent);
        getEvents = JSI::CreateFunction(CalendarModule::GetEvents);
        getAccount = JSI::CreateFunction(CalendarModule::GetAccount);
        getConfig = JSI::CreateFunction(CalendarModule::GetConfig);
        setConfig = JSI::CreateFunction(CalendarModule::SetConfig);
        JSI::SetNamedProperty(calendarObj, ADD_EVENT, addEvent);
        JSI::SetNamedProperty(calendarObj, ADD_EVENTS, addEvents);
        JSI::SetNamedProperty(calendarObj, DELETE_EVENT, deleteEvent);
        JSI::SetNamedProperty(calendarObj, DELETE_EVENTS, deleteEvents);
        JSI::SetNamedProperty(calendarObj, UPDATE_EVENT, updateEvent);
        JSI::SetNamedProperty(calendarObj, UPDATE_EVENTS, updateEvents);
        JSI::SetNamedProperty(calendarObj, GET_EVENT, getEvent);
        JSI::SetNamedProperty(calendarObj, GET_EVENTS, getEvents);
        JSI::SetNamedProperty(calendarObj, GET_ACCOUNT, getAccount);
        JSI::SetNamedProperty(calendarObj, GET_CONFIG, getConfig);
        JSI::SetNamedProperty(calendarObj, SET_CONFIG, setConfig);
        // Store calendar account information;
        args[1] = calendarObj;
        if (callbackData_) {
            FreeCallbackData(callbackData_);
            callbackData_ = nullptr;
        }
        callbackData_ = callbackData;
        JSI::CallFunction(callbackData->callback, callbackData->jsCtx, args, ARGC_TWO);
        JSI::ReleaseValueList(args[0],
            args[1],
            addEvent,
            addEvents,
            deleteEvent,
            deleteEvents,
            updateEvent,
            updateEvents,
            getConfig,
            setConfig,
            getEvent,
            getEvents,
            getAccount,
            ARGS_END);
    } else {
        // Failure: first parameter is error message, second is null
        args[0] = JSI::CreateString(GetCalendarErrorString(error));
        args[1] = JSI::CreateNull();
        JSI::CallFunction(callbackData->callback, callbackData->jsCtx, args, ARGC_TWO);
        JSI::ReleaseValueList(callbackData->callback, callbackData->jsCtx, args[0], args[1], ARGS_END);
        FreeCallbackData(callbackData);
    }
}

// Add event
void CalendarModule::AddEventCallback(void *data)
{
    CalendarCallback *callbackData = static_cast<CalendarCallback *>(data);
    if (!callbackData || !JSI::ValueIsFunction(callbackData->callback)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid callback data");
        FreeCallbackData(callbackData);
        return;
    }
    CalendarError error = CALENDAR_INVALID_PARAMETER;
    int id = AllocateAndSetCalendarEvent(callbackData->eventarray.events);
    if (id > 0) {
        error = AppendEvent(GetCanlendarFilePath(), callbackData_->account.name, callbackData->eventarray.events);
        if (error != CALENDAR_SUCCESS) {
            FreeEventId(id);
        }
    } else {
        error = CALENDAR_EVENT_ID_ALLOC_FAILED;
        free(callbackData->eventarray.events);
    }

    JSIValue args[ARGC_TWO];
    if (error == CALENDAR_SUCCESS) {
        args[1] = JSI::CreateNumber(callbackData->eventarray.events->id);
        args[0] = JSI::CreateNull();
        JSI::CallFunction(callbackData->callback, callbackData->jsCtx, args, ARGC_TWO);
        JSI::ReleaseValueList(callbackData->callback, callbackData->jsCtx, args[0], args[1], ARGS_END);
    } else {
        args[0] = JSI::CreateString(GetCalendarErrorString(error));
        args[1] = JSI::CreateNull();
        JSI::CallFunction(callbackData->callback, callbackData->jsCtx, args, ARGC_TWO);
        JSI::ReleaseValueList(callbackData->callback, callbackData->jsCtx, args[0], args[1], ARGS_END);
    }
    delete callbackData;
    callbackData = nullptr;
}

JSIValue CalendarModule::AddEvent(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    // Implement logic for adding a single event
    if (callbackData_ == nullptr) {
        return nullptr;
    }
    if (argsNum < 1 || !JSI::ValueIsFunction(args[1]) || !JSI::ValueIsObject(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid arguments for AddEvent");
        return JSI::CreateUndefined();
    }
    CalendarCallback *callbackData = new CalendarCallback();
    if (!callbackData) {
        return JSI::CreateUndefined();
    }
    callbackData->eventarray.events = (CalendarEvent *)malloc(sizeof(CalendarEvent));
    if (!callbackData->eventarray.events) {
        delete callbackData;
        return JSI::CreateUndefined();
    }
    if (strcpy_s(callbackData->eventarray.events->bundlename,
        sizeof(callbackData->eventarray.events->bundlename),
        GetBundleName()) != EOK) {
        free(callbackData->eventarray.events);
        delete callbackData;
        return JSI::CreateUndefined();
    }
    callbackData->eventarray.events->startTime = (int64_t)JSI::GetNumberProperty(args[0], "startTime");
    callbackData->eventarray.events->endTime = (int64_t)JSI::GetNumberProperty(args[0], "endTime");
    callbackData->eventarray.events->type = (int)JSI::GetNumberProperty(args[0], "type");
    char *eventContent = JSI::GetStringProperty(args[0], "eventContent");
    if (strcpy_s(callbackData->eventarray.events->eventContent,
        sizeof(callbackData->eventarray.events->eventContent),
        eventContent) != EOK) {
        free(callbackData->eventarray.events);
        ace_free(eventContent);
        delete callbackData;
        return JSI::CreateUndefined();
    }
    ace_free(eventContent);
    callbackData->eventarray.events->isAllDay = JSI::GetBooleanProperty(args[0], "isAllDay");
    callbackData->eventarray.events->hasReminder = JSI::GetBooleanProperty(args[0], "hasReminder");
    callbackData->eventarray.events->isRepeat = JSI::GetBooleanProperty(args[0], "isRepeat");
    callbackData->eventarray.events->recurrenceFrequency =
        (int64_t)JSI::GetNumberProperty(args[0], "recurrenceFrequency");
    // Save callback function and context
    callbackData->callback = JSI::AcquireValue(args[1]);
    callbackData->jsCtx = JSI::AcquireValue(thisVal);

    OHOS::ACELite::JsAsyncWork::DispatchAsyncWork(AddEventCallback, static_cast<void *>(callbackData));
    return JSI::CreateUndefined();
}

// Add multiple events
void CalendarModule::AddEventsCallback(void *data)
{
    CalendarCallback *callbackData = static_cast<CalendarCallback *>(data);
    if (!callbackData || !JSI::ValueIsFunction(callbackData->callback)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid callback data");
        FreeCallbackData(callbackData);
        return;
    }
    CalendarError error = CALENDAR_SUCCESS;
    int *ids = static_cast<int*>(ace_malloc(sizeof(int) * callbackData->eventarray.count));
    if (!ids) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid callback data");
        FreeCallbackData(callbackData);
        return;
    }
    for (uint32_t arrIndex = 0; arrIndex < callbackData->eventarray.count; arrIndex++) {
        CalendarEvent *event = (CalendarEvent *)malloc(sizeof(CalendarEvent));
        if (!event) {
            FreeCallbackData(callbackData);
            return;
        }
        if(memcpy_s(event, sizeof(CalendarEvent),
            callbackData->eventarray.events + arrIndex, sizeof(CalendarEvent)) != EOK) {
            FreeCallbackData(callbackData);
            return;
        }
        ids[arrIndex] = AllocateAndSetCalendarEvent(event);
        callbackData->eventarray.events[arrIndex].id = ids[arrIndex];
        if (ids[arrIndex] <= 0) {
            error = CALENDAR_EVENT_ID_ALLOC_FAILED;
            for (uint32_t i = 0; i < arrIndex; i++) {
                FreeEventId(ids[i]);
            }
            break;
        }
    }
    ace_free(ids);
    bool flag = true;
    if (error == CALENDAR_SUCCESS) {
        error = AppendEvents(GetCanlendarFilePath(), callbackData_->account.name, &callbackData->eventarray);
        if (error != CALENDAR_SUCCESS) {
            flag = false;
        }
    }
    JSIValue args[ARGC_TWO];
    JSIValue result[callbackData->eventarray.count];
    if (error == CALENDAR_SUCCESS) {
        JSIValue evnetArray = JSI::CreateArray(callbackData->eventarray.count);
        for (uint32_t evnetArrayCnt = 0; evnetArrayCnt < callbackData->eventarray.count; evnetArrayCnt++) {
            result[evnetArrayCnt] = JSI::CreateNumber(callbackData->eventarray.events[evnetArrayCnt].id);
            JSI::SetPropertyByIndex(evnetArray, evnetArrayCnt, result[evnetArrayCnt]);
        }
        args[1] = evnetArray;
        args[0] = JSI::CreateNull();
        JSI::CallFunction(callbackData->callback, callbackData->jsCtx, args, ARGC_TWO);
        for (uint32_t evnetArrayCnt = 0; evnetArrayCnt < callbackData->eventarray.count; evnetArrayCnt++) {
            JSI::ReleaseValue(result[evnetArrayCnt]);
        }
        JSI::ReleaseValueList(callbackData->callback, callbackData->jsCtx, args[0], args[1], ARGS_END);
    } else {
        if (!flag) {
            for (uint32_t arrIndex = 0; arrIndex < callbackData->eventarray.count; arrIndex++) {
                FreeEventId(callbackData->eventarray.events[arrIndex].id);
            }
        }
        args[0] = JSI::CreateString(GetCalendarErrorString(error));
        args[1] = JSI::CreateNull();
        JSI::CallFunction(callbackData->callback, callbackData->jsCtx, args, ARGC_TWO);
        JSI::ReleaseValueList(callbackData->callback, callbackData->jsCtx, args[0], args[1], ARGS_END);
    }
    if (callbackData->eventarray.events) {
        free(static_cast<void*>(callbackData->eventarray.events));
    }

    delete callbackData;
    callbackData = nullptr;
}

// Batch add events
JSIValue CalendarModule::AddEvents(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    // Implement logic for batch adding events
    if (callbackData_ == nullptr) {
        return nullptr;
    }
    if (argsNum < 1 || !JSI::ValueIsFunction(args[1]) || !JSI::ValueIsArray(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid arguments for AddEvents");
        return JSI::CreateUndefined();
    }
    CalendarCallback *callbackData = new CalendarCallback();
    if (!callbackData) {
        return JSI::CreateUndefined();
    }
    uint32_t count = JSI::GetArrayLength(args[0]);
    callbackData->eventarray.events = (CalendarEvent *)malloc(sizeof(CalendarEvent) * count);
    if (!callbackData->eventarray.events) {
        delete callbackData;
        return JSI::CreateUndefined();
    }
    for (uint32_t arrIndex = 0; arrIndex < count; arrIndex++) {
        JSIValue point = JSI::GetPropertyByIndex(args[0], arrIndex);
        if (strcpy_s(callbackData->eventarray.events[arrIndex].bundlename,
            sizeof(callbackData->eventarray.events[arrIndex].bundlename),
            GetBundleName()) != EOK) {
            free(callbackData->eventarray.events);
            delete callbackData;
            return JSI::CreateUndefined();
        }
        callbackData->eventarray.events[arrIndex].startTime = (int64_t)JSI::GetNumberProperty(point, "startTime");
        callbackData->eventarray.events[arrIndex].endTime = (int64_t)JSI::GetNumberProperty(point, "endTime");
        callbackData->eventarray.events[arrIndex].type = (int)JSI::GetNumberProperty(point, "type");
        callbackData->eventarray.events[arrIndex].isAllDay = JSI::GetBooleanProperty(point, "isAllDay");
        callbackData->eventarray.events[arrIndex].hasReminder = JSI::GetBooleanProperty(point, "hasReminder");
        callbackData->eventarray.events[arrIndex].isRepeat = JSI::GetBooleanProperty(point, "isRepeat");
        callbackData->eventarray.events[arrIndex].recurrenceFrequency =
            (int64_t)JSI::GetNumberProperty(point, "recurrenceFrequency");
        char *eventContent = JSI::GetStringProperty(point, "eventContent");
        if (strcpy_s(callbackData->eventarray.events[arrIndex].eventContent,
            sizeof(callbackData->eventarray.events[arrIndex].eventContent),
            eventContent) != EOK) {
            free(callbackData->eventarray.events);
            ace_free(eventContent);
            delete callbackData;
            return JSI::CreateUndefined();
        }
        ace_free(eventContent);
        JSI::ReleaseValue(point);
    }
    callbackData->eventarray.count = count;
    // Save callback function and context
    callbackData->callback = JSI::AcquireValue(args[1]);
    callbackData->jsCtx = JSI::AcquireValue(thisVal);

    OHOS::ACELite::JsAsyncWork::DispatchAsyncWork(AddEventsCallback, static_cast<void *>(callbackData));
    return JSI::CreateUndefined();
}

// Delete event
void CalendarModule::DeleteEventCallback(void *data)
{
    CalendarCallback *callbackData = static_cast<CalendarCallback *>(data);
    if (!callbackData || !JSI::ValueIsFunction(callbackData->callback)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid callback data");
        FreeCallbackData(callbackData);
        return;
    }
    CalendarError error = CALENDAR_INVALID_PARAMETER;
    error = CalendarDeleteEvent(GetCanlendarFilePath(),
        callbackData_->account.name, callbackData->eventarray.events->id);
    JSIValue args[ARGC_ONE];
    if (error == CALENDAR_SUCCESS) {
        FreeEventId(callbackData->eventarray.events->id);
        args[0] = JSI::CreateNull();
        JSI::CallFunction(callbackData->callback, callbackData->jsCtx, args, ARGC_ONE);
        JSI::ReleaseValueList(callbackData->callback, callbackData->jsCtx, args[0], ARGS_END);
    } else {
        args[0] = JSI::CreateString(GetCalendarErrorString(error));
        JSI::CallFunction(callbackData->callback, callbackData->jsCtx, args, ARGC_ONE);
        JSI::ReleaseValueList(callbackData->callback, callbackData->jsCtx, args[0], ARGS_END);
    }
    free(static_cast<void*>(callbackData->eventarray.events));
    delete callbackData;
    callbackData = nullptr;
}

JSIValue CalendarModule::DeleteEvent(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    // Implement logic for deleting a single event
    if (callbackData_ == nullptr) {
        return nullptr;
    }
    if (argsNum < 1 || !JSI::ValueIsFunction(args[1]) || !JSI::ValueIsNumber(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid arguments for DeleteEvent");
        return JSI::CreateUndefined();
    }
    CalendarCallback *callbackData = new CalendarCallback();
    if (!callbackData) {
        return JSI::CreateUndefined();
    }
    callbackData->eventarray.events = (CalendarEvent *)malloc(sizeof(CalendarEvent));
    if (!callbackData->eventarray.events) {
        delete callbackData;
        return JSI::CreateUndefined();
    }
    callbackData->eventarray.events->id = static_cast<int64_t>(JSI::ValueToNumber(args[0]));
    // Save callback function and context
    callbackData->callback = JSI::AcquireValue(args[1]);
    callbackData->jsCtx = JSI::AcquireValue(thisVal);

    OHOS::ACELite::JsAsyncWork::DispatchAsyncWork(DeleteEventCallback, static_cast<void *>(callbackData));
    return JSI::CreateUndefined();
}

// Batch delete events
void CalendarModule::DeleteEventsCallback(void *data)
{
    CalendarCallback *callbackData = static_cast<CalendarCallback *>(data);
    if (!callbackData || !JSI::ValueIsFunction(callbackData->callback)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid callback data");
        FreeCallbackData(callbackData);
        return;
    }
    CalendarError error = CALENDAR_INVALID_PARAMETER;
    int *eventIds = static_cast<int*>(ace_malloc(sizeof(int) * callbackData->eventarray.count));
    if (!eventIds) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid callback data");
        FreeCallbackData(callbackData);
        return;
    }
    for (uint32_t i = 0; i < callbackData->eventarray.count; i++) {
        eventIds[i] = callbackData->eventarray.events[i].id;
    }
    error = CalendarDeleteEvents(GetCanlendarFilePath(),
        callbackData_->account.name, eventIds, callbackData->eventarray.count);
    ace_free(eventIds);
    JSIValue args[ARGC_ONE];
    if (error == CALENDAR_SUCCESS) {
        for (uint32_t i = 0; i < callbackData->eventarray.count; i++) {
            FreeEventId(callbackData->eventarray.events[i].id);
        }
        args[0] = JSI::CreateNull();
        JSI::CallFunction(callbackData->callback, callbackData->jsCtx, args, ARGC_ONE);
        JSI::ReleaseValueList(callbackData->callback, callbackData->jsCtx, args[0], ARGS_END);
    } else {
        args[0] = JSI::CreateString(GetCalendarErrorString(error));
        JSI::CallFunction(callbackData->callback, callbackData->jsCtx, args, ARGC_ONE);
        JSI::ReleaseValueList(callbackData->callback, callbackData->jsCtx, args[0], ARGS_END);
    }
    free(static_cast<void*>(callbackData->eventarray.events));
    delete callbackData;
    callbackData = nullptr;
}

JSIValue CalendarModule::DeleteEvents(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    // Implement logic for batch deleting events
    if (callbackData_ == nullptr) {
        return nullptr;
    }
    if (argsNum < 1 || !JSI::ValueIsFunction(args[1]) || !JSI::ValueIsArray(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid arguments for DeleteEvents");
        return JSI::CreateUndefined();
    }
    CalendarCallback *callbackData = new CalendarCallback();
    if (!callbackData) {
        return JSI::CreateUndefined();
    }
    uint32_t count = JSI::GetArrayLength(args[0]);
    callbackData->eventarray.events = (CalendarEvent *)malloc(sizeof(CalendarEvent) * count);
    if (!callbackData->eventarray.events) {
        delete callbackData;
        return JSI::CreateUndefined();
    }
    for (uint32_t arrIndex = 0; arrIndex < count; arrIndex++) {
        JSIValue point = JSI::GetPropertyByIndex(args[0], arrIndex);
        callbackData->eventarray.events[arrIndex].id = static_cast<int64_t>(JSI::GetNumberProperty(point, "id"));
        JSI::ReleaseValue(point);
    }
    callbackData->eventarray.count = count;
    // Save callback function and context
    callbackData->callback = JSI::AcquireValue(args[1]);
    callbackData->jsCtx = JSI::AcquireValue(thisVal);

    OHOS::ACELite::JsAsyncWork::DispatchAsyncWork(DeleteEventsCallback, static_cast<void *>(callbackData));
    return JSI::CreateUndefined();
}

// Update event
void CalendarModule::UpdateEventCallback(void *data)
{
    CalendarCallback *callbackData = static_cast<CalendarCallback *>(data);
    if (!callbackData || !JSI::ValueIsFunction(callbackData->callback)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid callback data");
        FreeCallbackData(callbackData);
        return;
    }
    CalendarError error = CALENDAR_INVALID_PARAMETER;
    error = CalendarUpdateEvent(GetCanlendarFilePath(), callbackData_->account.name, callbackData->eventarray.events);
    JSIValue args[ARGC_ONE];
    if (error == CALENDAR_SUCCESS) {
        FreeEventId(callbackData->eventarray.events->id);
        CalendarEvent *event = (CalendarEvent *)malloc(sizeof(CalendarEvent));
        if (!event) {
            FreeCallbackData(callbackData);
            return;
        }
        if(memcpy_s(event, sizeof(CalendarEvent), callbackData->eventarray.events, sizeof(CalendarEvent)) != EOK) {
            HILOG_ERROR(HILOG_MODULE_ACE, "memcpy_s fail");
        }
        SetCalendarEventWithSpecificId(callbackData->eventarray.events->id, event);
        args[0] = JSI::CreateNull();
        JSI::CallFunction(callbackData->callback, callbackData->jsCtx, args, ARGC_ONE);
        JSI::ReleaseValueList(callbackData->callback, callbackData->jsCtx, args[0], ARGS_END);
    } else {
        args[0] = JSI::CreateString(GetCalendarErrorString(error));
        JSI::CallFunction(callbackData->callback, callbackData->jsCtx, args, ARGC_ONE);
        JSI::ReleaseValueList(callbackData->callback, callbackData->jsCtx, args[0], ARGS_END);
    }
    if (static_cast<void*>(callbackData->eventarray.events)) {
        free(static_cast<void*>(callbackData->eventarray.events));
    }
    delete callbackData;
    callbackData = nullptr;
}

JSIValue CalendarModule::UpdateEvent(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    // Implement logic for updating a single event
    if (callbackData_ == nullptr) {
        return nullptr;
    }
    if (argsNum < 1 || !JSI::ValueIsFunction(args[1]) || !JSI::ValueIsObject(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid arguments for UpdateEvent");
        return JSI::CreateUndefined();
    }
    CalendarCallback *callbackData = new CalendarCallback();
    if (!callbackData) {
        return JSI::CreateUndefined();
    }
    callbackData->eventarray.events = (CalendarEvent *)malloc(sizeof(CalendarEvent));
    if (!callbackData->eventarray.events) {
        delete callbackData;
        return JSI::CreateUndefined();
    }
    if (strcpy_s(callbackData->eventarray.events->bundlename,
        sizeof(callbackData->eventarray.events->bundlename),
        GetBundleName()) != EOK) {
        free(callbackData->eventarray.events);
        delete callbackData;
        return JSI::CreateUndefined();
    }
    callbackData->eventarray.events->startTime = (int64_t)JSI::GetNumberProperty(args[0], "startTime");
    callbackData->eventarray.events->endTime = (int64_t)JSI::GetNumberProperty(args[0], "endTime");
    callbackData->eventarray.events->type = (int)JSI::GetNumberProperty(args[0], "type");
    callbackData->eventarray.events->id = (int)JSI::GetNumberProperty(args[0], "id");
    char *eventContent = JSI::GetStringProperty(args[0], "eventContent");
    if (strcpy_s(callbackData->eventarray.events->eventContent,
        sizeof(callbackData->eventarray.events->eventContent),
        eventContent) != EOK) {
        free(callbackData->eventarray.events);
        ace_free(eventContent);
        delete callbackData;
        return JSI::CreateUndefined();
    }
    ace_free(eventContent);
    callbackData->eventarray.events->isAllDay = JSI::GetBooleanProperty(args[0], "isAllDay");
    callbackData->eventarray.events->hasReminder = JSI::GetBooleanProperty(args[0], "hasReminder");
    callbackData->eventarray.events->isRepeat = JSI::GetBooleanProperty(args[0], "isRepeat");
    callbackData->eventarray.events->recurrenceFrequency =
        (int64_t)JSI::GetNumberProperty(args[0], "recurrenceFrequency");
    // Save callback function and context
    callbackData->callback = JSI::AcquireValue(args[1]);
    callbackData->jsCtx = JSI::AcquireValue(thisVal);

    OHOS::ACELite::JsAsyncWork::DispatchAsyncWork(UpdateEventCallback, static_cast<void *>(callbackData));
    return JSI::CreateUndefined();
}

// Batch update events
void CalendarModule::UpdateEventsCallback(void *data)
{
    CalendarCallback *callbackData = static_cast<CalendarCallback *>(data);
    if (!callbackData || !JSI::ValueIsFunction(callbackData->callback)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid callback data");
        FreeCallbackData(callbackData);
        return;
    }
    CalendarError error = CALENDAR_INVALID_PARAMETER;
    error = CalendarUpdateEvents(GetCanlendarFilePath(), callbackData_->account.name, &callbackData->eventarray);
    if (error == CALENDAR_SUCCESS) {
        for (uint32_t evnetArrayCnt = 0; evnetArrayCnt < callbackData->eventarray.count; evnetArrayCnt++) {
            CalendarEvent *event = (CalendarEvent *)malloc(sizeof(CalendarEvent));
            if (!event) {
                FreeCallbackData(callbackData);
                return;
            }
            memcpy_s(
                event, sizeof(CalendarEvent), callbackData->eventarray.events + evnetArrayCnt, sizeof(CalendarEvent));
            FreeEventId(callbackData->eventarray.events[evnetArrayCnt].id);
            SetCalendarEventWithSpecificId(callbackData->eventarray.events[evnetArrayCnt].id, event);
        }
    }
    JSIValue args[ARGC_TWO];
    JSIValue result[callbackData->eventarray.count];
    if (error == CALENDAR_SUCCESS) {
        JSIValue evnetArray = JSI::CreateArray(callbackData->eventarray.count);
        for (uint32_t evnetArrayCnt = 0; evnetArrayCnt < callbackData->eventarray.count; evnetArrayCnt++) {
            result[evnetArrayCnt] = JSI::CreateNumber(callbackData->eventarray.events[evnetArrayCnt].id);
            JSI::SetPropertyByIndex(evnetArray, evnetArrayCnt, result[evnetArrayCnt]);
        }
        args[1] = evnetArray;
        args[0] = JSI::CreateNull();
        JSI::CallFunction(callbackData->callback, callbackData->jsCtx, args, ARGC_TWO);
        for (uint32_t evnetArrayCnt = 0; evnetArrayCnt < callbackData->eventarray.count; evnetArrayCnt++) {
            JSI::ReleaseValue(result[evnetArrayCnt]);
        }
        JSI::ReleaseValueList(callbackData->callback, callbackData->jsCtx, args[0], args[1], ARGS_END);
    } else {
        args[0] = JSI::CreateString(GetCalendarErrorString(error));
        args[1] = JSI::CreateNull();
        JSI::CallFunction(callbackData->callback, callbackData->jsCtx, args, ARGC_TWO);
        JSI::ReleaseValueList(callbackData->callback, callbackData->jsCtx, args[0], args[1], ARGS_END);
    }
    if (callbackData->eventarray.events) {
        free(static_cast<void*>(callbackData->eventarray.events));
    }
    delete callbackData;
    callbackData = nullptr;
}

JSIValue CalendarModule::UpdateEvents(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    // Implement logic for batch updating events
    if (callbackData_ == nullptr) {
        return nullptr;
    }
    if (argsNum < 1 || !JSI::ValueIsFunction(args[1]) || !JSI::ValueIsArray(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid arguments for UpdateEvents");
        return JSI::CreateUndefined();
    }
    CalendarCallback *callbackData = new CalendarCallback();
    if (!callbackData) {
        return JSI::CreateUndefined();
    }
    uint32_t count = JSI::GetArrayLength(args[0]);
    callbackData->eventarray.events = (CalendarEvent *)malloc(sizeof(CalendarEvent) * count);
    if (!callbackData->eventarray.events) {
        delete callbackData;
        return JSI::CreateUndefined();
    }
    for (uint32_t arrIndex = 0; arrIndex < count; arrIndex++) {
        JSIValue point = JSI::GetPropertyByIndex(args[0], arrIndex);
        if (strcpy_s(callbackData->eventarray.events[arrIndex].bundlename,
            sizeof(callbackData->eventarray.events[arrIndex].bundlename),
            GetBundleName()) != EOK) {
            free(callbackData->eventarray.events);
            delete callbackData;
            return JSI::CreateUndefined();
        }
        callbackData->eventarray.events[arrIndex].startTime = (int64_t)JSI::GetNumberProperty(point, "startTime");
        callbackData->eventarray.events[arrIndex].endTime = (int64_t)JSI::GetNumberProperty(point, "endTime");
        callbackData->eventarray.events[arrIndex].type = (int)JSI::GetNumberProperty(point, "type");
        callbackData->eventarray.events[arrIndex].id = (int64_t)JSI::GetNumberProperty(point, "id");
        char *eventContent = JSI::GetStringProperty(point, "eventContent");
        if (strcpy_s(callbackData->eventarray.events[arrIndex].eventContent,
            sizeof(callbackData->eventarray.events[arrIndex].eventContent),
            eventContent) != EOK) {
            ace_free(eventContent);
            free(callbackData->eventarray.events);
            delete callbackData;
            return JSI::CreateUndefined();
        }
        ace_free(eventContent);
        callbackData->eventarray.events[arrIndex].isAllDay = JSI::GetBooleanProperty(point, "isAllDay");
        callbackData->eventarray.events[arrIndex].hasReminder = JSI::GetBooleanProperty(point, "hasReminder");
        callbackData->eventarray.events[arrIndex].isRepeat = JSI::GetBooleanProperty(point, "isRepeat");
        callbackData->eventarray.events[arrIndex].recurrenceFrequency =
            (int64_t)JSI::GetNumberProperty(point, "recurrenceFrequency");
        JSI::ReleaseValue(point);
    }
    callbackData->eventarray.count = count;
    // Save callback function and context
    callbackData->callback = JSI::AcquireValue(args[1]);
    callbackData->jsCtx = JSI::AcquireValue(thisVal);

    OHOS::ACELite::JsAsyncWork::DispatchAsyncWork(UpdateEventsCallback, static_cast<void *>(callbackData));
    return JSI::CreateUndefined();
}

// Get event
void CalendarModule::GetEventCallback(void *data)
{
    CalendarCallback *callbackData = static_cast<CalendarCallback *>(data);
    if (!callbackData || !JSI::ValueIsFunction(callbackData->callback)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid callback data");
        FreeCallbackData(callbackData);
        return;
    }
    CalendarError error = CALENDAR_INVALID_PARAMETER;
    CalendarEvent *event = CalendarGetEvent(GetCanlendarFilePath(),
        callbackData_->account.name, callbackData->eventarray.events->id, &error);
    JSIValue args[ARGC_TWO];
    if (error == CALENDAR_SUCCESS && event != nullptr) {
        args[1] = JSI::CreateObject();
        JSI::SetNumberProperty(args[1], "startTime", event->startTime);
        JSI::SetNumberProperty(args[1], "endTime", event->endTime);
        JSI::SetNumberProperty(args[1], "type", event->type);
        JSI::SetNumberProperty(args[1], "id", event->id);
        JSI::SetStringProperty(args[1], "bundlename", event->bundlename);
        JSI::SetBooleanProperty(args[1], "isAllDay", event->isAllDay);
        JSI::SetBooleanProperty(args[1], "hasReminder", event->hasReminder);
        JSI::SetBooleanProperty(args[1], "isRepeat", event->isRepeat);
        JSI::SetNumberProperty(args[1], "recurrenceFrequency", event->recurrenceFrequency);
        JSI::SetStringProperty(args[1], "eventContent", event->eventContent);
        args[0] = JSI::CreateNull();
        JSI::CallFunction(callbackData->callback, callbackData->jsCtx, args, ARGC_TWO);
        JSI::ReleaseValueList(callbackData->callback, callbackData->jsCtx, args[0], args[1], ARGS_END);
    } else {
        args[0] = JSI::CreateString(GetCalendarErrorString(error));
        args[1] = JSI::CreateNull();
        JSI::CallFunction(callbackData->callback, callbackData->jsCtx, args, ARGC_TWO);
        JSI::ReleaseValueList(callbackData->callback, callbackData->jsCtx, args[0], args[1], ARGS_END);
    }
    if (event) {
        free(event);
    }
    free(callbackData->eventarray.events);
    delete callbackData;
    callbackData = nullptr;
}

JSIValue CalendarModule::GetEvent(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    // Implement logic for getting a single event
    if (callbackData_ == nullptr) {
        return nullptr;
    }
    if (argsNum < 1 || !JSI::ValueIsFunction(args[1]) || !JSI::ValueIsNumber(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid arguments for GetEvent");
        return JSI::CreateUndefined();
    }
    CalendarCallback *callbackData = new CalendarCallback();
    if (!callbackData) {
        return JSI::CreateUndefined();
    }
    callbackData->eventarray.events = (CalendarEvent *)malloc(sizeof(CalendarEvent));
    if (!callbackData->eventarray.events) {
        delete callbackData;
        return JSI::CreateUndefined();
    }
    callbackData->eventarray.events->id = static_cast<int64_t>(JSI::ValueToNumber(args[0]));
    // Save callback function and context
    callbackData->callback = JSI::AcquireValue(args[1]);
    callbackData->jsCtx = JSI::AcquireValue(thisVal);

    OHOS::ACELite::JsAsyncWork::DispatchAsyncWork(GetEventCallback, static_cast<void *>(callbackData));
    return JSI::CreateUndefined();
}

// Get all events
void CalendarModule::GetEventsCallback(void *data)
{
    CalendarCallback *callbackData = static_cast<CalendarCallback *>(data);
    if (!callbackData || !JSI::ValueIsFunction(callbackData->callback)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid callback data");
        FreeCallbackData(callbackData);
        return;
    }
    CalendarError error = CALENDAR_INVALID_PARAMETER;
    CalendarEventArray *eventarray = CalendarGetEvents(GetCanlendarFilePath(), callbackData_->account.name, &error);
    JSIValue args[ARGC_TWO];
    if (error == CALENDAR_SUCCESS && eventarray != nullptr) {
        args[1] = JSI::CreateArray(eventarray->count);
        for (uint32_t arrIndex = 0; arrIndex < eventarray->count; arrIndex++) {
            JSIValue point = JSI::CreateObject();
            JSI::SetNumberProperty(point, "startTime", eventarray->events[arrIndex].startTime);
            JSI::SetNumberProperty(point, "endTime", eventarray->events[arrIndex].endTime);
            JSI::SetNumberProperty(point, "type", eventarray->events[arrIndex].type);
            JSI::SetNumberProperty(point, "id", eventarray->events[arrIndex].id);
            JSI::SetStringProperty(point, "bundlename", eventarray->events[arrIndex].bundlename);
            JSI::SetBooleanProperty(point, "isAllDay", eventarray->events[arrIndex].isAllDay);
            JSI::SetBooleanProperty(point, "hasReminder", eventarray->events[arrIndex].hasReminder);
            JSI::SetBooleanProperty(point, "isRepeat", eventarray->events[arrIndex].isRepeat);
            JSI::SetNumberProperty(point, "recurrenceFrequency", eventarray->events[arrIndex].recurrenceFrequency);
            JSI::SetStringProperty(point, "eventContent", eventarray->events[arrIndex].eventContent);
            JSI::SetPropertyByIndex(args[1], arrIndex, point);
            JSI::ReleaseValue(point);
        }
        args[0] = JSI::CreateNull();
        JSI::CallFunction(callbackData->callback, callbackData->jsCtx, args, ARGC_TWO);
        JSI::ReleaseValueList(callbackData->callback, callbackData->jsCtx, args[0], args[1], ARGS_END);
    } else {
        args[0] = JSI::CreateString(GetCalendarErrorString(error));
        args[1] = JSI::CreateNull();
        JSI::CallFunction(callbackData->callback, callbackData->jsCtx, args, ARGC_TWO);
        JSI::ReleaseValueList(callbackData->callback, callbackData->jsCtx, args[0], args[1], ARGS_END);
    }
    if (eventarray) {
        if (eventarray->events) {
            free(eventarray->events);
        }
        free(eventarray);
    }

    delete callbackData;
    callbackData = nullptr;
}

JSIValue CalendarModule::GetEvents(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    // Implement logic for getting all events
    if (callbackData_ == nullptr) {
        return nullptr;
    }
    if (argsNum < 1 || !JSI::ValueIsFunction(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid arguments for GetEvents");
        return JSI::CreateUndefined();
    }
    CalendarCallback *callbackData = new CalendarCallback();
    if (!callbackData) {
        return JSI::CreateUndefined();
    }
    // Save callback function and context
    callbackData->callback = JSI::AcquireValue(args[0]);
    callbackData->jsCtx = JSI::AcquireValue(thisVal);

    OHOS::ACELite::JsAsyncWork::DispatchAsyncWork(GetEventsCallback, static_cast<void *>(callbackData));
    return JSI::CreateUndefined();
}

// Get account information
JSIValue CalendarModule::GetAccount(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    // Implement logic for getting account information
    if (argsNum != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid arguments for GetAccount");
        return JSI::CreateUndefined();
    }
    if (callbackData_) {
        JSIValue point = JSI::CreateObject();
        JSI::SetStringProperty(point, "name", callbackData_->account.name);
        JSI::SetStringProperty(point, "displayName", callbackData_->account.displayName);
        JSI::SetStringProperty(point, "type", callbackData_->account.type);
        return point;
    } else {
        return JSI::CreateUndefined();
    }
}

// Set Config information
JSIValue CalendarModule::SetConfig(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    // Implement logic for getting account information
    if (argsNum < 1 || !JSI::ValueIsObject(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid arguments for SetConfig");
        return JSI::CreateBoolean(false);
    }
    if (callbackData_) {
        callbackData_->enableReminder = JSI::GetBooleanProperty(args[0], "enableReminder");
        callbackData_->color = JSI::GetNumberProperty(args[0], "color");
        return JSI::CreateBoolean(true);
    } else {
        return JSI::CreateBoolean(false);
    }
}

// Get Config information
JSIValue CalendarModule::GetConfig(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    // Implement logic for getting account information
    if (argsNum != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid arguments for GetConfig");
        return JSI::CreateUndefined();
    }
    if (callbackData_) {
        JSIValue point = JSI::CreateObject();
        JSI::SetBooleanProperty(point, "enableReminder", callbackData_->enableReminder);
        JSI::SetNumberProperty(point, "color", callbackData_->color);
        return point;
    } else {
        return JSI::CreateUndefined();
    }
}

// Asynchronous callback for event operations
void CalendarModule::EventOperationCallback(void *data)
{
    // Implement callback handling for event operations
}

#ifdef __cplusplus
extern "C" {
#endif
void SetCanlendarFilePath(char *canlendarFilePath)
{
    g_canlendarFilePath = canlendarFilePath;
}

const char *GetCanlendarFilePath(void)
{
    return g_canlendarFilePath.c_str();
}
#ifdef __cplusplus
};
#endif

}  // namespace ACELite
}  // namespace OHOS
