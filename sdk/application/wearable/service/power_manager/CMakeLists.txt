set(COMPONENT_NAME "power_manager")
set(MODULE_NAME "app")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(SOURCES
${CMAKE_CURRENT_SOURCE_DIR}/src/display_state_machine.c
${CMAKE_CURRENT_SOURCE_DIR}/src/power_display_service.c
${CMAKE_CURRENT_SOURCE_DIR}/src/filters/gui_event_filter.c
${CMAKE_CURRENT_SOURCE_DIR}/src/filters/slice_event_filter.c
${CMAKE_CURRENT_SOURCE_DIR}/src/filters/timer_event_filter.c
${CMAKE_CURRENT_SOURCE_DIR}/src/actions/display_action.c
${CMAKE_CURRENT_SOURCE_DIR}/src/actions/timer_action.c
${CMAKE_CURRENT_SOURCE_DIR}/src/actions/touch_action.c

)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(PUBLIC_DEFINES
SUPPORT_POWER_MANAGER
)

set(PRIVATE_DEFINES
)

build_component()

