qt1_wrap_moc(MOC_FILES
    kgoldrunner.h
    kgoldrunnerwidget.h
    kgrobj.h
)

add_executable(kgoldrunner
    main.cpp

    kgoldrunnerwidget.cpp
    kgrobj.cpp
    kgoldrunner.cpp

    ${MOC_FILES}
)
target_link_libraries(kgoldrunner
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    Qt::Qt1
    X11::X11
)

install(TARGETS kgoldrunner RUNTIME DESTINATION ${KDE1_BINDIR})

install_icon(kgoldrunner.xpm)
install(FILES kgoldrunner.kdelnk DESTINATION ${KDE1_APPSDIR}/Games/)

install(FILES
    docs/en/editbar.png
    docs/en/editor.htm
    docs/en/enemy.png
    docs/en/genhints.htm
    docs/en/hero.png
    docs/en/history.htm
    docs/en/index.htm
    docs/en/install.htm
    docs/en/kgoldrunner.png
    docs/en/level010.png
    docs/en/menu.htm
    docs/en/play.htm
    docs/en/rules.htm
    docs/en/select.htm
    docs/en/select.png
    docs/en/tech.htm
    docs/en/tute008.png
    docs/en/typical.htm

    DESTINATION ${KDE1_HTMLDIR}/en/kgoldrunner
)
