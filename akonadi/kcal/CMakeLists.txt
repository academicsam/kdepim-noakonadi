include_directories(
    ${QT_QTDBUS_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII ${KDE4_ENABLE_EXCEPTIONS}" )

########### next target ###############

set( kcalakonadi_LIB_SRC
     kcalmodel.cpp
     kcalitembrowser.cpp
     itembrowser.cpp
)

kde4_add_library( akonadi-kcal_next SHARED ${kcalakonadi_LIB_SRC} )

target_link_libraries( akonadi-kcal_next  ${KDEPIMLIBS_AKONADI_LIBS} ${KDEPIMLIBS_AKONADI_KCAL_LIBS} ${KDEPIMLIBS_KCAL_LIBS} ${KDE4_KDEUI_LIBS} )
set_target_properties( akonadi-kcal_next PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )
install(TARGETS akonadi-kcal_next ${INSTALL_TARGETS_DEFAULT_ARGS})
