add_executable(ParticleSystemEx)

target_sources(ParticleSystemEx PRIVATE
    main.cpp
    ParticleSystemEx.cpp
)

set_target_properties(ParticleSystemEx PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(ParticleSystemEx PRIVATE -fexperimental-library -pthread)
    configure_file(particlesystemex.zip ${CMAKE_BINARY_DIR}/particlesystemex.zip COPYONLY)
    target_link_options(ParticleSystemEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -sALLOW_MEMORY_GROWTH -pthread --embed-file particlesystemex.zip)
else()
    configure_file(particlesystemex.zip ${CMAKE_BINARY_DIR}/../../x64/particlesystemex.zip COPYONLY)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(ParticleSystemEx PRIVATE tcob_static)
else()
    target_link_libraries(ParticleSystemEx PRIVATE tcob_shared)
endif()

target_include_directories(ParticleSystemEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET ParticleSystemEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:ParticleSystemEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
