add_executable(TweenEx)

target_sources(TweenEx PRIVATE
    main.cpp
    TweenEx.cpp
)

set_target_properties(TweenEx PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(TweenEx PRIVATE -fexperimental-library -pthread)
    target_link_options(TweenEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(TweenEx PRIVATE tcob_static)
else()
    target_link_libraries(TweenEx PRIVATE tcob_shared)
endif()

target_include_directories(TweenEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET TweenEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:TweenEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
