set(TARGET translations)

set(TRANSLATION_FILES
  urtconnector_ru.ts
)

qt4_add_translation(QM_FILES ${TRANSLATION_FILES})

add_custom_target(${TARGET} ALL SOURCES ${QM_FILES})

install(FILES 
  ${CMAKE_CURRENT_BINARY_DIR}/urtconnector_ru.qm 
DESTINATION share/urtconnector/translations)