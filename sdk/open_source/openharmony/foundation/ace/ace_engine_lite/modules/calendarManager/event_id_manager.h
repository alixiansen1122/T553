/**
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: appmanger_module module.
 * Author: CompanyName
 * Create: 2025-09-04
 */
#ifndef EVENT_ID_MANAGER_H
#define EVENT_ID_MANAGER_H

#include <cstdint>
#include <map>
#include <unordered_set>
#include "calendar_timer.h"
#include "calendar_manager.h"

namespace OHOS {
namespace ACELite {

// Event ID range constants
const int64_t MIN_EVENT_ID = 1;         // Minimum event ID
const int64_t MAX_EVENT_ID = 64;        // Maximum event ID
const int64_t MAX_REMINDER_COUNT = 32;  // Maximum number of reminder events

// Global container declarations
extern std::map<int64_t, CalendarEvent *> reminderEventMap;  // Reminder event container
extern std::map<int64_t, CalendarEvent *> normalEventMap;    // Non-reminder event container
extern std::unordered_set<int64_t> usedEventIds;             // Set of used IDs
extern int64_t g_reminderEventCount;

// Function declarations
/**
 * @brief Initialize event containers from JSON file (automatically split into reminder and non-reminder containers)
 * @param jsonFilePath Path to the JSON file
 * @return Returns true if initialization is successful, false otherwise
 */
bool InitEventMaps();

/**
 * @brief Clear all event containers and free memory
 */
void ClearAllEvents();

/**
 * @brief Check if the specified ID is already in use
 * @param id Event ID
 * @return Returns true if the ID is in use, false otherwise
 */
bool IsEventIdInUse(int64_t id);

/**
 * @brief Automatically allocate ID and add event (classified into corresponding container based on hasReminder
 * property)
 * @param event Pointer to the event
 * @return Returns the allocated ID on success, 0 on failure
 */
int64_t AllocateAndSetCalendarEvent(CalendarEvent *event);

/**
 * @brief Add event with specified ID (classified into corresponding container based on hasReminder property)
 * @param id Event ID
 * @param event Pointer to the event
 * @return Returns true on success, false on failure
 */
bool SetCalendarEventWithSpecificId(int64_t id, CalendarEvent *event);

/**
 * @brief Free the event with the specified ID
 * @param id Event ID
 * @return Returns 1 on success, 0 on failure
 */
int FreeEventId(int64_t id);

/**
 * @brief Get the event with the specified ID
 * @param id Event ID
 * @return Returns pointer to the event on success, nullptr on failure
 */
CalendarEvent *GetCalendarEvent(int64_t id);

/**
 * @brief Get the count of reminder events
 * @return Number of reminder events
 */
int GetReminderEventCount();

/**
 * @brief Check if more reminder events can be added
 * @return Returns true if more reminders can be added, false otherwise
 */
bool CanAddMoreReminders();

/**
 * @brief Update a non-reminder event to a reminder event
 * @param id Event ID
 * @param updatedEvent Updated event
 * @return Returns true on success, false on failure
 */
bool UpdateNonReminderToReminder(int64_t id, CalendarEvent *updatedEvent);

/**
 * @brief Timer initialization function
 * @param intervalMs Timer interval in milliseconds
 * @param callback Callback function
 * @param arg Callback argument
 * @param isLoop Whether to loop periodically
 */
void CalendarTimerInit(uint32_t intervalMs, void (*callback)(void *), void *arg, bool isLoop);

/**
 * @brief Get the reminder event container (for external traversal)
 * @return Constant reference to the reminder event container
 */
const std::map<int64_t, CalendarEvent *> &GetReminderEventMap();

/**
 * @brief Get the non-reminder event container (for external traversal)
 * @return Constant reference to the non-reminder event container
 */
const std::map<int64_t, CalendarEvent *> &GetNormalEventMap();

}  // namespace ACELite
}  // namespace OHOS

#endif  // EVENT_ID_MANAGER_H
