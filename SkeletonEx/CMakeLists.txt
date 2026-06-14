add_executable(SkeletonEx)

target_sources(SkeletonEx PRIVATE
    main.cpp
    SkeletonEx.cpp
)

set_target_properties(SkeletonEx PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(SkeletonEx PRIVATE -fexperimental-library -pthread)
    target_link_options(SkeletonEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(SkeletonEx PRIVATE tcob_static)
else()
    target_link_libraries(SkeletonEx PRIVATE tcob_shared)
endif()

target_include_directories(SkeletonEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET SkeletonEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:SkeletonEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
