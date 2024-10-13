add_executable(SoundFontEx)

target_sources(SoundFontEx PRIVATE
    main.cpp
    SoundFontEx.cpp
)

set_target_properties(SoundFontEx PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(SoundFontEx PRIVATE -fexperimental-library -pthread)
    configure_file(soundfontex.zip ${CMAKE_BINARY_DIR}/soundfontex.zip COPYONLY)
    target_link_options(SoundFontEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread --embed-file soundfontex.zip)
else()
    configure_file(soundfontex.zip ${CMAKE_BINARY_DIR}/../../x64/soundfontex.zip COPYONLY)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(SoundFontEx PRIVATE tcob_static)
else()
    target_link_libraries(SoundFontEx PRIVATE tcob_shared)
endif()

target_include_directories(SoundFontEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET SoundFontEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:SoundFontEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
