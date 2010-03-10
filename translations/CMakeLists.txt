set(TARGET translations)

file(GLOB TRANSLATIONS_FILES "*.ts")
qt4_add_translation(QM_FILES ${TRANSLATIONS_FILES})

add_custom_target(${TARGET} ALL SOURCES ${QM_FILES})
