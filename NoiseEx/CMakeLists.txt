add_executable(NoiseEx)

target_sources(NoiseEx PRIVATE
    main.cpp
    NoiseEx.cpp
)

set_target_properties(NoiseEx PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(NoiseEx PRIVATE -fexperimental-library -pthread)
    target_link_options(NoiseEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(NoiseEx PRIVATE tcob_static)
else()
    target_link_libraries(NoiseEx PRIVATE tcob_shared)
endif()

target_include_directories(NoiseEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET NoiseEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:NoiseEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
