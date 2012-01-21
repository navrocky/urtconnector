set(NSIS_VERSION_PREFIX "!define PRODUCT_VERSION")
if (DEFINED URT_BUILD)
    set(NSIS_PRODUCT_VERSION "${NSIS_VERSION_PREFIX} \"${URT_VERSION}.${URT_BUILD}\"")
else()
    set(NSIS_PRODUCT_VERSION "${NSIS_VERSION_PREFIX} \"${URT_VERSION}\"")
endif()

configure_file(urtconnector.nsi.in urtconnector.nsi @ONLY)