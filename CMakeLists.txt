project( equalizer )

cmake_minimum_required( VERSION 2.8.11 )


# root of your msvc14 x64 prebuild, there should be a `/lib/cmake` dir

set( CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{QT5_DIR} )

set( CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "C:/Qt/5.11.2/msvc2015_64" ) # remember to add `C:\Qt\VS2015x64\Qt5.6.0\5.6\msvc2015_64\bin` to %PATH%

set( CMAKE_INCLUDE_CURRENT_DIR ON )

set( CMAKE_AUTOMOC ON )


find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets Multimedia)  #find_package( Qt5Widgets REQUIRED )

qt5_wrap_ui( UI_HEADERS mainwindow.ui )

qt5_add_resources( QRCS buttons.qrc )

add_executable( ${PROJECT_NAME} main.cpp mainwindow.cpp pitcher.cpp Wave.cpp ${UI_HEADERS} ${QRCS} )

target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Multimedia) #target_link_libraries( ${PROJECT_NAME} Qt5::Widgets )


# auto copy dlls if on windows
if(WIN32)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Core> $<TARGET_FILE_DIR:${PROJECT_NAME}>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Gui> $<TARGET_FILE_DIR:${PROJECT_NAME}>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Widgets> $<TARGET_FILE_DIR:${PROJECT_NAME}> 
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Multimedia> $<TARGET_FILE_DIR:${PROJECT_NAME}> )
endif(WIN32)
