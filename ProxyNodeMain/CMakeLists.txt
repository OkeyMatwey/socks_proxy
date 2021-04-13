project(ProxyNodeMain)

find_package(Qt5 COMPONENTS Network Core Gui Widgets REQUIRED)

add_executable(ProxyNodeMain
        mainwindow.ui
        main.cpp
        mainwindow.cpp
        worker.cpp)

target_link_libraries(ProxyNodeMain Qt5::Network Qt5::Core Qt5::Gui Qt5::Widgets)