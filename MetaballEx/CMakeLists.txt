add_executable(MetaballEx)

target_sources(MetaballEx PRIVATE
    main.cpp
    MetaballEx.cpp
)

set_target_properties(MetaballEx PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(MetaballEx PRIVATE -fexperimental-library -pthread)
    target_link_options(MetaballEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(MetaballEx PRIVATE tcob_static)
else()
    target_link_libraries(MetaballEx PRIVATE tcob_shared)
endif()

target_include_directories(MetaballEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET MetaballEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:MetaballEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
