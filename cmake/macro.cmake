# firstly you must define target
macro(define_target NAME)
    set(TARGET ${NAME})
    set(TARGET_SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")
endmacro()

macro(folder_files LIST)

    # relative path to target folder
    string(LENGTH "${TARGET_SOURCE_DIRECTORY}" TARGET_DIR_LENGTH)
    string(SUBSTRING ${CMAKE_CURRENT_SOURCE_DIR} ${TARGET_DIR_LENGTH} -1 FOLDER)

    foreach(CUR_FILE ${ARGN})
	list(APPEND ${LIST} "${FOLDER}/${CUR_FILE}")
    endforeach()
endmacro()

macro(export_files VAR_NAME)
    set(__FILE_LIST)
    folder_files(__FILE_LIST ${ARGN})
    list(APPEND __FILE_LIST "${${VAR_NAME}}")
    set(${VAR_NAME} ${__FILE_LIST} PARENT_SCOPE)
    unset(__FILE_LIST)
endmacro()
