cmake_minimum_required(VERSION 3.19)
project(batch-watermarker LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets)

qt_standard_project_setup()

qt_add_executable(batch-watermarker
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    core/watermark.cpp
    core/watermark.h
)

target_include_directories(batch-watermarker PRIVATE core)

target_link_libraries(batch-watermarker
    PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

install(TARGETS batch-watermarker
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET batch-watermarker
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})