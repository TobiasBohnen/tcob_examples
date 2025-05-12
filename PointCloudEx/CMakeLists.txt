add_executable(PointCloudEx)

target_sources(PointCloudEx PRIVATE
    main.cpp
    PointCloudEx.cpp
)

set_target_properties(PointCloudEx PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(PointCloudEx PRIVATE -fexperimental-library -pthread)
    target_link_options(PointCloudEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(PointCloudEx PRIVATE tcob_static)
else()
    target_link_libraries(PointCloudEx PRIVATE tcob_shared)
endif()

target_include_directories(PointCloudEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET PointCloudEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:PointCloudEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
