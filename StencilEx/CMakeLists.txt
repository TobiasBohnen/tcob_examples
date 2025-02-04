add_executable(StencilEx)

target_sources(StencilEx PRIVATE
    main.cpp
    StencilEx.cpp
)

set_target_properties(StencilEx PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(StencilEx PRIVATE -fexperimental-library -pthread)
    target_link_options(StencilEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(StencilEx PRIVATE tcob_static)
else()
    target_link_libraries(StencilEx PRIVATE tcob_shared)
endif()

target_include_directories(StencilEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET StencilEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:StencilEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
