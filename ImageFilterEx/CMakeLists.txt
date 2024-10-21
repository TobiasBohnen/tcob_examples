add_executable(ImageFilterEx)

target_sources(ImageFilterEx PRIVATE
    main.cpp
    ImageFilterEx.cpp
)

set_target_properties(ImageFilterEx PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(ImageFilterEx PRIVATE -fexperimental-library -pthread)
    target_link_options(ImageFilterEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(ImageFilterEx PRIVATE tcob_static)
else()
    target_link_libraries(ImageFilterEx PRIVATE tcob_shared)
endif()

target_include_directories(ImageFilterEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET ImageFilterEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:ImageFilterEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
