cmake_minimum_required(VERSION 2.8.3)
project(ssl-visual-planner)

add_compile_options(-std=c++14)

file(GLOB_RECURSE LibFiles "include/*.h")
add_custom_target(headers SOURCES ${LibFiles})

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)

set (${PROJECT_NAME}_SRCS
        src/agentplanclass.cpp
        src/tags.cpp
        src/playonplansql.cpp
        src/playon.cpp
        src/playoffplansql.cpp
        src/playoff.cpp
        src/mainwindow.cpp
        src/main.cpp
        src/
        )
set(${PROJECT_NAME}_HDRS
        include/agentplanclass.h
        include/ul.h
        include/tags.h
        include/playonplansql.h
        include/playon.h
        include/playoffplansql.h
        include/playoff.h
        include/mainwindow.h
        include/base.h
        )


set(${PROJECT_NAME}_UIS
        ui/mainwindow.ui
        ui/tags.ui
        )

qt5_wrap_cpp(${PROJECT_NAME}_MOCS ${${PROJECT_NAME}_HDRS})
qt5_wrap_ui(${PROJECT_NAME}_UIS_H ${${PROJECT_NAME}_UIS})
qt5_add_resources(${PROJECT_NAME}_UI_RESOURCES resource.qrc)


include_directories(
        include
        ${CMAKE_BINARY_DIR}
)

## Declare a cpp library
add_executable(${PROJECT_NAME}
        ${${PROJECT_NAME}_SRCS}
        ${${PROJECT_NAME}_MOCS}
        ${${PROJECT_NAME}_UIS_H}
        ${${PROJECT_NAME}_UI_RESOURCES}
        )

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Sql)
