add_executable(MusicEx)

target_sources(MusicEx PRIVATE
    main.cpp
    MusicEx.cpp
    UI.cpp
)

set_target_properties(MusicEx PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(MusicEx PRIVATE -fexperimental-library -pthread)
    configure_file(musicex.zip ${CMAKE_BINARY_DIR}/musicex.zip COPYONLY)
    target_link_options(MusicEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread --embed-file musicex.zip)
else()
    configure_file(musicex.zip ${CMAKE_BINARY_DIR}/../../x64/musicex.zip COPYONLY)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(MusicEx PRIVATE tcob_static)
else()
    target_link_libraries(MusicEx PRIVATE tcob_shared)
endif()

target_include_directories(MusicEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET MusicEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:MusicEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
