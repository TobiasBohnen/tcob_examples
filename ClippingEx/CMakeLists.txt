add_executable(ClippingEx)

target_sources(ClippingEx PRIVATE
    main.cpp
    ClippingEx.cpp
)

set_target_properties(ClippingEx PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(ClippingEx PRIVATE -fexperimental-library -pthread)
    target_link_options(ClippingEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(ClippingEx PRIVATE tcob_static)
else()
    target_link_libraries(ClippingEx PRIVATE tcob_shared)
endif()

target_include_directories(ClippingEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET ClippingEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:ClippingEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
