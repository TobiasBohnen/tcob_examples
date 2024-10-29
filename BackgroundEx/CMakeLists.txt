add_executable(BackgroundEx)

target_sources(BackgroundEx PRIVATE
    main.cpp
    BackgroundEx.cpp
)

set_target_properties(BackgroundEx PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(BackgroundEx PRIVATE -fexperimental-library -pthread)
    configure_file(backgroundex.zip ${CMAKE_BINARY_DIR}/backgroundex.zip COPYONLY)
    target_link_options(BackgroundEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread --embed-file backgroundex.zip)
else()
    configure_file(backgroundex.zip ${CMAKE_BINARY_DIR}/../../x64/backgroundex.zip COPYONLY)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(BackgroundEx PRIVATE tcob_static)
else()
    target_link_libraries(BackgroundEx PRIVATE tcob_shared)
endif()

target_include_directories(BackgroundEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET BackgroundEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:BackgroundEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
