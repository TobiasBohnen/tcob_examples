add_executable(MultiPassEx)

target_sources(MultiPassEx PRIVATE
    main.cpp
    MultiPassEx.cpp
)

set_target_properties(MultiPassEx PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(MultiPassEx PRIVATE -fexperimental-library -pthread)
    target_link_options(MultiPassEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(MultiPassEx PRIVATE tcob_static)
else()
    target_link_libraries(MultiPassEx PRIVATE tcob_shared)
endif()

target_include_directories(MultiPassEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET MultiPassEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:MultiPassEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
