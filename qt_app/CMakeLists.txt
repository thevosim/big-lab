set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
add_executable(qt_app
        main.cpp
)
target_link_libraries(qt_app
        Qt6::Widgets
        containers
)