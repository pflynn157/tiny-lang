cmake_minimum_required(VERSION 3.0.0)
project(ide_src)

set(BASE_SRC
    actions.cpp
    editor.cpp
    main.cpp
    statusbar.cpp
    toolbar.cpp
    window.cpp
    icons.qrc
)

set(MENUBAR_SRC
    menubar/file_menu.cpp
    menubar/edit_menu.cpp
    menubar/help_menu.cpp
)

set(SRC
    ${BASE_SRC}
    ${MENUBAR_SRC}
)

add_executable(oride ${SRC})

target_link_libraries(oride
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    oride-highlighter
)
