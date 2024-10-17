add_executable(HtmlEx)

target_sources(HtmlEx PRIVATE
    main.cpp
    HtmlEx.cpp
)

set_target_properties(HtmlEx PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(HtmlEx PRIVATE -fexperimental-library -pthread)
    configure_file(htmlex.zip ${CMAKE_BINARY_DIR}/htmlex.zip COPYONLY)
    target_link_options(HtmlEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread --embed-file htmlex.zip)
else()
    configure_file(htmlex.zip ${CMAKE_BINARY_DIR}/../../x64/htmlex.zip COPYONLY)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(HtmlEx PRIVATE tcob_static)
else()
    target_link_libraries(HtmlEx PRIVATE tcob_shared)
endif()

target_include_directories(HtmlEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET HtmlEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:HtmlEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
