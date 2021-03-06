include_directories( ${CMAKE_SOURCE_DIR}/kleopatra ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

if ( KDEPIM_ONLY_KLEO )
  add_definitions( -DKDEPIM_ONLY_KLEO )
  set( _kcm_kleopatra_kleo_only_SRCS
    ../utils/kleo_kicondialog.cpp
    )
else ( KDEPIM_ONLY_KLEO )
  set( _kcm_kleopatra_extra_libs ${KDE4_KIO_LIBS} )
endif ( KDEPIM_ONLY_KLEO )

if ( BUILD_libkleopatraclient )

   set( _kcm_kleopatra_libkleopatraclient_extra_SRCS
     smimevalidationconfigurationwidget.cpp
     smimevalidationconfigurationpage.cpp
     )
   kde4_add_ui_files( _kcm_kleopatra_libkleopatraclient_extra_SRCS
     smimevalidationconfigurationwidget.ui
     )
   kde4_add_kcfg_files( _kcm_kleopatra_libkleopatraclient_extra_SRCS
     ${CMAKE_SOURCE_DIR}/kleopatra/kcfg/smimevalidationpreferences.kcfgc
     )

   set( _kcm_kleopatra_libkleopatraclient_extra_LIBS
     kleopatraclientgui
     )

   set( _kcm_kleopatra_libkleopatraclient_extra_install_FILES
     kleopatra_config_smimevalidation.desktop
     )

else ( BUILD_libkleopatraclient )
  set( _kcm_kleopatra_libkleopatraclient_extra_SRCS
    )
  set( _kcm_kleopatra_libkleopatraclient_extra_LIBS
    )
  set( _kcm_kleopatra_libkleopatraclient_extra_install_FILES
    )
endif ( BUILD_libkleopatraclient )

set(kcm_kleopatra_PART_SRCS 
   ${_kcm_kleopatra_kleo_only_SRCS}
   dirservconfigpage.cpp 
   appearanceconfigpage.cpp 
   appearanceconfigwidget.cpp 
   dnorderconfigpage.cpp
   gnupgsystemconfigurationpage.cpp
   ${_kcm_kleopatra_libkleopatraclient_extra_SRCS}
   )

add_definitions( -DKDE_DEFAULT_DEBUG_AREA=5152 )

kde4_add_ui_files( kcm_kleopatra_PART_SRCS
  appearanceconfigwidget.ui
  smimevalidationconfigurationwidget.ui
  )

kde4_add_kcfg_files( kcm_kleopatra_PART_SRCS
  ${CMAKE_SOURCE_DIR}/kleopatra/kcfg/tooltippreferences.kcfgc
  ${CMAKE_SOURCE_DIR}/kleopatra/kcfg/emailoperationspreferences.kcfgc
  )

kde4_add_plugin(kcm_kleopatra ${kcm_kleopatra_PART_SRCS})

if (KDEPIM_ONLY_KLEO)
 target_link_libraries(kcm_kleopatra ${KDE4_KDEUI_LIBS} ${KDE4_KCMUTILS_LIBS} kleo ${_kcm_kleopatra_extra_libs} ${_kcm_kleopatra_libkleopatraclient_extra_LIBS})
else (KDEPIM_ONLY_KLEO)
 target_link_libraries(kcm_kleopatra ${KDE4_KDEUI_LIBS} ${KDE4_KUTILS_LIBS} kleo ${_kcm_kleopatra_extra_libs} ${_kcm_kleopatra_libkleopatraclient_extra_LIBS})
endif (KDEPIM_ONLY_KLEO) 


install(TARGETS kcm_kleopatra  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES
  kleopatra_config_dirserv.desktop
  kleopatra_config_appear.desktop
  kleopatra_config_dnorder.desktop
  kleopatra_config_gnupgsystem.desktop
  ${_kcm_kleopatra_libkleopatraclient_extra_install_FILES}
  DESTINATION ${SERVICES_INSTALL_DIR})

