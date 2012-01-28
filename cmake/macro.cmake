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

#######################

macro(cl_define_target NAME)
    set(TARGET ${NAME})
    set(TARGET_SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")
endmacro()

macro(__cl_is_root_folder)
    unset(IS_ROOT_FOLDER)
    string(LENGTH "${TARGET_SOURCE_DIRECTORY}" TARGET_DIR_LENGTH)
    string(LENGTH "${CMAKE_CURRENT_SOURCE_DIR}" CURRENT_DIR_LENGTH)
    if(NOT (${CURRENT_DIR_LENGTH} GREATER ${TARGET_DIR_LENGTH}))
        set(IS_ROOT_FOLDER true)
    endif()
endmacro()

macro(__cl_folder_files LIST)

    # relative path to target folder
    string(LENGTH "${TARGET_SOURCE_DIRECTORY}" TARGET_DIR_LENGTH)
    string(LENGTH "${CMAKE_CURRENT_SOURCE_DIR}" CURRENT_DIR_LENGTH)
    if(${CURRENT_DIR_LENGTH} GREATER ${TARGET_DIR_LENGTH})
        math(EXPR LEN "${CURRENT_DIR_LENGTH}-${TARGET_DIR_LENGTH}")
        string(SUBSTRING ${CMAKE_CURRENT_SOURCE_DIR} ${TARGET_DIR_LENGTH} ${LEN} FOLDER)
        set(FOLDER "${FOLDER}/")
    else()
        set(FOLDER "")
    endif()

    foreach(CUR_FILE ${ARGN})
        list(APPEND ${LIST} "${FOLDER}${CUR_FILE}")
    endforeach()
endmacro()

macro(cl_export_files VAR_NAME)
    set(__FILE_LIST)
    __cl_folder_files(__FILE_LIST ${ARGN})
    list(APPEND __FILE_LIST "${${VAR_NAME}}")

    __cl_is_root_folder()
    if(IS_ROOT_FOLDER)
        set(${VAR_NAME} ${__FILE_LIST})
    else()
        set(${VAR_NAME} ${__FILE_LIST} PARENT_SCOPE)
    endif()
    unset(__FILE_LIST)
endmacro()

macro(cl_define_headers)
    cl_export_files(CL_HEADERS ${ARGN})
endmacro()

macro(cl_define_moc)
    cl_export_files(CL_MOC ${ARGN})
endmacro()

macro(cl_define_sources)
    cl_export_files(CL_SOURCES ${ARGN})
endmacro()

macro(cl_define_resources)
    cl_export_files(CL_RESOURCES ${ARGN})
endmacro()

macro(cl_define_interfaces)
    cl_export_files(CL_IFACES ${ARGN})
endmacro()

macro(cl_define_deps)
    cl_export_files(CL_DEPS ${ARGN})
endmacro()

macro(cl_qt_required)
    set(CL_QT_REQUIRED_LIBS ${ARGN})
endmacro()

macro(__cl_source_group)
    foreach(CUR_FILE ${ARGN})
        string(REGEX MATCH "^(.+)/.+$" TMP ${CUR_FILE})
        if(CMAKE_MATCH_1)
            string(REPLACE "/" "\\" FOLDER ${CMAKE_MATCH_1})
            source_group(${FOLDER} FILES ${CUR_FILE})
            
            set(CMAKE_FILE ${CMAKE_MATCH_1}/CMakeLists.txt)
            source_group(${FOLDER} FILES ${CMAKE_FILE})
        else()
            source_group(\\ FILES ${CUR_FILE})
            source_group(\\ FILES CMakeLists.txt)
        endif()
    endforeach()
endmacro()

macro(cl_implement_common)

    __cl_source_group(${CL_HEADERS})
    __cl_source_group(${CL_MOC})
    __cl_source_group(${CL_SOURCES})
    __cl_source_group(${CL_IFACES})
    __cl_source_group(${CL_RESOURCES})

    unset(__QT_LIBRARIES)
    if(CL_QT_REQUIRED_LIBS)
        find_package(Qt4 REQUIRED ${CL_QT_REQUIRED_LIBS})
        include(${QT_USE_FILE})
        include_directories(${CMAKE_CURRENT_BINARY_DIR})
        if(CL_MOC)
            qt4_wrap_cpp(CL_SOURCES ${CL_MOC})
        endif()
        if(CL_IFACES)
            qt4_wrap_ui(CL_SOURCES ${CL_IFACES})
        endif()
        if(CL_RESOURCES)
            qt4_add_resources(CL_SOURCES ${CL_RESOURCES})
        endif()
        set(__QT_LIBRARIES ${QT_LIBRARIES})
    endif()
#    target_link_libraries(${TARGET} ${__QT_LIBRARIES} ${CL_DEPS})
endmacro()


macro(cl_implement_library KIND)
    cl_implement_common()
    add_library(${TARGET} ${KIND} ${CL_HEADERS} ${CL_SOURCES})
    target_link_libraries(${TARGET} ${__QT_LIBRARIES} ${CL_DEPS})
endmacro()

macro(cl_implement_executable KIND)

    set(PARAM WIN32)
    cl_implement_common()
    add_executable(${TARGET} ${PARAM} ${CL_HEADERS} ${CL_SOURCES})
    target_link_libraries(${TARGET} ${__QT_LIBRARIES} ${CL_DEPS})
endmacro()


macro(cl_proxy_files_to_parent)
    set(CL_HEADERS ${CL_HEADERS} PARENT_SCOPE)
    set(CL_SOURCES ${CL_SOURCES} PARENT_SCOPE)
    set(CL_MOC ${CL_MOC} PARENT_SCOPE)
    set(CL_RESOURCES ${CL_RESOURCES} PARENT_SCOPE)
    set(CL_IFACES ${CL_IFACES} PARENT_SCOPE)
    set(CL_DEPS ${CL_DEPS} PARENT_SCOPE)
endmacro()

macro(cl_add_subdirectory DIR)
    add_subdirectory(${DIR})

    __cl_is_root_folder()
    if(NOT IS_ROOT_FOLDER)
        cl_proxy_files_to_parent()
    endif()
endmacro()
