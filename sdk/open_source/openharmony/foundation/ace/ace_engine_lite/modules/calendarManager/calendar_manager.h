/**
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: appmanger_module module.
 * Author: CompanyName
 * Create: 2025-09-04
 */
#ifndef CALENDAR_MANAGER_H
#define CALENDAR_MANAGER_H

#include <stddef.h>
#include <stdint.h>
#include <ctime>

namespace OHOS {
namespace ACELite {
// Error code definitions
typedef enum {
    CALENDAR_SUCCESS = 0,
    CALENDAR_FILE_NOT_FOUND,
    CALENDAR_FILE_OPEN_FAILED,
    CALENDAR_JSON_PARSE_ERROR,
    CALENDAR_USER_NOT_FOUND,
    CALENDAR_EVENT_NOT_FOUND,
    CALENDAR_EVENT_ID_EXISTS,
    CALENDAR_USER_INFO_MISMATCH,
    CALENDAR_MEMORY_ALLOC_FAILED,
    CALENDAR_INVALID_PARAMETER,
    CALENDAR_FILE_WRITE_FAILED,
    CALENDAR_EVENT_ID_ALLOC_FAILED,
    CALENDAR_ERROR_MAX
} CalendarError;

// Calendar event structure
typedef struct {
    int id;
    int type;
    int64_t startTime;
    int64_t endTime;
    bool isAllDay;
    bool hasReminder;
    bool isRepeat;
    int64_t recurrenceFrequency;
    char bundlename[64];  // String passed from outside, internal code does not handle its release
    char eventContent[256];
} CalendarEvent;

// Batch event array structure
typedef struct {
    CalendarEvent *events;  // Pointer to event array
    size_t count;           // Number of events
} CalendarEventArray;

// Calendar account structure
typedef struct {
    char *name;
    char *type;
    char *displayName;
} CalendarAccount;

// Error message retrieval interface
const char *GetCalendarErrorString(CalendarError error);

// 1. Create or append user
// Return value: Returns username string on success, nullptr on failure
const char *CreateOrAppendUser(
    const char *filename, const char *name, const char *type, const char *displayName, CalendarError *error);

// 2. Append single event (using CalendarEvent struct for parameters)
CalendarError AppendEvent(const char *filename, const char *name, const CalendarEvent *event);

// 3. Append multiple events
CalendarError AppendEvents(const char *filename, const char *name, const CalendarEventArray *eventArray);

// 4. Get user's specific event
// Return value: Returns dynamically allocated CalendarEvent on success,
// needs to be released by calling FreeCalendarEvent
CalendarEvent *CalendarGetEvent(const char *filename, const char *name, int eventId, CalendarError *error);

// Batch get all events of a user
CalendarEventArray *CalendarGetEvents(const char *filename, const char *name, CalendarError *error);

// 5. Delete single event
CalendarError CalendarDeleteEvent(const char *filename, const char *name, int eventId);

// 6. Batch delete events
CalendarError CalendarDeleteEvents(const char *filename, const char *name, const int *eventIds, size_t idCount);

// 7. Modify/add event (using CalendarEvent struct for parameters)
CalendarError CalendarUpdateEvent(const char *filename, const char *name, const CalendarEvent *event);

CalendarError CalendarUpdateEvents(const char *filename, const char *name, const CalendarEventArray *events);

// 8. Get user information
// Return value: Returns dynamically allocated CalendarAccount on success,
CalendarAccount *GetUserInfo(const char *filename, const char *name, CalendarError *error);

// Convert timestamp to datetime string
// Return 0 on success, -1 on failure
int TimestampToDatetime(time_t timestamp, char *datetimeStr, size_t maxLen);

// Memory release interfaces
void FreeCalendarEvent(CalendarEvent *event);
}  // namespace ACELite
}  // namespace OHOS
#endif  // CALENDAR_MANAGER_H
