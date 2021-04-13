project(ProxyServerMain)

find_package(Qt5 COMPONENTS Network Core REQUIRED)

add_executable(ProxyServerMain
        main.cpp
        backend.cpp)

target_link_libraries(ProxyServerMain Qt5::Network Qt5::Core)