add_executable(project
    defines.h
    math.h
    vec2.h
    vec3.h
    mat4.h
    memory.h
    memory.cpp
    dynarray.h
    dynarray.cpp
    system.h
    system.cpp
    logging.h
    logging.cpp
    input.h
    input.cpp
    image.h
    image.cpp
    mesh.h
    mesh.cpp
    camera.h
    camera.cpp
    renderer.h
    renderer.cpp
    themepark.h
    themepark.cpp
    main.cpp
)

target_link_libraries(project SDL3::SDL3)
target_link_libraries(project glad)
target_link_libraries(project OpenGL)

add_custom_command(
    TARGET project POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:project>/assets/
)

add_custom_command(
    TARGET project POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:project>/shaders/
)
