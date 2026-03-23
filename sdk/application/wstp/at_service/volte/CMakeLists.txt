set(COMPONENT_NAME "volte_at_service")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/volte_at_process.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER

)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_AT_VOLTE
)

# use this when you want to add ccflags like -include xxx
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

build_component()
