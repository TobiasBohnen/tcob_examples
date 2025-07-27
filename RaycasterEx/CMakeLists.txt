add_executable(RaycasterEx)

target_sources(RaycasterEx PRIVATE
    main.cpp
    RaycasterEx.cpp
)

set_target_properties(RaycasterEx PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(RaycasterEx PRIVATE -fexperimental-library -pthread)
    configure_file(raycasterex.zip ${CMAKE_BINARY_DIR}/raycasterex.zip COPYONLY)
    target_link_options(RaycasterEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread --embed-file raycasterex.zip)
else()
    configure_file(raycasterex.zip ${CMAKE_BINARY_DIR}/../../x64/raycasterex.zip COPYONLY)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(RaycasterEx PRIVATE tcob_static)
else()
    target_link_libraries(RaycasterEx PRIVATE tcob_shared)
endif()

target_include_directories(RaycasterEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET RaycasterEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:RaycasterEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
