add_executable (LoopyMSE
        "config.h"
        "options.h"
        "options.cpp"
        "imgwriter.h"
        "imgwriter.cpp"
        "main.cpp"
)

target_link_libraries (LoopyMSE PRIVATE
    core
    SDL2
    SDL2_image
    Boost::program_options
)

target_compile_definitions (LoopyMSE PRIVATE
    PROJECT_NAME="${PROJECT_NAME}"
    PROJECT_ORG="${PROJECT_ORG}"
    PROJECT_DESCRIPTION="${PROJECT_DESCRIPTION}"
    PROJECT_VERSION="${PROJECT_VERSION}"
)

if (WIN32)
    target_sources (LoopyMSE PRIVATE "windows.rc")
    target_link_options (LoopyMSE PRIVATE -mwindows -static-libgcc -static-libstdc++)
elseif (APPLE)
    set (CMAKE_OSX_DEPLOYMENT_TARGET "15.0" CACHE STRING "" FORCE)
    set (CMAKE_MACOSX_RPATH ON)
    set (ICON_NAME "macos.icns")
    set (ICON_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${ICON_NAME})
    set_source_files_properties (${ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_target_properties (LoopyMSE PROPERTIES 
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_ICONFILE ${ICON_NAME}
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
    )
endif ()


# INSTALLATION

install (TARGETS LoopyMSE
    RUNTIME_DEPENDENCY_SET LoopyMSEDeps
    RUNTIME DESTINATION ${DIST_DIR}
    BUNDLE DESTINATION ${DIST_DIR}
)

if (WIN32)
    install (DIRECTORY ${ASSETS_DIR}/ DESTINATION ${DIST_DIR})
    # Find anything that MSYS2/build toolchain provided that we haven't statically linked already
    # Currently this should be just one: libwinpthread-1.dll
    get_filename_component (WIN_BUILDTOOLS_DIR ${CMAKE_C_COMPILER} DIRECTORY)
    install (RUNTIME_DEPENDENCY_SET LoopyMSEDeps
        DESTINATION ${DIST_DIR}
        DIRECTORIES
            ${sdl2_BINARY_DIR}
            ${sdl2_image_BINARY_DIR}
            ${WIN_BUILDTOOLS_DIR}
        PRE_EXCLUDE_REGEXES
            "api-ms-.*"
            "ext-ms-.*"
    	POST_EXCLUDE_REGEXES
	    	[[.*(\\|/)system32(\\|/).*\.dll]]
    )
    # We know exactly where SDL2.dll, SDL2_image.dll are and we know we need them
    # This may be redundant to the step above, but (doing this last) is good insurance that it's not shipping an incorrect SDL2.dll provided by the system
    install (FILES $<TARGET_FILE:SDL2> DESTINATION ${DIST_DIR})
    install (FILES $<TARGET_FILE:SDL2_image> DESTINATION ${DIST_DIR})
elseif (APPLE)
    install (DIRECTORY ${ASSETS_DIR}/ DESTINATION ${DIST_DIR}/LoopyMSE.app/Contents/Resources)
    install (FILES ${ICON_PATH} DESTINATION ${DIST_DIR}/LoopyMSE.app/Contents/Resources)
    install (FILES ${ASSETS_DIR}/README.md DESTINATION ${DIST_DIR})

    # This copies .dylibs to Frameworks/ but the system will not find them there as it's not searching there:
    # install(RUNTIME_DEPENDENCY_SET LoopyMSEDeps
    #     DESTINATION ${DIST_DIR}/LoopyMSE.app/Contents/Frameworks)

    # This copies AND relocates the libraries
    install (CODE "
        include (BundleUtilities)
        fixup_bundle (\"${DIST_DIR}/LoopyMSE.app\" \"\" \"${sdl2_BINARY_DIR};${sdl2_image_BINARY_DIR};/opt/homebrew/lib;/opt/homebrew/opt/lib;/usr/local/lib\" \"\")
    ")
endif ()

# Touch these files as a reminder that no IP is shipped with LoopyMSE
install (CODE "file (TOUCH \"${DIST_DIR}/__PLACE_BIOS.BIN_HERE__\")")
install (CODE "file (TOUCH \"${DIST_DIR}/__PLACE_SOUNDBIOS.BIN_HERE__\")")
