add_executable(SoundGeneratorEx)

target_sources(SoundGeneratorEx PRIVATE
    main.cpp
    SoundGeneratorEx.cpp
    UI.cpp
)

set_target_properties(SoundGeneratorEx PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(SoundGeneratorEx PRIVATE -fexperimental-library -pthread)
    target_link_options(SoundGeneratorEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(SoundGeneratorEx PRIVATE tcob_static)
else()
    target_link_libraries(SoundGeneratorEx PRIVATE tcob_shared)
endif()

target_include_directories(SoundGeneratorEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET SoundGeneratorEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:SoundGeneratorEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
