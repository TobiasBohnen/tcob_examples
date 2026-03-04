add_executable(UniformBufferEx)

target_sources(UniformBufferEx PRIVATE
    main.cpp
    UniformBufferEx.cpp
)

set_target_properties(UniformBufferEx PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(UniformBufferEx PRIVATE -fexperimental-library -pthread)
    target_link_options(UniformBufferEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(UniformBufferEx PRIVATE tcob_static)
else()
    target_link_libraries(UniformBufferEx PRIVATE tcob_shared)
endif()

target_include_directories(UniformBufferEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET UniformBufferEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:UniformBufferEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
