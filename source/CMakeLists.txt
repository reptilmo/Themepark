add_executable(project
    defines.h
    math.h
    vec3.h
    system.h
    system.cpp
    logging.h
    logging.cpp
    input.h
    input.cpp
    themepark.h
    themepark.cpp
    main.cpp
)

target_link_libraries(project SDL3::SDL3)
target_link_libraries(project glad)
target_link_libraries(project OpenGL)
