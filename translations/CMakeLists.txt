set(TARGET translations)

set(TRANSLATION_FILES
  urtconnector_ru.ts
  urtconnector_ru.pl
)

find_package( Qt4 REQUIRED )

qt4_add_translation(QM_FILES ${TRANSLATION_FILES})

add_custom_target(${TARGET} ALL DEPENDS ${QM_FILES})

install(FILES 
  ${CMAKE_CURRENT_BINARY_DIR}/urtconnector_ru.qm 
  ${CMAKE_CURRENT_BINARY_DIR}/urtconnector_pl.qm 
DESTINATION share/urtconnector/translations)

