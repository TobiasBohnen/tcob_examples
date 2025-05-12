add_executable(PhysicsEx)

target_sources(PhysicsEx PRIVATE
    main.cpp
    PhysicsEx.cpp
)

set_target_properties(PhysicsEx PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(PhysicsEx PRIVATE -fexperimental-library -pthread)
    target_link_options(PhysicsEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(PhysicsEx PRIVATE tcob_static)
else()
    target_link_libraries(PhysicsEx PRIVATE tcob_shared)
endif()

target_include_directories(PhysicsEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET PhysicsEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:PhysicsEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
