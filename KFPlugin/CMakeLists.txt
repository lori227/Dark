set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -shared")

macro(PLUGIN_PROPERTIES_MACRO project_name)
    aux_source_directory(. KF_SRC)
    add_library(${project_name} SHARED ${KF_SRC})
    add_dependencies(${project_name} KFProtocol)
    add_dependencies(${project_name} KFZConfig)

    set_target_properties(${project_name} PROPERTIES PREFIX "")
    set_target_properties(${project_name} PROPERTIES OUTPUT_NAME_DEBUG "${project_name}d")
    set_target_properties(${project_name} PROPERTIES
        FOLDER "KFPlugin"
        ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR})

endmacro(PLUGIN_PROPERTIES_MACRO project_name)


set(FRAME_LIB_DEBUG KFGlobald KFLoggerd KFPlugind KFMemoryd KFZConfigd KFUtilityd)
set(FRAME_LIB_RELEASE KFGlobal KFLogger KFPlugin KFMemory KFZConfig KFUtility)

macro(LINK_PLUGIN_MACRO project_name)
    if(CMAKE_BUILD_TYPE MATCHES "Debug") 
        target_link_libraries(${project_name} ${FRAME_LIB_DEBUG} ${EXTEND_LIB_DEBUG})
    else()
        target_link_libraries(${project_name} ${FRAME_LIB_RELEASE} ${EXTEND_LIB_RELEASE})
    endif()
endmacro(LINK_PLUGIN_MACRO project_name)

#############################################################################
## frame plugin param
macro(BUILD_PLUGIN_NO_PROTO project_name)
    ## properties
    PLUGIN_PROPERTIES_MACRO(${project_name})

    ## link
    LINK_PLUGIN_MACRO(${project_name})
endmacro(BUILD_PLUGIN_NO_PROTO project_name)
#############################################################################


#############################################################################
## logic plugin
macro(BUILD_PLUGIN_WITH_PROTO project_name)
    ## properties
    PLUGIN_PROPERTIES_MACRO(${project_name})

    ## extend
    set(EXTEND_LIB_DEBUG KFProtod KFProtocold protobuf ${EXTEND_LIB_DEBUG} )
    set(EXTEND_LIB_RELEASE KFProto KFProtocol protobuf ${EXTEND_LIB_DEBUG} )

    ## link
    LINK_PLUGIN_MACRO(${project_name})
endmacro(BUILD_PLUGIN_WITH_PROTO project_name)
#############################################################################
## logic plugin
macro(BUILD_PLUGIN_WITH_LOGIC project_name)
    ## properties
    PLUGIN_PROPERTIES_MACRO(${project_name})

    ## extend
    set(EXTEND_LIB_DEBUG KFProtod KFProtocold protobuf KFCored KFMath3Dd ${EXTEND_LIB_DEBUG} )
    set(EXTEND_LIB_RELEASE KFProto KFProtocol protobuf KFCore KFMath3D ${EXTEND_LIB_DEBUG} )

    ## link
    LINK_PLUGIN_MACRO(${project_name})
endmacro(BUILD_PLUGIN_WITH_LOGIC project_name)
#############################################################################




############################################################################################
############################################################################################
############################################################################################
############################################################################################
add_subdirectory(KFReward)
add_subdirectory(KFRole)
add_subdirectory(KFMatchClient)
add_subdirectory(KFMatchShard)
add_subdirectory(KFRoomClient)
add_subdirectory(KFRoomShard)
add_subdirectory(KFHero)
add_subdirectory(KFItem)
add_subdirectory(KFTrain)
add_subdirectory(KFRecruit)
add_subdirectory(KFGenerate)
add_subdirectory(KFClinic)
add_subdirectory(KFExplore)
add_subdirectory(KFChapter)
add_subdirectory(KFRecordClient)
add_subdirectory(KFRecordShard)
add_subdirectory(KFCamp)
add_subdirectory(KFChancel)
add_subdirectory(KFGranary)
add_subdirectory(KFSmithy)
add_subdirectory(KFTask)
add_subdirectory(KFSignIn)
add_subdirectory(KFBuild)
add_subdirectory(KFDialogue)
add_subdirectory(KFTLog)
