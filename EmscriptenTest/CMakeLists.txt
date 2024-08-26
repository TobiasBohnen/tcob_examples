add_executable(EmscriptenTest)

target_sources(EmscriptenTest PRIVATE
    main.cpp
)

set_target_properties(EmscriptenTest PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

target_compile_options(EmscriptenTest PRIVATE -fexperimental-library -pthread)
target_link_options(EmscriptenTest PRIVATE -sFULL_ES3)

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(EmscriptenTest PRIVATE tcob_static)
else()
    target_link_libraries(EmscriptenTest PRIVATE tcob_shared)
endif()

target_include_directories(EmscriptenTest PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET EmscriptenTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:EmscriptenTest> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
