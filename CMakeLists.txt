cmake_minimum_required(VERSION 3.19)
project(practica_5 LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(practica_5 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    particula.h particula.cpp
    caja.h
    obstaculo.h
    fusion.h
    simulacion.h simulacion.cpp
    lectorsimulacion.h lectorsimulacion.cpp
)

target_link_libraries(practica_5
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS practica_5
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET practica_5
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
