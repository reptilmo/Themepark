add_executable(project
    defines.h
    system.h
    system.cpp
    logging.h
    logging.cpp
    themepark.h
    themepark.cpp
    main.cpp
)

target_link_libraries(project glfw)
target_link_libraries(project glad)
target_link_libraries(project OpenGL)
