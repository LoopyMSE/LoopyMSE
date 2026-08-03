add_executable(LoopyMSE
        "config.h"
        "options.h"
        "options.cpp"
        "imgwriter.h"
        "imgwriter.cpp"
        "main.cpp")

target_compile_definitions(LoopyMSE PRIVATE
        PROJECT_NAME="${PROJECT_NAME}"
        PROJECT_ORG="${PROJECT_ORG}"
        PROJECT_DESCRIPTION="${PROJECT_DESCRIPTION}"
        PROJECT_VERSION="${PROJECT_VERSION}"
)

if (WIN32)
    target_sources(LoopyMSE PRIVATE "windows.rc")
    target_link_options(LoopyMSE PRIVATE -mwindows -static-libgcc -static-libstdc++)
elseif (APPLE)
    set(ICON_NAME "macos.icns")
    set(ICON_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${ICON_NAME})
    set_source_files_properties(${ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_target_properties(LoopyMSE PROPERTIES 
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_ICONFILE ${ICON_NAME}
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
endif ()

target_link_libraries(LoopyMSE PRIVATE core SDL2::SDL2-static SDL2_image::SDL2_image-static Boost::program_options)

# INSTALLATION

install(TARGETS LoopyMSE
        RUNTIME_DEPENDENCY_SET LoopyMSEDeps
        RUNTIME DESTINATION ${DIST_DIR}
        BUNDLE DESTINATION ${DIST_DIR})

if(WIN32)
    install(RUNTIME_DEPENDENCY_SET LoopyMSEDeps
        DESTINATION ${DIST_DIR}
        # DIRECTORIES
        #     "C:/msys64/mingw64/bin"
        PRE_EXCLUDE_REGEXES
            "api-ms-"
            "ext-ms-"
        POST_EXCLUDE_REGEXES
            ".*/System32/.*"
    )
    install(DIRECTORY ${ASSETS_DIR}/
            DESTINATION ${DIST_DIR})
    install(CODE "file(TOUCH \"${DIST_DIR}/__PLACE_BIOS.BIN_HERE__\")")
    install(CODE "file(TOUCH \"${DIST_DIR}/__PLACE_SOUNDBIOS.BIN_HERE__\")")
elseif(APPLE)
    install(RUNTIME_DEPENDENCY_SET LoopyMSEDeps
        DESTINATION ${DIST_DIR}/LoopyMSE.app/Contents/Frameworks)
    install(DIRECTORY ${ASSETS_DIR}/
        DESTINATION ${DIST_DIR}/LoopyMSE.app/Contents/Resources)
    install(FILES ${ICON_PATH}
        DESTINATION ${DIST_DIR}/LoopyMSE.app/Contents/Resources)
    install(FILES ${ASSETS_DIR}/README.md 
        DESTINATION ${DIST_DIR})
endif()
