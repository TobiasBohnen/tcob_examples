add_executable(LightingSystemEx)

target_sources(LightingSystemEx PRIVATE
    main.cpp
    LightingSystemEx.cpp
)

set_target_properties(LightingSystemEx PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(LightingSystemEx PRIVATE -fexperimental-library -pthread)
    target_link_options(LightingSystemEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(LightingSystemEx PRIVATE tcob_static)
else()
    target_link_libraries(LightingSystemEx PRIVATE tcob_shared)
endif()

target_include_directories(LightingSystemEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET LightingSystemEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:LightingSystemEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
