cmake_minimum_required (VERSION 3.30)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

project (LoopyMSE LANGUAGES C CXX ASM)
set (PROJECT_VERSION 0.5.3)
set (PROJECT_DESCRIPTION "Loopy My Seal Emulator")
set (PROJECT_HOMEPAGE_URL "https://github.com/LoopyMSE/LoopyMSE")
set (PROJECT_ORG "LoopyMSE")

string (REPLACE "/DNDEBUG" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

### DEPENDENCIES
include (FetchContent)

# libboost used for program_options
# currently binaries are fetched via packager + statically linked in at build time,
# as we have had no issues with this so far and Boost is a large monolithic thing to build, and we don't need to customize anything
set (Boost_USE_STATIC_LIBS ON CACHE BOOL "" FORCE)
find_package (Boost REQUIRED COMPONENTS program_options)

# SDL2, SDL2_image
# Due to package managers pulling shenanigans like replacing SDL2 with SDL2-compat,
# and because a non-customized sdl2-image build will indlude absurd number of dependencies for unused image formats,
# we use CMake to fetch and build these from source
set (SDL_TESTS OFF CACHE BOOL "" FORCE)
set (SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set (SDL_EXAMPLES OFF CACHE BOOL "" FORCE)

# Static linking IS an option, however, since we are already in the business of bundling dependencies for distribution,
# for now we'll bundle our own SDL2.dll
set (SDL_STATIC OFF CACHE BOOL "" FORCE)
set (SDL_SHARED ON CACHE BOOL "" FORCE)

add_compile_definitions (SDL_MAIN_HANDLED) # Prevents SDL from touching main()

FetchContent_Declare (SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.32.10
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable (SDL2)

set (SDL2IMAGE_INSTALL OFF CACHE BOOL "" FORCE)
set (SDL2IMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
set (SDL2IMAGE_TESTS OFF CACHE BOOL "" FORCE)

set (SDL2IMAGE_WEBP OFF CACHE BOOL "" FORCE)
set (SDL2IMAGE_AVIF OFF CACHE BOOL "" FORCE)
set (SDL2IMAGE_JXL OFF CACHE BOOL "" FORCE)
set (SDL2IMAGE_TIF OFF CACHE BOOL "" FORCE)

set (SDL2IMAGE_PNG ON CACHE BOOL "" FORCE)
set (SDL2IMAGE_JPG ON CACHE BOOL "" FORCE)

set (SDL2IMAGE_VENDORED OFF CACHE BOOL "" FORCE)
FetchContent_Declare (SDL2_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-2.8.12
    GIT_SHALLOW TRUE
    # For PNG/JPEG with vendored off we shouldn't need to fetch any image libraries from submodules, speeds build time
    GIT_SUBMODULES ""
)
FetchContent_MakeAvailable (SDL2_image)

set (DIST_DIR ${PROJECT_BINARY_DIR}/dist)
set (ASSETS_DIR ${PROJECT_SOURCE_DIR}/assets)

add_subdirectory (src)