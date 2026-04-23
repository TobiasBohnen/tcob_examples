add_executable(FSMEx)

target_sources(FSMEx PRIVATE
    main.cpp
    FSMEx.cpp
    Sim.cpp
)

set_target_properties(FSMEx PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(FSMEx PRIVATE -fexperimental-library -pthread)
    target_link_options(FSMEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(FSMEx PRIVATE tcob_static)
else()
    target_link_libraries(FSMEx PRIVATE tcob_shared)
endif()

target_include_directories(FSMEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET FSMEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:FSMEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
