/**
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: appmanger_module module.
 * Author: CompanyName
 * Create: 2025-09-04
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sys/time.h>
#include <cerrno>
#include <iostream>
#include "cJSON.h"
#include "calendar_utils.h"
#include "msg_center_msg.h"
#include "msg_center_client.h"
#include "event_id_manager.h"

namespace OHOS {
namespace ACELite {
#define ALLOC_CALENDAR_EVENTID_FAILD -1
#define MINUTE_IN_MILLISECONDS 60000
// Global containers (after splitting)
std::map<int64_t, CalendarEvent *> reminderEventMap;  // Reminder event container
std::map<int64_t, CalendarEvent *> normalEventMap;    // Non-reminder event container
std::unordered_set<int64_t> usedEventIds;             // Used ID set
int64_t g_reminderEventCount = 0;                       // Reminder event count

/**
 * @brief Read file content into a string
 */
static char *ReadFileToString(const char *filePath)
{
    if (!filePath) {
        printf("[ERROR] File path is nullptr\n");
        return nullptr;
    }

    FILE *file = fopen(filePath, "r");
    if (!file) {
        printf("[ERROR] Failed to open file %s (errno: %d)\n", filePath, errno);
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (fileSize < 0) {
        printf("[ERROR] Failed to get file size (errno: %d)\n", errno);
        fclose(file);
        return nullptr;
    }

    char *buffer = (char *)malloc(fileSize + 1);
    if (!buffer) {
        printf("[ERROR] Malloc buffer failed\n");
        fclose(file);
        return nullptr;
    }

    size_t readSize = fread(buffer, 1, fileSize, file);
    if (readSize != static_cast<size_t>(fileSize)) {
        printf("[ERROR] Failed to read file content\n");
        free(buffer);
        fclose(file);
        return nullptr;
    }
    buffer[readSize] = '\0';

    fclose(file);
    return buffer;
}

/**
 * @brief Parse calendar event from JSON object
 */
static CalendarEvent *parseCalendarEvent(cJSON *eventJson)
{
    if (!eventJson) {
        return nullptr;
    }

    CalendarEvent *event = (CalendarEvent *)malloc(sizeof(CalendarEvent));
    if (!event) {
        return nullptr;
    }
    memset_s(event, sizeof(CalendarEvent), 0, sizeof(CalendarEvent));

    cJSON *idJson = cJSON_GetObjectItem(eventJson, "id");
    cJSON *typeJson = cJSON_GetObjectItem(eventJson, "type");
    cJSON *startTimeJson = cJSON_GetObjectItem(eventJson, "startTime");
    cJSON *endTimeJson = cJSON_GetObjectItem(eventJson, "endTime");
    cJSON *bundlenameJson = cJSON_GetObjectItem(eventJson, "bundlename");
    cJSON *isAllDayJson = cJSON_GetObjectItem(eventJson, "isAllDay");
    cJSON *hasReminderJson = cJSON_GetObjectItem(eventJson, "hasReminder");
    cJSON *isRepeatJson = cJSON_GetObjectItem(eventJson, "isRepeat");
    cJSON *recurrenceFrequencyJson = cJSON_GetObjectItem(eventJson, "recurrenceFrequency");
    cJSON *eventContentJson = cJSON_GetObjectItem(eventJson, "eventContent");

    event->id = idJson ? idJson->valueint : 0;
    event->type = typeJson ? typeJson->valueint : 0;
    event->startTime = startTimeJson ? (int64_t)startTimeJson->valuedouble : 0;
    event->endTime = endTimeJson ? (int64_t)endTimeJson->valuedouble : 0;

    if (bundlenameJson && bundlenameJson->valuestring) {
        strncpy_s(event->bundlename, sizeof(event->bundlename) - 1,
            bundlenameJson->valuestring, sizeof(event->bundlename) - 1);
    }

    event->isAllDay = isAllDayJson ? (isAllDayJson->type == cJSON_True) : false;
    event->hasReminder = hasReminderJson ? (hasReminderJson->type == cJSON_True) : false;
    event->isRepeat = isRepeatJson ? (isRepeatJson->type == cJSON_True) : false;
    event->recurrenceFrequency = recurrenceFrequencyJson ? recurrenceFrequencyJson->valuedouble : 0;

    if (eventContentJson && eventContentJson->valuestring) {
        strncpy_s(event->eventContent, sizeof(event->eventContent) - 1,
            eventContentJson->valuestring, sizeof(event->eventContent) - 1);
    }
    return event;
}

/**
 * @brief Get current timestamp (in milliseconds)
 */
static int64_t GetCurrentTimestamp()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return static_cast<int64_t>(
        static_cast<uint64_t>(tv.tv_sec) * SEC_TO_MILLI +
        static_cast<uint64_t>(tv.tv_usec) / SEC_TO_MILLI
    );
}

/**
 * @brief Calendar timer callback function
 */
static void CalendarTimerCallback(void *arg)
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    uint64_t currentTimeMs = GetCurrentTimestamp();

    // Use iterator for traversal to support safe element deletion
    auto it = reminderEventMap.begin();
    while (it != reminderEventMap.end()) {
        // Remove const qualifier since we need to modify startTime and endTime
        CalendarEvent *event = it->second;
        if (!event) {
            // Handle null pointer event, delete directly
            it = reminderEventMap.erase(it);
            g_reminderEventCount--;
            continue;
        }

        // Logic correction: consider "triggered" when current time exceeds start time
        bool isTriggered = (currentTimeMs > event->startTime);
        if (isTriggered) {
            MsgCenterSendMsg(CALENDAR_TO_MSGCENTER, 0, (void *)event, sizeof(CalendarEvent));
            if (event->isRepeat) {
                // Calculate next trigger time for recurring events
                uint64_t times = (currentTimeMs - event->startTime) / event->recurrenceFrequency;
                event->startTime += (times + 1) * event->recurrenceFrequency;
                event->endTime += (times + 1) * event->recurrenceFrequency;
                ++it;  // Move to next element
            } else {
                // Insert key-value pair (not iterator) to normalEventMap
                normalEventMap.insert(*it);
                // Remove from reminderEventMap and move iterator
                it = reminderEventMap.erase(it);
                g_reminderEventCount--;
            }
        } else {
            // Untriggered events: keep in reminder list
            ++it;  // Move to next element
        }
    }
}

// Check if it's a valid reminder event
static bool CheckValidReminderEvent(int64_t currentTimeMs, CalendarEvent *event)
{
    bool isActiveReminder = false;
    if (event->hasReminder) {
        bool isExpired = (currentTimeMs > event->startTime);
        if (!(isExpired && !event->isRepeat)) {
            isActiveReminder = true;
            if (isExpired && event->isRepeat) {
                uint64_t times = (currentTimeMs - event->startTime) / event->recurrenceFrequency;
                event->startTime += ((times + 1) * event->recurrenceFrequency);
                event->endTime += ((times + 1) * event->recurrenceFrequency);
            }
        }
    }
    return isActiveReminder;
}

/**
 * @brief Initialize event maps from JSON file
 */
bool InitEventMaps()
{
    ClearAllEvents();
    char *jsonContent = ReadFileToString(GetCanlendarFilePath());
    if (!jsonContent) {
        return false;
    }
    cJSON *root = cJSON_Parse(jsonContent);
    free(jsonContent);
    if (!root) {
        return false;
    }
    int64_t currentTimeMs = GetCurrentTimestamp();

    cJSON *userNode = nullptr;
    cJSON_ArrayForEach(userNode, root)
    {
        cJSON *userProp = nullptr;
        cJSON_ArrayForEach(userProp, userNode)
        {
            if (strcmp(userProp->string, "events") == 0 && cJSON_IsArray(userProp)) {
                cJSON *eventNode = nullptr;
                cJSON_ArrayForEach(eventNode, userProp)
                {
                    CalendarEvent *event = parseCalendarEvent(eventNode);
                    if (!event) {
                        continue;
                    }

                    usedEventIds.insert(event->id);
                    if (CheckValidReminderEvent(currentTimeMs, event)) {
                        reminderEventMap[event->id] = event;
                        g_reminderEventCount++;
                    } else {
                        normalEventMap[event->id] = event;
                    }
                }
            }
        }
    }
    CalendarTimerInit(MINUTE_IN_MILLISECONDS, CalendarTimerCallback, nullptr, true);

    cJSON_Delete(root);
    return true;
}

/**
 * @brief Clear all event data
 */
void ClearAllEvents()
{
    // Free reminder event memory
    for (auto &pair : reminderEventMap) {
        if (pair.second)
            free(pair.second);
    }
    reminderEventMap.clear();

    // Free non-reminder event memory
    for (auto &pair : normalEventMap) {
        if (pair.second)
            free(pair.second);
    }
    normalEventMap.clear();

    usedEventIds.clear();
    g_reminderEventCount = 0;
}

/**
 * @brief Check if specified ID is already in use
 */
bool IsEventIdInUse(int64_t id)
{
    if (id < MIN_EVENT_ID || id > MAX_EVENT_ID) {
        return false;
    }
    return usedEventIds.find(id) != usedEventIds.end();
}

/**
 * @brief Automatically allocate ID and set event
 */
int64_t AllocateAndSetCalendarEvent(CalendarEvent *event)
{
    if (!event) {
        return ALLOC_CALENDAR_EVENTID_FAILD;
    }

    // Check reminder event count
    if (event->hasReminder && g_reminderEventCount >= MAX_REMINDER_COUNT) {
        fprintf(stderr, "Reminder events have reached maximum limit\n");
        return ALLOC_CALENDAR_EVENTID_FAILD;
    }
    int64_t currentTime = GetCurrentTimestamp();
    // Find available ID
    for (int64_t id = MIN_EVENT_ID; id <= MAX_EVENT_ID; ++id) {
        if (!IsEventIdInUse(id)) {
            event->id = id;
            usedEventIds.insert(id);

            // Add to corresponding container based on reminder property
            if (event->hasReminder && ((event->startTime > currentTime || event->isRepeat))) {
                reminderEventMap[id] = event;
                g_reminderEventCount++;
            } else {
                normalEventMap[id] = event;
            }
            return id;
        }
    }
    fprintf(stderr, "No available event ID\n");
    return ALLOC_CALENDAR_EVENTID_FAILD;
}

/**
 * @brief Set event with specified ID
 */
bool SetCalendarEventWithSpecificId(int64_t id, CalendarEvent *event)
{
    if (!event || id < MIN_EVENT_ID || id > MAX_EVENT_ID) {
        return false;
    }
    if (IsEventIdInUse(id)) {
        return false;
    }

    // Check reminder event count
    if (event->hasReminder && g_reminderEventCount >= MAX_REMINDER_COUNT) {
        return false;
    }

    event->id = id;
    usedEventIds.insert(id);

    // Add to corresponding container based on reminder property
    if (event->hasReminder) {
        reminderEventMap[id] = event;
        g_reminderEventCount++;
    } else {
        normalEventMap[id] = event;
    }

    return true;
}

/**
 * @brief Free resources for event with specified ID
 */
int FreeEventId(int64_t id)
{
    if (id < MIN_EVENT_ID || id > MAX_EVENT_ID) {
        return 0;
    }
    if (!IsEventIdInUse(id)) {
        return 0;
    }

    // Free from corresponding container
    auto remIt = reminderEventMap.find(id);
    if (remIt != reminderEventMap.end()) {
        free(remIt->second);
        reminderEventMap.erase(remIt);
        g_reminderEventCount--;
        usedEventIds.erase(id);
        return 1;
    }

    auto normIt = normalEventMap.find(id);
    if (normIt != normalEventMap.end()) {
        free(normIt->second);
        normalEventMap.erase(normIt);
        usedEventIds.erase(id);
        return 1;
    }

    return 0;
}

/**
 * @brief Get event with specified ID
 */
CalendarEvent *GetCalendarEvent(int64_t id)
{
    if (id < MIN_EVENT_ID || id > MAX_EVENT_ID) {
        return nullptr;
    }

    auto remIt = reminderEventMap.find(id);
    if (remIt != reminderEventMap.end())
        return remIt->second;

    auto normIt = normalEventMap.find(id);
    if (normIt != normalEventMap.end())
        return normIt->second;

    return nullptr;
}

/**
 * @brief Get current count of reminder events
 */
int GetReminderEventCount()
{
    return g_reminderEventCount;
}

/**
 * @brief Check if more reminder events can be added
 */
bool CanAddMoreReminders()
{
    return g_reminderEventCount < MAX_REMINDER_COUNT;
}

/**
 * @brief Update non-reminder event to reminder event
 */
bool UpdateNonReminderToReminder(int64_t id, CalendarEvent *updatedEvent)
{
    if (id < MIN_EVENT_ID || id > MAX_EVENT_ID || !updatedEvent || !updatedEvent->hasReminder) {
        return false;
    }

    // Check if original event is a non-reminder event
    auto normIt = normalEventMap.find(id);
    if (normIt == normalEventMap.end() || !normIt->second) {
        return false;
    }

    // Check reminder event count
    if (g_reminderEventCount >= MAX_REMINDER_COUNT) {
        return false;
    }

    // Free original event, add new event to reminder container
    free(normIt->second);
    normalEventMap.erase(normIt);

    updatedEvent->id = id;
    reminderEventMap[id] = updatedEvent;
    g_reminderEventCount++;

    return true;
}

/**
 * @brief Timer initialization function (external implementation)
 */
void CalendarTimerInit(uint32_t intervalMs, void (*callback)(void *), void *arg, bool isLoop)
{
    // Timer implementation provided externally
    printf("Timer initialized, interval: %u ms\n", intervalMs);
}

/**
 * @brief Get reminder event container
 */
const std::map<int64_t, CalendarEvent *> &GetReminderEventMap()
{
    return reminderEventMap;
}

/**
 * @brief Get non-reminder event container
 */
const std::map<int64_t, CalendarEvent *> &GetNormalEventMap()
{
    return normalEventMap;
}

}  // namespace ACELite
}  // namespace OHOS
