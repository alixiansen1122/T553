/**
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: appmanger_module module.
 * Author: CompanyName
 * Create: 2025-09-04
 */
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <securec.h>
#include <fcntl.h>
#include <unistd.h>
#include "ace_mem_base.h"
#include "memory_heap.h"
#include "cJSON.h"
#include "ace_log.h"
#include "calendar_manager.h"

#define FILE_SIZE_MAX 0xF00000
namespace OHOS {
namespace ACELite {
// Error message mapping table
static const char *g_errorStrings[CALENDAR_ERROR_MAX] = {"Operation successful",
    "File does not exist",
    "File open failed",
    "JSON parsing error",
    "User does not exist",
    "Event does not exist",
    "Event ID already exists",
    "User information does not match",
    "Memory allocation failed",
    "Invalid parameter",
    "File write failed",
    "Failed to allocate event ID"};

// Get error message string
const char *GetCalendarErrorString(CalendarError error)
{
    return (error >= 0 && error < CALENDAR_ERROR_MAX) ? g_errorStrings[error] : "Unknown error";
}

// Internal utility: Read JSON file content
static char *ReadJsonFile(const char *filename, CalendarError *error)
{
    if (!filename || !error) {
        if (error) {
            *error = CALENDAR_INVALID_PARAMETER;
        }
        return nullptr;
    }

    FILE *file = fopen(filename, "rb");
    if (!file) {
        *error = (errno == ENOENT) ? CALENDAR_FILE_NOT_FOUND : CALENDAR_FILE_OPEN_FAILED;
        return nullptr;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long fileLen = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (fileLen > FILE_SIZE_MAX) {
        return nullptr;
    }
    // Allocate memory (+1 for terminator)
    char *data = (char *)malloc(fileLen + 1);
    if (!data) {
        fclose(file);
        *error = CALENDAR_MEMORY_ALLOC_FAILED;
        return nullptr;
    }

    // Read file
    size_t readLen = fread(data, 1, fileLen, file);
    fclose(file);

    if (readLen != fileLen) {
        free(data);
        *error = CALENDAR_FILE_OPEN_FAILED;
        return nullptr;
    }
    data[fileLen] = '\0';
    *error = CALENDAR_SUCCESS;
    return data;
}

// Internal utility: Write to JSON file
static CalendarError writeJsonFile(const char *filename, cJSON *root)
{
    if (!filename || !root) {
        return CALENDAR_INVALID_PARAMETER;
    }

    char *jsonStr = cJSON_Print(root);
    if (!jsonStr) {
        return CALENDAR_MEMORY_ALLOC_FAILED;
    }

    int fd = open(filename, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        free(jsonStr);
        return CALENDAR_FILE_OPEN_FAILED;
    }

    size_t strLen = strlen(jsonStr);
    ssize_t writeLen = write(fd, jsonStr, strLen);
    close(fd);  // Close file descriptor
    free(jsonStr);

    return (writeLen == strLen) ? CALENDAR_SUCCESS : CALENDAR_FILE_WRITE_FAILED;
}

// Internal utility: Get or create JSON root object
static cJSON *getOrCreateRoot(const char *filename, CalendarError *error)
{
    if (!filename || !error) {
        *error = CALENDAR_INVALID_PARAMETER;
        return nullptr;
    }

    char *jsonData = ReadJsonFile(filename, error);
    cJSON *root = nullptr;

    // Create empty root object when file doesn't exist
    if (*error == CALENDAR_FILE_NOT_FOUND) {
        root = cJSON_CreateObject();
        *error = (root) ? CALENDAR_SUCCESS : CALENDAR_MEMORY_ALLOC_FAILED;
        return root;
    }

    // Return directly if read failed
    if (*error != CALENDAR_SUCCESS) {
        return nullptr;
    }

    // Parse JSON data
    root = cJSON_Parse(jsonData);
    free(jsonData);
    jsonData = nullptr;
    if (!root) {
        *error = CALENDAR_JSON_PARSE_ERROR;
    } else {
        *error = CALENDAR_SUCCESS;
    }

    return root;
}

// Internal utility: Find user JSON object
static cJSON *findUserObject(cJSON *root, const char *userName)
{
    if (!root || !userName || !cJSON_IsObject(root)) {
        return nullptr;
    }
    return cJSON_GetObjectItemCaseSensitive(root, userName);
}

// Internal utility: Find event index in array (-1 means not exists)
static int findEventIndex(cJSON *eventsArray, int eventId)
{
    if (!eventsArray || !cJSON_IsArray(eventsArray)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameter");
        return -1;
    }

    int index = 0;
    cJSON *event = nullptr;
    cJSON_ArrayForEach(event, eventsArray)
    {
        cJSON *idItem = cJSON_GetObjectItemCaseSensitive(event, "id");
        if (idItem && cJSON_IsNumber(idItem) && idItem->valueint == eventId) {
            return index;
        }
        index++;
    }
    return -1;
}

// Internal utility: Create cJSON object from CalendarEvent struct
static cJSON *createEventJson(const CalendarEvent *event)
{
    if (!event) {
        return nullptr;
    }

    cJSON *eventJson = cJSON_CreateObject();
    if (!eventJson) {
        return nullptr;
    }
    cJSON_AddNumberToObject(eventJson, "id", event->id);
    cJSON_AddNumberToObject(eventJson, "type", event->type);
    cJSON_AddNumberToObject(eventJson, "startTime", event->startTime);
    cJSON_AddNumberToObject(eventJson, "endTime", event->endTime);
    cJSON_AddStringToObject(eventJson, "bundlename", event->bundlename);
    cJSON_AddBoolToObject(eventJson, "isAllDay", event->isAllDay);
    cJSON_AddBoolToObject(eventJson, "hasReminder", event->hasReminder);
    cJSON_AddBoolToObject(eventJson, "isRepeat", event->isRepeat);
    cJSON_AddNumberToObject(eventJson, "recurrenceFrequency", event->recurrenceFrequency);
    cJSON_AddStringToObject(eventJson, "eventContent", event->eventContent);
    return eventJson;
}

// Internal utility: Create user account JSON object
static cJSON *createUserAccountJson(const char *name, const char *type, const char *displayName)
{
    if (!name || !type || !displayName) {
        return nullptr;
    }

    cJSON *account = cJSON_CreateObject();
    if (!account) {
        return nullptr;
    }

    // Create CalendarAccount object
    cJSON *calendarAccount = cJSON_CreateObject();
    if (!calendarAccount) {
        cJSON_Delete(account);
        return nullptr;
    }
    cJSON_AddStringToObject(calendarAccount, "name", name);
    cJSON_AddStringToObject(calendarAccount, "type", type);
    cJSON_AddStringToObject(calendarAccount, "displayName", displayName);
    cJSON_AddItemToObject(account, "CalendarAccount", calendarAccount);

    // Create empty events array
    cJSON *events = cJSON_CreateArray();
    if (!events) {
        cJSON_Delete(account);
        return nullptr;
    }
    cJSON_AddItemToObject(account, "events", events);

    return account;
}

// 1. Create or append user
const char *CreateOrAppendUser(
    const char *filename, const char *name, const char *type, const char *displayName, CalendarError *error)
{
    if (!filename || !name || !type || !displayName || !error) {
        if (error) {
            *error = CALENDAR_INVALID_PARAMETER;
        }
        return nullptr;
    }

    cJSON *root = getOrCreateRoot(filename, error);
    if (!root || *error != CALENDAR_SUCCESS) {
        return nullptr;
    }

    cJSON *user = findUserObject(root, name);
    if (user) {
        // User already exists, check if information matches
        cJSON *account = cJSON_GetObjectItemCaseSensitive(user, "CalendarAccount");
        if (!account) {
            cJSON_Delete(root);
            *error = CALENDAR_JSON_PARSE_ERROR;
            return nullptr;
        }

        cJSON *existingtype = cJSON_GetObjectItemCaseSensitive(account, "type");
        cJSON *existingDisplayName = cJSON_GetObjectItemCaseSensitive(account, "displayName");

        if (!existingtype || !existingDisplayName || !cJSON_IsString(existingtype) ||
            !cJSON_IsString(existingDisplayName) || strcmp(existingtype->valuestring, type) != 0 ||
            strcmp(existingDisplayName->valuestring, displayName) != 0) {
            cJSON_Delete(root);
            *error = CALENDAR_USER_INFO_MISMATCH;
            return nullptr;
        }

        // Information matches, return username
        cJSON_Delete(root);
        *error = CALENDAR_SUCCESS;
        return name;
    }

    // Create new user
    cJSON *newUser = createUserAccountJson(name, type, displayName);
    if (!newUser) {
        cJSON_Delete(root);
        *error = CALENDAR_MEMORY_ALLOC_FAILED;
        return nullptr;
    }

    cJSON_AddItemToObject(root, name, newUser);
    *error = writeJsonFile(filename, root);
    cJSON_Delete(root);

    return (*error == CALENDAR_SUCCESS) ? name : nullptr;
}

// 2. Append single event (using CalendarEvent struct for parameters)
CalendarError AppendEvent(const char *filename, const char *name, const CalendarEvent *event)
{
    if (!filename || !name || !event) {
        return CALENDAR_INVALID_PARAMETER;
    }

    CalendarError error = CALENDAR_INVALID_PARAMETER;
    cJSON *root = getOrCreateRoot(filename, &error);
    if (!root || error != CALENDAR_SUCCESS) {
        return error;
    }

    cJSON *user = findUserObject(root, name);
    if (!user) {
        cJSON_Delete(root);
        return CALENDAR_USER_NOT_FOUND;
    }

    cJSON *events = cJSON_GetObjectItemCaseSensitive(user, "events");
    if (!events || !cJSON_IsArray(events)) {
        events = cJSON_CreateArray();
        if (!events) {
            cJSON_Delete(root);
            return CALENDAR_MEMORY_ALLOC_FAILED;
        }
        cJSON_AddItemToObject(user, "events", events);
    }

    // Check if event ID already exists
    if (findEventIndex(events, event->id) != -1) {
        cJSON_Delete(root);
        return CALENDAR_EVENT_ID_EXISTS;
    }

    // Create and add new event
    cJSON *eventJson = createEventJson(event);
    if (!eventJson) {
        cJSON_Delete(root);
        return CALENDAR_MEMORY_ALLOC_FAILED;
    }

    cJSON_AddItemToArray(events, eventJson);
    error = writeJsonFile(filename, root);
    cJSON_Delete(root);

    return error;
}

// 3. Append multiple events
CalendarError AppendEvents(const char *filename, const char *name, const CalendarEventArray *eventArray)
{
    if (!filename || !name || !eventArray || !eventArray->events || eventArray->count == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "AppendEvents Invalid parameter");
        return CALENDAR_INVALID_PARAMETER;
    }

    CalendarError error = CALENDAR_INVALID_PARAMETER;
    cJSON *root = getOrCreateRoot(filename, &error);
    if (!root || error != CALENDAR_SUCCESS) {
        return error;
    }

    cJSON *user = findUserObject(root, name);
    if (!user) {
        cJSON_Delete(root);
        return CALENDAR_USER_NOT_FOUND;
    }

    cJSON *eventsArray = cJSON_GetObjectItemCaseSensitive(user, "events");
    if (!eventsArray || !cJSON_IsArray(eventsArray)) {
        eventsArray = cJSON_CreateArray();
        if (!eventsArray) {
            cJSON_Delete(root);
            return CALENDAR_MEMORY_ALLOC_FAILED;
        }
        cJSON_AddItemToObject(user, "events", eventsArray);
    }

    // Check if all event IDs already exist
    for (size_t i = 0; i < eventArray->count; i++) {
        if (findEventIndex(eventsArray, eventArray->events[i].id) != -1) {
            cJSON_Delete(root);
            return CALENDAR_EVENT_ID_EXISTS;
        }
    }

    // Add events in batch
    for (size_t i = 0; i < eventArray->count; i++) {
        cJSON *eventJson = createEventJson(&eventArray->events[i]);
        if (!eventJson) {
            cJSON_Delete(root);
            return CALENDAR_MEMORY_ALLOC_FAILED;
        }
        cJSON_AddItemToArray(eventsArray, eventJson);
    }

    error = writeJsonFile(filename, root);
    cJSON_Delete(root);

    return error;
}

// 4. Get user's specific event
CalendarEvent *CalendarGetEvent(const char *filename, const char *name, int eventId, CalendarError *error)
{
    if (!filename || !name || !error) {
        *error = CALENDAR_INVALID_PARAMETER;
        HILOG_ERROR(HILOG_MODULE_ACE, "AppendEvents Invalid parameter");
        return nullptr;
    }

    char *jsonData = ReadJsonFile(filename, error);
    if (!jsonData || *error != CALENDAR_SUCCESS) {
        return nullptr;
    }

    cJSON *root = cJSON_Parse(jsonData);
    free(jsonData);
    jsonData = nullptr;
    if (!root) {
        *error = CALENDAR_JSON_PARSE_ERROR;
        return nullptr;
    }

    cJSON *user = findUserObject(root, name);
    if (!user) {
        cJSON_Delete(root);
        *error = CALENDAR_USER_NOT_FOUND;
        return nullptr;
    }

    cJSON *events = cJSON_GetObjectItemCaseSensitive(user, "events");
    if (!events || !cJSON_IsArray(events)) {
        cJSON_Delete(root);
        *error = CALENDAR_EVENT_NOT_FOUND;
        return nullptr;
    }

    int index = findEventIndex(events, eventId);
    if (index == -1) {
        cJSON_Delete(root);
        *error = CALENDAR_EVENT_NOT_FOUND;
        return nullptr;
    }

    // Extract event information
    cJSON *eventObj = cJSON_GetArrayItem(events, index);
    CalendarEvent *event = (CalendarEvent *)malloc(sizeof(CalendarEvent));
    if (!event) {
        cJSON_Delete(root);
        *error = CALENDAR_MEMORY_ALLOC_FAILED;
        return nullptr;
    }

    cJSON *item = nullptr;

    item = cJSON_GetObjectItemCaseSensitive(eventObj, "id");
    event->id = (item && cJSON_IsNumber(item)) ? item->valueint : -1;

    item = cJSON_GetObjectItemCaseSensitive(eventObj, "type");
    event->type = (item && cJSON_IsNumber(item)) ? item->valueint : -1;

    item = cJSON_GetObjectItemCaseSensitive(eventObj, "startTime");
    event->startTime = (item && cJSON_IsNumber(item)) ? item->valuedouble : -1;

    item = cJSON_GetObjectItemCaseSensitive(eventObj, "endTime");
    event->endTime = (item && cJSON_IsNumber(item)) ? item->valuedouble : -1;

    item = cJSON_GetObjectItemCaseSensitive(eventObj, "bundlename");
    if (item && cJSON_IsString(item)) {
        if (strcpy_s(event->bundlename, sizeof(event->bundlename), item->valuestring) != EOK) {
            cJSON_Delete(root);
            *error = CALENDAR_MEMORY_ALLOC_FAILED;
            return nullptr;
        }
    }

    item = cJSON_GetObjectItemCaseSensitive(eventObj, "isAllDay");
    event->isAllDay = (item && cJSON_IsBool(item)) ? item->valueint : false;

    item = cJSON_GetObjectItemCaseSensitive(eventObj, "hasReminder");
    event->hasReminder = (item && cJSON_IsBool(item)) ? item->valueint : false;

    item = cJSON_GetObjectItemCaseSensitive(eventObj, "isRepeat");
    event->isRepeat = (item && cJSON_IsBool(item)) ? item->valueint : false;

    item = cJSON_GetObjectItemCaseSensitive(eventObj, "recurrenceFrequency");
    event->recurrenceFrequency = (item && cJSON_IsNumber(item)) ? item->valuedouble : -1;

    item = cJSON_GetObjectItemCaseSensitive(eventObj, "eventContent");
    if (item && cJSON_IsString(item)) {
        if (strcpy_s(event->eventContent, sizeof(event->eventContent), item->valuestring) != EOK) {
            cJSON_Delete(root);
            *error = CALENDAR_MEMORY_ALLOC_FAILED;
            return nullptr;
        }
    }

    cJSON_Delete(root);
    root = nullptr;
    *error = CALENDAR_SUCCESS;
    return event;
}

// Batch get all events of a user
CalendarEventArray *CalendarGetEvents(const char *filename, const char *name, CalendarError *error)
{
    // Parameter validation
    if (!filename || !name || !error) {
        HILOG_ERROR(HILOG_MODULE_ACE, "AppendEvents Invalid parameter");
        *error = CALENDAR_INVALID_PARAMETER;
        return nullptr;
    }
    *error = CALENDAR_SUCCESS;

    // Read JSON file content
    char *jsonData = ReadJsonFile(filename, error);
    if (!jsonData || *error != CALENDAR_SUCCESS) {
        return nullptr;
    }

    // Parse JSON
    cJSON *root = cJSON_Parse(jsonData);
    free(jsonData);  // Free JSON string memory
    jsonData = nullptr;
    if (!root) {
        *error = CALENDAR_JSON_PARSE_ERROR;
        return nullptr;
    }

    // Find user object
    cJSON *user = findUserObject(root, name);
    if (!user) {
        cJSON_Delete(root);
        *error = CALENDAR_USER_NOT_FOUND;
        return nullptr;
    }

    // Get events array
    cJSON *events = cJSON_GetObjectItemCaseSensitive(user, "events");
    if (!events || !cJSON_IsArray(events)) {
        cJSON_Delete(root);
        *error = CALENDAR_EVENT_NOT_FOUND;
        return nullptr;
    }

    // Get event count
    int eventCount = cJSON_GetArraySize(events);
    if (eventCount <= 0) {
        cJSON_Delete(root);
        *error = CALENDAR_EVENT_NOT_FOUND;
        return nullptr;
    }

    // Allocate memory for event array
    CalendarEvent *eventList = (CalendarEvent *)malloc(sizeof(CalendarEvent) * eventCount);
    if (!eventList) {
        cJSON_Delete(root);
        *error = CALENDAR_MEMORY_ALLOC_FAILED;
        return nullptr;
    }

    // Allocate memory for event array structure
    CalendarEventArray *eventArray = (CalendarEventArray *)malloc(sizeof(CalendarEventArray));
    if (!eventArray) {
        free(eventList);
        cJSON_Delete(root);
        *error = CALENDAR_MEMORY_ALLOC_FAILED;
        return nullptr;
    }

    // Parse events one by one
    cJSON *eventObj = nullptr;
    int i = 0;
    cJSON_ArrayForEach(eventObj, events)
    {
        cJSON *item = nullptr;

        // Parse event ID
        item = cJSON_GetObjectItemCaseSensitive(eventObj, "id");
        eventList[i].id = (item && cJSON_IsNumber(item)) ? item->valueint : -1;

        // Parse event type
        item = cJSON_GetObjectItemCaseSensitive(eventObj, "type");
        eventList[i].type = (item && cJSON_IsNumber(item)) ? item->valueint : -1;

        // Parse start time
        item = cJSON_GetObjectItemCaseSensitive(eventObj, "startTime");
        eventList[i].startTime = (item && cJSON_IsNumber(item)) ? (int64_t)item->valuedouble : -1;

        // Parse end time
        item = cJSON_GetObjectItemCaseSensitive(eventObj, "endTime");
        eventList[i].endTime = (item && cJSON_IsNumber(item)) ? (int64_t)item->valuedouble : -1;

        // Parse bundlename
        item = cJSON_GetObjectItemCaseSensitive(eventObj, "bundlename");
        if (item && cJSON_IsString(item)) {
            if (strcpy_s(eventList[i].bundlename, sizeof(eventList[i].bundlename), item->valuestring) != 0) {
                HILOG_ERROR(HILOG_MODULE_ACE, "strcpy_s faild");
            }
        } else {
            memset_s(eventList[i].bundlename, sizeof(eventList[i].bundlename), 0, sizeof(eventList[i].bundlename));
        }

        // Parse isAllDay
        item = cJSON_GetObjectItemCaseSensitive(eventObj, "isAllDay");
        eventList[i].isAllDay = (item && cJSON_IsBool(item)) ? item->valueint : false;

        // Parse hasReminder
        item = cJSON_GetObjectItemCaseSensitive(eventObj, "hasReminder");
        eventList[i].hasReminder = (item && cJSON_IsBool(item)) ? item->valueint : false;

        // Parse isRepeat
        item = cJSON_GetObjectItemCaseSensitive(eventObj, "isRepeat");
        eventList[i].isRepeat = (item && cJSON_IsBool(item)) ? (int64_t)item->valuedouble : false;

        // Parse recurrenceFrequency
        item = cJSON_GetObjectItemCaseSensitive(eventObj, "recurrenceFrequency");
        eventList[i].recurrenceFrequency = (item && cJSON_IsNumber(item)) ? (int64_t)item->valuedouble : -1;

        // Parse eventContent
        item = cJSON_GetObjectItemCaseSensitive(eventObj, "eventContent");
        if (item && cJSON_IsString(item)) {
            if(strcpy_s((char *)eventList[i].eventContent, sizeof(eventList[i].eventContent), item->valuestring) != 0) {
                HILOG_ERROR(HILOG_MODULE_ACE, "strcpy_s faild");
            }
        } else {
            memset_s(
                eventList[i].eventContent, sizeof(eventList[i].eventContent), 0, sizeof(eventList[i].eventContent));
        }
        i++;
    }

    // Fill event array structure
    eventArray->events = eventList;
    eventArray->count = eventCount;

    // Clean up resources
    cJSON_Delete(root);

    *error = CALENDAR_SUCCESS;
    return eventArray;
}

// 5. Delete single event
CalendarError CalendarDeleteEvent(const char *filename, const char *name, int eventId)
{
    if (!filename || !name) {
        return CALENDAR_INVALID_PARAMETER;
    }

    CalendarError error = CALENDAR_INVALID_PARAMETER;
    cJSON *root = getOrCreateRoot(filename, &error);
    if (!root || error != CALENDAR_SUCCESS) {
        return error;
    }

    cJSON *user = findUserObject(root, name);
    if (!user) {
        cJSON_Delete(root);
        return CALENDAR_USER_NOT_FOUND;
    }

    cJSON *events = cJSON_GetObjectItemCaseSensitive(user, "events");
    if (!events || !cJSON_IsArray(events)) {
        cJSON_Delete(root);
        return CALENDAR_EVENT_NOT_FOUND;
    }

    int index = findEventIndex(events, eventId);
    if (index == -1) {
        cJSON_Delete(root);
        return CALENDAR_EVENT_NOT_FOUND;
    }

    // Delete event
    cJSON_DeleteItemFromArray(events, index);
    error = writeJsonFile(filename, root);
    cJSON_Delete(root);

    return error;
}

// 6. Batch delete events
CalendarError CalendarDeleteEvents(const char *filename, const char *name, const int *eventIds, size_t idCount)
{
    if (!filename || !name || !eventIds || idCount == 0) {
        return CALENDAR_INVALID_PARAMETER;
    }

    CalendarError error = CALENDAR_INVALID_PARAMETER;
    cJSON *root = getOrCreateRoot(filename, &error);
    if (!root || error != CALENDAR_SUCCESS) {
        return error;
    }

    cJSON *user = findUserObject(root, name);
    if (!user) {
        cJSON_Delete(root);
        return CALENDAR_USER_NOT_FOUND;
    }

    cJSON *events = cJSON_GetObjectItemCaseSensitive(user, "events");
    if (!events || !cJSON_IsArray(events)) {
        cJSON_Delete(root);
        return CALENDAR_EVENT_NOT_FOUND;
    }

    // Mark indices of events to delete (sorted from largest to smallest to avoid index changes)
    int *indices = (int *)malloc(sizeof(int) * idCount);
    if (!indices) {
        cJSON_Delete(root);
        return CALENDAR_MEMORY_ALLOC_FAILED;
    }

    size_t foundCount = 0;
    for (size_t i = 0; i < idCount; i++) {
        int index = findEventIndex(events, eventIds[i]);
        if (index != -1) {
            indices[foundCount++] = index;
        }
    }

    // Delete from largest index to smallest to avoid index shifting issues
    for (int i = foundCount - 1; i >= 0; i--) {
        cJSON_DeleteItemFromArray(events, indices[i]);
    }

    free(indices);
    error = writeJsonFile(filename, root);
    cJSON_Delete(root);

    return error;
}

// 7. Modify/add event (using CalendarEvent struct for parameters)
CalendarError CalendarUpdateEvent(const char *filename, const char *name, const CalendarEvent *event)
{
    if (!filename || !name || !event) {
        return CALENDAR_INVALID_PARAMETER;
    }

    CalendarError error = CALENDAR_INVALID_PARAMETER;
    cJSON *root = getOrCreateRoot(filename, &error);
    if (!root || error != CALENDAR_SUCCESS) {
        return error;
    }

    cJSON *user = findUserObject(root, name);
    if (!user) {
        cJSON_Delete(root);
        return CALENDAR_USER_NOT_FOUND;
    }

    cJSON *events = cJSON_GetObjectItemCaseSensitive(user, "events");
    if (!events || !cJSON_IsArray(events)) {
        events = cJSON_CreateArray();
        if (!events) {
            cJSON_Delete(root);
            return CALENDAR_MEMORY_ALLOC_FAILED;
        }
        cJSON_AddItemToObject(user, "events", events);
    }

    // Find if event exists
    int index = findEventIndex(events, event->id);
    if (index != -1) {
        // Event exists, delete old event first
        cJSON_DeleteItemFromArray(events, index);
        // Add new event (whether update or add)
        cJSON *eventJson = createEventJson(event);
        if (!eventJson) {
            cJSON_Delete(root);
            return CALENDAR_MEMORY_ALLOC_FAILED;
        }
        cJSON_AddItemToArray(events, eventJson);
        error = writeJsonFile(filename, root);
    } else {
        error = CALENDAR_EVENT_NOT_FOUND;
    }

    cJSON_Delete(root);
    return error;
}

CalendarError CalendarUpdateEvents(const char *filename, const char *name, const CalendarEventArray *events)
{
    // Parameter validity check
    if (!filename || !name || !events || !events->events || events->count == 0) {
        return CALENDAR_INVALID_PARAMETER;
    }

    CalendarError error = CALENDAR_INVALID_PARAMETER;
    cJSON *root = getOrCreateRoot(filename, &error);
    if (!root || error != CALENDAR_SUCCESS) {
        return error;
    }

    // Find user object
    cJSON *user = findUserObject(root, name);
    if (!user) {
        cJSON_Delete(root);
        return CALENDAR_USER_NOT_FOUND;
    }

    // Get or create events array
    cJSON *eventsArray = cJSON_GetObjectItemCaseSensitive(user, "events");
    if (!eventsArray || !cJSON_IsArray(eventsArray)) {
        eventsArray = cJSON_CreateArray();
        if (!eventsArray) {
            cJSON_Delete(root);
            return CALENDAR_MEMORY_ALLOC_FAILED;
        }
        cJSON_AddItemToObject(user, "events", eventsArray);
    }

    // Record whether any events were successfully updated
    bool hasUpdated = false;

    // Iterate through all events to update
    for (size_t i = 0; i < events->count; i++) {
        const CalendarEvent *event = &events->events[i];

        // Find event position in JSON array
        int index = findEventIndex(eventsArray, event->id);
        if (index != -1) {
            // Found event, delete old version first
            cJSON_DeleteItemFromArray(eventsArray, index);

            // Create new event JSON object
            cJSON *eventJson = createEventJson(event);
            if (!eventJson) {
                cJSON_Delete(root);
                return CALENDAR_MEMORY_ALLOC_FAILED;
            }

            // Add updated event
            cJSON_AddItemToArray(eventsArray, eventJson);
            hasUpdated = true;
        }
    }

    // If any events were updated, write to file
    if (hasUpdated) {
        error = writeJsonFile(filename, root);
    } else {
        // No events found to update
        error = CALENDAR_EVENT_NOT_FOUND;
    }

    // Clean up resources
    cJSON_Delete(root);
    return error;
}

// 8. Get user information
CalendarAccount *GetUserInfo(const char *filename, const char *name, CalendarError *error)
{
    if (!filename || !name || !error) {
        *error = CALENDAR_INVALID_PARAMETER;
        return nullptr;
    }

    char *jsonData = ReadJsonFile(filename, error);
    if (!jsonData || *error != CALENDAR_SUCCESS) {
        return nullptr;
    }

    cJSON *root = cJSON_Parse(jsonData);
    free(jsonData);
    jsonData = nullptr;
    if (!root) {
        *error = CALENDAR_JSON_PARSE_ERROR;
        return nullptr;
    }

    cJSON *user = findUserObject(root, name);
    if (!user) {
        cJSON_Delete(root);
        *error = CALENDAR_USER_NOT_FOUND;
        return nullptr;
    }

    cJSON *accountObj = cJSON_GetObjectItemCaseSensitive(user, "CalendarAccount");
    if (!accountObj) {
        cJSON_Delete(root);
        *error = CALENDAR_JSON_PARSE_ERROR;
        return nullptr;
    }

    // Allocate memory for account information structure
    CalendarAccount *account = (CalendarAccount *)malloc(sizeof(CalendarAccount));
    if (!account) {
        cJSON_Delete(root);
        *error = CALENDAR_MEMORY_ALLOC_FAILED;
        return nullptr;
    }
    memset_s(account, sizeof(CalendarAccount), 0, sizeof(CalendarAccount));

    // Extract account information
    cJSON *item = cJSON_GetObjectItemCaseSensitive(accountObj, "name");
    if (item && cJSON_IsString(item)) {
        account->name = (char *)ace_malloc(strlen(item->valuestring) + 1);
        if (!account->name) {
            free(account);
            cJSON_Delete(root);
            *error = CALENDAR_MEMORY_ALLOC_FAILED;
            return nullptr;
        }
        memset_s(account->name, (strlen(item->valuestring) + 1), 0, (strlen(item->valuestring) + 1));
        if(memcpy_s(account->name,
            strlen(item->valuestring) + 1, item->valuestring, strlen(item->valuestring) + 1) != EOK) {
            HILOG_ERROR(HILOG_MODULE_ACE, "memcpy_s faild");
        }
    }

    item = cJSON_GetObjectItemCaseSensitive(accountObj, "type");
    if (item && cJSON_IsString(item)) {
        account->type = (char *)ace_malloc(strlen(item->valuestring) + 1);
        if (!account->type) {
            ace_free(account->name);
            free(account);
            cJSON_Delete(root);
            *error = CALENDAR_MEMORY_ALLOC_FAILED;
            return nullptr;
        }
        memset_s(account->type, (strlen(item->valuestring) + 1), 0, (strlen(item->valuestring) + 1));
        if(memcpy_s(account->type, strlen(item->valuestring) + 1, item->valuestring, strlen(item->valuestring) + 1) != EOK) {
            HILOG_ERROR(HILOG_MODULE_ACE, "memcpy_s faild");
        }
    }

    item = cJSON_GetObjectItemCaseSensitive(accountObj, "displayName");
    if (item && cJSON_IsString(item)) {
        account->displayName = (char *)ace_malloc(strlen(item->valuestring) + 1);
        if (!account->displayName) {
            ace_free(account->name);
            ace_free(account->type);
            free(account);
            cJSON_Delete(root);
            *error = CALENDAR_MEMORY_ALLOC_FAILED;
            return nullptr;
        }
        memset_s(account->displayName, (strlen(item->valuestring) + 1), 0, (strlen(item->valuestring) + 1));
        if(memcpy_s(account->displayName,
            strlen(item->valuestring) + 1, item->valuestring, strlen(item->valuestring) + 1) != EOK) {
            HILOG_ERROR(HILOG_MODULE_ACE, "memcpy_s faild");
        }
    }

    cJSON_Delete(root);
    *error = CALENDAR_SUCCESS;
    return account;
}

// Free event memory
void FreeCalendarEvent(CalendarEvent *event)
{
    if (event) {
        free(event);
    }
}

// Helper function to free CalendarEventArray resources
void FreeCalendarEventArray(CalendarEventArray *array)
{
    if (array) {
        free(static_cast<void*>(array->events));  // Free event array
        free(array);                  // Free array structure
    }
}

// Convert timestamp to datetime string
// Return 0 on success, -1 on failure
int TimestampToDatetime(time_t timestamp, char *datetimeStr, size_t maxLen)
{
    if (datetimeStr == nullptr || maxLen < 20) {  // At least 20 bytes needed for "YYYY-MM-DD HH:MM:SS"
        return -1;
    }

    struct tm *tminfo = localtime(&timestamp);  // Convert to local time
    if (tminfo == nullptr) {
        return -1;
    }

    // Format datetime string
    strftime(datetimeStr, maxLen, "%Y-%m-%d %H:%M:%S", tminfo);
    return 0;
}

}  // namespace ACELite
}  // namespace OHOS
