add_executable(LSystemEx)

target_sources(LSystemEx PRIVATE
    main.cpp
    LSystemEx.cpp
    UI.cpp
)

set_target_properties(LSystemEx PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(LSystemEx PRIVATE -fexperimental-library -pthread)
    target_link_options(LSystemEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(LSystemEx PRIVATE tcob_static)
else()
    target_link_libraries(LSystemEx PRIVATE tcob_shared)
endif()

target_include_directories(LSystemEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET LSystemEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:LSystemEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
