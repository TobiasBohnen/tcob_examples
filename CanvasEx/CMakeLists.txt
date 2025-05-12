add_executable(CanvasEx)

target_sources(CanvasEx PRIVATE
    main.cpp
    CanvasEx.cpp
)

set_target_properties(CanvasEx PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(CanvasEx PRIVATE -fexperimental-library -pthread)
    target_link_options(CanvasEx PRIVATE -sFULL_ES3 -sMAX_WEBGL_VERSION=2 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(CanvasEx PRIVATE tcob_static)
else()
    target_link_libraries(CanvasEx PRIVATE tcob_shared)
endif()

target_include_directories(CanvasEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET CanvasEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:CanvasEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
