add_executable(CameraEx)

target_sources(CameraEx PRIVATE
    main.cpp
    CameraEx.cpp
)

set_target_properties(CameraEx PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(CameraEx PRIVATE -fexperimental-library -pthread)
    target_link_options(CameraEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(CameraEx PRIVATE tcob_static)
else()
    target_link_libraries(CameraEx PRIVATE tcob_shared)
endif()

target_include_directories(CameraEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET CameraEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:CameraEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
