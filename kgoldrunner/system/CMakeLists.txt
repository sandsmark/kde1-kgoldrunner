install(FILES
    games.dat
    hi_level.dat
    hi_plws.dat
    hi_plwv.dat
    hi_wad.dat

    DESTINATION ${KDE1_DATADIR}/kgoldrun/
)

install(DIRECTORY
    levels/

    DESTINATION ${KDE1_DATADIR}/kgoldrun
)

