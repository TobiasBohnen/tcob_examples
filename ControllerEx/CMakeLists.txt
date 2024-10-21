add_executable(ControllerEx)

target_sources(ControllerEx PRIVATE
    main.cpp
    ControllerEx.cpp
)

set_target_properties(ControllerEx PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(ControllerEx PRIVATE -fexperimental-library -pthread)
    target_link_options(ControllerEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(ControllerEx PRIVATE tcob_static)
else()
    target_link_libraries(ControllerEx PRIVATE tcob_shared)
endif()

target_include_directories(ControllerEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET ControllerEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:ControllerEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
