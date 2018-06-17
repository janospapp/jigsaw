cmake_minimum_required(VERSION 2.8)
project(Puzzle)
set(CMAKE_CXX_FLAGS "-std=c++0x")

set(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/../resources/")

if(CMAKE_BUILD_TYPE STREQUAL Debug)
        set(INSTALL_DIR "${CMAKE_SOURCE_DIR}/../install/bin-debug/")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
        set(RES_DLL_DIR "${RESOURCE_DIR}dll/debug/")
else(CMAKE_BUILD_TYPE STREQUAL Debug)
        set(INSTALL_DIR "${CMAKE_SOURCE_DIR}/../install/bin/")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O4")
        set(RES_DLL_DIR "${RESOURCE_DIR}dll/release/")
endif(CMAKE_BUILD_TYPE STREQUAL Debug)

set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(SFML_ROOT C:/dev/Tools/lib/SFML)
set(TGUI_ROOT C:/dev/Tools/lib/TGUI)
set(BOOST_ROOT C:/dev/tools/lib)
set(Boost_ADDITIONAL_VERSIONS 1.57.0)
#set(JPEG_LIBRARY C:/dev/Tools/lib/jpeg-9a/lib)
#set(JPEG_INCLUDE_DIR C:/dev/Tools/lib/jpeg-9a/include)
find_package(SFML 2 COMPONENTS graphics window system REQUIRED)
find_package(TGUI)
find_package(Boost COMPONENTS filesystem system REQUIRED)
#find_package(JPEG)

include_directories(${SFML_INCLUDE_DIR} ${TGUI_INCLUDE_DIR} ${Boost_INCLUDE_DIR})# ${JPEG_INCLUDE_DIR})

#This is a reference for src/include, to make relative includes in the code
include_directories(include)

set(SOURCES cpp/GameCore/GameApp.cpp
			cpp/GameCore/GameClock.cpp
			cpp/GameCore/GameState.cpp
			cpp/GameCore/Main.cpp
			cpp/Managers/ImageManager.cpp
			cpp/Managers/InputManager.cpp
			cpp/Managers/FileManager.cpp
			cpp/Managers/GuiManager.cpp
			cpp/Managers/PuzzleManager.cpp
			cpp/Managers/WindowManager.cpp
			cpp/Puzzle/PieceBorder.cpp
			cpp/Puzzle/Curve.cpp
			cpp/Puzzle/Edge.cpp
			cpp/Puzzle/Line.cpp
			cpp/Puzzle/Link.cpp
			cpp/Puzzle/Piece.cpp
			cpp/Utils/Debug.cpp
			cpp/Utils/Helper.cpp
			cpp/Utils/INIParser.cpp
			cpp/Utils/Random.cpp
			cpp/Utils/SelectBox.cpp
			cpp/Utils/Selecter.cpp
			cpp/Utils/SpriteBatch.cpp
			cpp/Utils/XMLParser.cpp
			cpp/Utils/VertexSprite.cpp
			)

set(HEADERS include/GameCore/GameApp.hpp
			include/GameCore/GameClock.hpp
			include/GameCore/GameState.hpp
			include/Managers/ImageManager.hpp
			include/Managers/InputManager.hpp
			include/Managers/FileManager.hpp
			include/Managers/GuiManager.hpp
			include/Managers/PuzzleManager.hpp
			include/Managers/WindowManager.hpp
			include/Puzzle/PieceBorder.hpp
			include/Puzzle/Curve.hpp
			include/Puzzle/Edge.hpp
			include/Puzzle/Line.hpp
			include/Puzzle/Link.hpp
			include/Puzzle/Piece.hpp
			include/Utils/Config.hpp
			include/Utils/Debug.hpp
			include/Utils/Helper.hpp
			include/Utils/INIParser.hpp
			include/Utils/Random.hpp
			include/Utils/SelectBox.hpp
			include/Utils/Selecter.hpp
			include/Utils/SpriteBatch.hpp
			include/Utils/XMLParser.hpp
			include/Utils/VertexSprite.hpp
			)
			  
add_executable(jp ${SOURCES} ${HEADERS})
target_link_libraries(jp ${SFML_LIBRARIES})
target_link_libraries(jp ${TGUI_LIBRARY})
target_link_libraries(jp ${Boost_LIBRARIES})
#target_link_libraries(jp ${JPEG_LIBRARIES}/libjpeg.dll)
set(CMAKE_INSTALL_PREFIX ${INSTALL_DIR})

install(TARGETS jp DESTINATION ${INSTALL_DIR})

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(DEBUG_FILE_SYMBOL "-d")
else(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(DEBUG_FILE_SYMBOL "")
endif(CMAKE_BUILD_TYPE STREQUAL Debug)

install(DIRECTORY ${RESOURCE_DIR} DESTINATION ${INSTALL_DIR} REGEX "dll|update_libs.cmd" EXCLUDE)
install(DIRECTORY ${RES_DLL_DIR} DESTINATION ${INSTALL_DIR} PATTERN "*.dll")
