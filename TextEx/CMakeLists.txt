add_executable(TextEx)

target_sources(TextEx PRIVATE
    main.cpp
    TextEx.cpp
)

set_target_properties(TextEx PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(TextEx PRIVATE -fexperimental-library -pthread)
    configure_file(textex.zip ${CMAKE_BINARY_DIR}/textex.zip COPYONLY)
    target_link_options(TextEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread --embed-file textex.zip)
else()
    configure_file(textex.zip ${CMAKE_BINARY_DIR}/../../x64/textex.zip COPYONLY)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(TextEx PRIVATE tcob_static)
else()
    target_link_libraries(TextEx PRIVATE tcob_shared)
endif()

target_include_directories(TextEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET TextEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:TextEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
