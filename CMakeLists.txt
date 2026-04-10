cmake_minimum_required(VERSION 3.5)


project(DetectorDemo VERSION 0.1 LANGUAGES CXX)


set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 COMPONENTS Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Multimedia Quick REQUIRED)

#find_package(OpenCV REQUIRED )
# Use external ONNX
#if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
#    find_package(onnxruntime REQUIRED PATHS /usr/local/opt/onnxruntime_1.10)
#endif()


set(ONNX_LIB_PATH "${ONNX_INSTALL_DIR}/lib")
set(onnxruntime_INCLUDE_DIRS "${ONNX_INSTALL_DIR}/include")
message(STATUS "ONNX_LIB_PATH : ${ONNX_LIB_PATH}")
message(STATUS "onnxruntime_INCLUDE_DIRS : ${onnxruntime_INCLUDE_DIRS}")
message(STATUS "SRC_BLUR_LIB : ${SRC_BLUR_LIB}")


if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

    set(onnxruntime_LIBRARY ${ONNX_LIB_PATH}/onnxruntime_common.lib
                        ${ONNX_LIB_PATH}/onnxruntime_session.lib
                        ${ONNX_LIB_PATH}/onnxruntime_optimizer.lib
                        ${ONNX_LIB_PATH}/onnxruntime_providers.lib
                        ${ONNX_LIB_PATH}/onnxruntime_util.lib
                        ${ONNX_LIB_PATH}/onnxruntime_framework.lib
                        ${ONNX_LIB_PATH}/onnxruntime_graph.lib
                        ${ONNX_LIB_PATH}/onnxruntime_mlas.lib
                        ${ONNX_LIB_PATH}/onnxruntime_flatbuffers.lib
                        ${ONNX_LIB_PATH}/external/onnx/onnx.lib
                        ${ONNX_LIB_PATH}/external/onnx/onnx_proto.lib
                        ${ONNX_LIB_PATH}/external/flatbuffers/flatbuffers.lib
                        ${ONNX_LIB_PATH}/external/re2/re2.lib
                        winmm.lib
                        dbghelp.lib
                        shlwapi.lib
                      )

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(onnxruntime_LIBRARY ${onnxruntime_LIBRARY}
                    ${ONNX_LIB_PATH}/external/protobuf/libprotobuf-lited.lib
                    ${ONNX_LIB_PATH}/external/protobuf/libprotobufd.lib)
    else()
        set(onnxruntime_LIBRARY ${onnxruntime_LIBRARY}
                    ${ONNX_LIB_PATH}/external/protobuf/libprotobuf-lite.lib
                    ${ONNX_LIB_PATH}/external/protobuf/libprotobuf.lib)
    endif()

else()

    set(onnxruntime_LIBRARY ${ONNX_LIB_PATH}/libonnxruntime_session.a
                        ${ONNX_LIB_PATH}/libonnxruntime_optimizer.a
                        ${ONNX_LIB_PATH}/libonnxruntime_providers.a
                        ${ONNX_LIB_PATH}/libonnxruntime_util.a
                        ${ONNX_LIB_PATH}/libonnxruntime_framework.a
                        ${ONNX_LIB_PATH}/libonnxruntime_graph.a
                        ${ONNX_LIB_PATH}/libonnxruntime_common.a
                        ${ONNX_LIB_PATH}/libonnxruntime_mlas.a
                        ${ONNX_LIB_PATH}/libonnxruntime_flatbuffers.a
                        ${ONNX_LIB_PATH}/external/onnx/libonnx.a
                        ${ONNX_LIB_PATH}/external/onnx/libonnx_proto.a
                        ${ONNX_LIB_PATH}/external/protobuf/libprotobuf-lite.a
                        ${ONNX_LIB_PATH}/external/re2/libre2.a
                        ${ONNX_LIB_PATH}/external/flatbuffers/libflatbuffers.a
                        ${ONNX_LIB_PATH}/external/nsync/libnsync_cpp.a)

endif()

set(TARGET_NAME FaceDetector)

list (APPEND RESOURCES Resources.qrc)
list (APPEND RESOURCES ${THEME_BreezeStyleSheets}/breeze.qrc)

set(PROJECT_QML_SOURCES
        qml/qml.qrc)

set(PROJECT_SOURCES
        main.cpp
        mainprocess.cpp
        mainprocess.h
        RecognizerCore.h
        antispoofing/AntispoofingONNXRecognizer.cpp
        antispoofing/AntispoofingONNXRecognizer.h
        antispoofing/AntispoofingDetector.cpp
        antispoofing/AntispoofingDetector.h

        face/FaceDetectONNXRecognizer.cpp
        face/FaceDetectONNXRecognizer.h
        face/FaceDetectOpenCvRecognizer.cpp
        face/FaceDetectOpenCvRecognizer.h
        face/FaceDetector.cpp
        face/FaceDetector.h
        videosrc.cpp
        videosrc.h
        utils.cpp
        utils.h
        gui.h
        guiQuick.h
        guiQuick.cpp
        CppQmlObjects.h
        contour/ONNXContourRecognizer.cpp
        contour/ONNXContourRecognizer.h
        contour/Contour.cpp
        contour/Contour.h
        contour/ContourDetector.cpp
        contour/ContourDetector.h
        contour/ContourDebugger.h

        feature/ONNXVFeatureRecognizer.cpp
        feature/ONNXVFeatureRecognizer.h

        Rotate.h
        Types.h

        feature/FeatureDetector.cpp
        feature/FeatureDetector.h

        comboboxmodel.h
        comboboxmodel.cpp

        imageprovider.cpp
        imageprovider.h

        FaceHintResource.h
        FaceTurnHint.h

        faceprocessor.h faceprocessor.cpp

       # face/FaceDetectorCore.h
       # face3diviimpl.h face3diviimpl.cpp
       # face/facedetector3divi.h face/facedetector3divi.cpp

        Settings.h
        JsonCfg.cpp JsonCfg.h

        docfrontprocessor.h docfrontprocessor.cpp

        ${RESOURCES} )

if(DEFINED ${WITH_TFL})
    set(TFL_SOURCE_FILES
            contour/TFContourRecognizer.cpp
            contour/TFContourRecognizer.h
    )
    target_compile_definitions(${TARGET_NAME} PRIVATE WITH_TFL)
endif()



if(WIN32)
    add_executable(${TARGET_NAME}
        #WIN32                       # for without console
        ${PROJECT_SOURCES}
        ${PROJECT_QML_SOURCES}
    )

    #set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "/MANIFEST" )
    # Чтобы манифест был внешним файлом (не вшитым внутрь), используйте:
    #add_link_options("/MANIFEST:EMBED,NO")

else()
    add_executable(${TARGET_NAME}
        #MACOSX_BUNDLE
        ${PROJECT_SOURCES}
        ${PROJECT_QML_SOURCES}
    )
endif()

target_compile_definitions(${TARGET_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${TARGET_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia Qt${QT_VERSION_MAJOR}::Quick)

#set_target_properties(${TARGET_NAME} PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
#    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#)

add_dependencies(${TARGET_NAME} fake_onnx)

#include OpenCV
include_directories (${CMAKE_BINARY_DIR})
target_include_directories(${TARGET_NAME} PRIVATE ${OPENCV_DIR}/include)
target_include_directories(${TARGET_NAME} PRIVATE ${OPENCV_DIR}/modules/videoio/include)
target_include_directories(${TARGET_NAME} PRIVATE ${OPENCV_DIR}/modules/core/include)
target_include_directories(${TARGET_NAME} PRIVATE ${OPENCV_DIR}/modules/imgproc/include)
target_include_directories(${TARGET_NAME} PRIVATE ${OPENCV_DIR}/modules/highgui/include)
target_include_directories(${TARGET_NAME} PRIVATE ${OPENCV_DIR}/modules/video/include)
target_include_directories(${TARGET_NAME} PRIVATE ${OPENCV_DIR}/modules/imgcodecs/include)
target_include_directories(${TARGET_NAME} PRIVATE ${OPENCV_DIR}/modules/features2d/include)
target_include_directories(${TARGET_NAME} PRIVATE ${OPENCV_DIR}/modules/flann/include)
target_include_directories(${TARGET_NAME} PRIVATE ${OPENCV_DIR}/modules/calib3d/include)
target_include_directories(${TARGET_NAME} PRIVATE ${OPENCV_DIR}/modules/objdetect/include)
target_include_directories(${TARGET_NAME} PRIVATE ${onnxruntime_INCLUDE_DIRS})
target_include_directories(${TARGET_NAME} PRIVATE include)
target_include_directories(${TARGET_NAME} PRIVATE ${SRC_GLARE_LIB})
target_include_directories(${TARGET_NAME} PRIVATE ${SRC_BLUR_LIB})
#target_include_directories(${TARGET_NAME} PRIVATE ${SRC_BLUR_LIB}/../common_src)

target_link_libraries(${TARGET_NAME}  PRIVATE core_contour fake_onnx)
target_link_libraries(${TARGET_NAME}  PRIVATE core_types_all)
target_link_libraries(${TARGET_NAME}  PRIVATE opencv_core opencv_highgui opencv_objdetect)
target_link_libraries(${TARGET_NAME}  PRIVATE ${onnxruntime_LIBRARY})
target_link_libraries(${TARGET_NAME} PRIVATE glare_core blur_core)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_link_libraries(${TARGET_NAME}  PRIVATE pthread)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)
    target_link_libraries(${TARGET_NAME}  INTERFACE  PkgConfig::deps)
endif()

#--------------------------------------------------------------------------------------
# 3DiVi FaceSDK
#--------------------------------------------------------------------------------------
message(STATUS "FaceDetectorDemo: FACE_SDK_3DIVI_PATH : ${FACE_SDK_3DIVI_PATH}")
#target_compile_definitions(${TARGET_NAME} PRIVATE FACE_SDK_3DIVI="\\"${FACE_SDK_3DIVI_PATH}\\"")
#target_compile_definitions(${TARGET_NAME} PRIVATE FACE_SDK_3DIVI="${FACE_SDK_3DIVI_PATH}")
#target_include_directories(${TARGET_NAME} PRIVATE ${FACE_SDK_3DIVI_PATH}/include)
#if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
#    target_link_libraries(${TARGET_NAME} PRIVATE ${FACE_SDK_3DIVI_PATH}/bin/facerec.dll)
#else()
#    target_link_libraries(${TARGET_NAME} PRIVATE ${FACE_SDK_3DIVI_PATH}/lib/libfacerec.so)
#    target_link_libraries(${TARGET_NAME} PRIVATE  dl)
#endif()


if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  set(PACKAGE_NAME ${TARGET_NAME})
  set(PROJECT_NAME ${TARGET_NAME})

  set(ICON_FILE_PATH ${CMAKE_CURRENT_LIST_DIR}/res/Darwin/${TARGET_NAME}.icns)
  set(PLIST_FILE_PATH ${CMAKE_CURRENT_LIST_DIR}/res/Darwin/${TARGET_NAME}.plist)

  message(STATUS "ICON_FILE_PATH : ${ICON_FILE_PATH}")
  message(STATUS "PLIST_FILE_PATH : ${PLIST_FILE_PATH}")


  #set(CMAKE_MACOSX_BUNDLE ON)
  #set(MACOSX_BUNDLE ON)
  #set(MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/res/Darwin/${TARGET_NAME}.plist)
  # the below settings related to code signing should be set just after declaring a project name
  set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "11111111" CACHE STRING "")
  set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "Mac Developer" CACHE STRING "")

    set(MACOSX_BUNDLE_GUI_IDENTIFIER "org.net.${TARGET_NAME}")
    set(MACOSX_BUNDLE_BUNDLE_NAME "${TARGET_NAME}")
    set(MACOSX_BUNDLE_EXECUTABLE "${TARGET_NAME}")

  set_target_properties(${TARGET_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PLIST_FILE_PATH})
  set_target_properties(${TARGET_NAME} PROPERTIES MACOSX_BUNDLE ON)

    set(APPS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}.app")
    message(STATUS "APPS : ${APPS}")
else()
  set(PROJECT_NAME ${TARGET_NAME})
endif()


#MacOs Bundle
# use: make package
if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")

if((${USE_MACOSX_CPACK}) STREQUAL "ON")

    message(STATUS "USE_MACOSX_CPACK : ${USE_MACOSX_CPACK}")

    set(CPACK_PACKAGE_VERSION_MAJOR "1")
    set(CPACK_PACKAGE_VERSION_MINOR "0")
    set(CPACK_PACKAGE_VERSION_PATCH "0")

    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})



    set(CPACK_GENERATOR    Bundle)
    set(CPACK_BUNDLE_NAME  ${TARGET_NAME})
    set(CPACK_BUNDLE_ICON  ${ICON_FILE_PATH})
    set(CPACK_BUNDLE_PLIST ${PLIST_FILE_PATH})



    # Destination paths below are relative to ${CMAKE_INSTALL_PREFIX}
 #   install(TARGETS ${TARGET_NAME}
 #       BUNDLE DESTINATION . COMPONENT Runtime
 #       RUNTIME DESTINATION bin COMPONENT Runtime
 #   )

  #  install(TARGETS ${TARGET_NAME}
  #      BUNDLE DESTINATION ./bin COMPONENT Runtime
  #      RUNTIME DESTINATION bin COMPONENT Runtime
  #  )

    # Application
    #set(APPS "${CMAKE_BINARY_DIR}/${TARGET_NAME}")
    set(APPS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}.app")

    
    
    #set(APPS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}")
    message(STATUS "APPS : ${APPS}")

    # Dependencies
    set(DEPS "")
#    set(DEPS "libglib-2.0.0.dylib")
#    set(DEPS "/usr/local/lib/libglib-2.0.0.dylib")
    message(STATUS "DEPS : ${DEPS}")

    # Directories to look for dependencies
    set(DIRS ${CMAKE_BINARY_DIR}
             ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
             "/usr/local/lib"
             "/usr/local/opt")
    #set(DIRS "/usr/local/lib/")
    message(STATUS "DIRS : ${DIRS}")

    install(CODE "
            include(BundleUtilities)
            fixup_bundle(\"${APPS}\" \"${DEPS}\" \"${DIRS}\")
            ")

    include(CPack)

endif() #USE_MACOSX_CPACK
endif() 



if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FaceDetect)
endif()


if(DEFINED OPENCV_WITH_CUDA)
    set(OPENCV_HAAR_FILE  ${OPENCV_DIR}/data/haarcascades_cuda/haarcascade_frontalface_default.xml)
else()
    set(OPENCV_HAAR_FILE  ${OPENCV_DIR}/data/haarcascades/haarcascade_frontalface_default.xml)
endif()
message(STATUS "OPENCV_HAAR_FILEL: ${OPENCV_HAAR_FILE}")

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMENT "Copy HAAR from ${OPENCV_HAAR_FILE} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        COMMAND ${CMAKE_COMMAND} -E copy ${OPENCV_HAAR_FILE} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )

#------------------------------------------------------------------------------------------------
#
#  deploy for MacOS
#
#------------------------------------------------------------------------------------------------
if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")

    set(APPS_SAVE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}_save.app")
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND echo "Save APP copy  ${APPS} to ${APPS_SAVE}"
        COMMAND ${CMAKE_COMMAND} -E remove_directory  ${APPS_SAVE}
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${APPS} ${APPS_SAVE}
    )

    add_dependencies(${TARGET_NAME} macdylibbundler)

    set(DEPLOY_TARGET_APP_EXE_PATH ${APPS}/Contents/MacOS)
    set(DEPLOY_TARGET_APP_EXE ${DEPLOY_TARGET_APP_EXE_PATH}/${TARGET_NAME})
    set(DEPLOY_DEST_DIR ${APPS}/Contents/libs)
    message(STATUS "DEPLOY_TARGET_APP_EXE_PATH : ${DEPLOY_TARGET_APP_EXE_PATH}")
    message(STATUS "DEPLOY_TARGET_APP_EXE : ${DEPLOY_TARGET_APP_EXE}")
    message(STATUS "DEPLOY_DEST_DIR       : ${DEPLOY_DEST_DIR}")

    # copy dependencies libs
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
	COMMAND echo "Copy dependents of ${DEPLOY_TARGET_APP_EXE} overwrite dir"
	COMMAND ${MACDYLIBBINDLER_EXE} -od -b -x ${DEPLOY_TARGET_APP_EXE} -d ${DEPLOY_DEST_DIR}
    )
    
    # copy resources
    set(DEPLOY_RESOURCES_DEST_DIR ${APPS}/Contents/Resources)
    message(STATUS "DEPLOY_RESOURCES_DEST_DIR : ${DEPLOY_RESOURCES_DEST_DIR}")
    
    # copy resources
    set(DEPLOY_RESOURCES_DEST_DIR ${APPS}/Contents/Resources)
    message(STATUS "DEPLOY_RESOURCES_DEST_DIR : ${DEPLOY_RESOURCES_DEST_DIR}")
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND echo "Create resources dir"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${DEPLOY_RESOURCES_DEST_DIR}
    )

#    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
#	COMMAND echo "Copy models"
#	COMMAND mkdir ${DEPLOY_RESOURCES_DEST_DIR}/models
#	COMMAND cp -r ${CMAKE_CURRENT_LIST_DIR}/models/*  ${DEPLOY_RESOURCES_DEST_DIR}/models/
#	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/models/ ${DEPLOY_RESOURCES_DEST_DIR}/models/
#    )

    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
	COMMAND echo "Copy icon"
	COMMAND ${CMAKE_COMMAND} -E copy ${ICON_FILE_PATH} ${DEPLOY_RESOURCES_DEST_DIR}/
    )

    set(OPENCV_FFMPEG_PLUGIN_NAME  libopencv_videoio_ffmpeg.so)
    set(OPENCV_FFMPEG_PLUGIN  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../lib/${OPENCV_FFMPEG_PLUGIN_NAME})
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
	COMMAND echo "Copy OpenCV plugins: from ${OPENCV_FFMPEG_PLUGIN}   to   ${DEPLOY_TARGET_APP_EXE_PATH}/"
	COMMAND ${CMAKE_COMMAND} -E copy ${OPENCV_FFMPEG_PLUGIN} ${DEPLOY_TARGET_APP_EXE_PATH}/
    )

    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
	    COMMAND echo "Copy dependents of ${OPENCV_FFMPEG_PLUGIN} owerwrite files"
        COMMAND ${MACDYLIBBINDLER_EXE} -of -b -x ${DEPLOY_TARGET_APP_EXE_PATH}/${OPENCV_FFMPEG_PLUGIN_NAME} -d ${DEPLOY_DEST_DIR}
    )


    #/Users/initguest/work/MacOS_Build/scripts/test_data/FaceDetector.app  /Users/initguest/work/MacOS_Build/scripts/test_data/out FaceDetector /Users/initguest/work/MacOS_Build/scripts/test_data/Icon.icns
    # create fancy DMG installer 
    set(DEPLOY_FANCY_ICON 	/Users/initguest/work/MacOS_Build/scripts/test_data/Icon.icns)
    set(DEPLOY_FANCY_NAME 	${TARGET_NAME})
    set(DEPLOY_FANCY_BUNDLE	${APPS})
    set(DEPLOY_FANCY_OUT	${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
	   COMMAND echo "Create fancy DMG"
	   COMMAND ${DARWIN_FANCY_DMG} ${DEPLOY_FANCY_BUNDLE} ${DEPLOY_FANCY_OUT} ${DEPLOY_FANCY_NAME} ${DEPLOY_FANCY_ICON}
	   WORKING_DIRECTORY /Users/initguest/work/FaceDetect_prj/Installer/Darwin/fancy_dmg
    )
    
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
       COMMENT "Restore  copy APP" 
       COMMENT "rename  ${APPS_SAVE} to ${APPS}"
       COMMAND echo "Restore  copy APP"
       COMMAND echo "remove  ${APPS}"
	   COMMAND ${CMAKE_COMMAND} -E remove_directory ${APPS}
	   COMMAND echo "rename  ${APPS_SAVE} to ${APPS}"
	   COMMAND ${CMAKE_COMMAND} -E rename ${APPS_SAVE} ${APPS}
    )

endif()

#------------------------------------------------------------------------------------------------
#
#  deploy for Windows
#
#------------------------------------------------------------------------------------------------
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

    set(DEPLOY_TARGET_APP_EXE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    set(DEPLOY_TARGET_APP_EXE       ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}.exe)
    set(DEPLOY_MANIFEST_FILE        ${DEPLOY_TARGET_APP_EXE}.manifest)

    set(OPENCV_FFMPEG_PLUGIN_NAME  opencv_videoio_ffmpeg450_64.dll)
    set(OPENCV_FFMPEG_PLUGIN  ${CMAKE_BINARY_DIR}/bin/${OPENCV_FFMPEG_PLUGIN_NAME})

    message(STATUS "DEPLOY_MANIFEST_FILE : ${DEPLOY_MANIFEST_FILE}")
    message(STATUS "DEPLOY_TARGET_APP_EXE : ${DEPLOY_TARGET_APP_EXE}")
    message(STATUS "DEPLOY_TARGET_APP_EXE_PATH : ${DEPLOY_TARGET_APP_EXE_PATH}")
    message(STATUS "QT_WIN_DEPLOYQT_EXE  : ${QT_WIN_DEPLOYQT_EXE}")
    message(STATUS "QT_WIN_INSTALLER_FRAMEWORK_EXE : ${QT_WIN_INSTALLER_FRAMEWORK_EXE}")

    add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMENT "Copy OpenCV FFMPEG Plugin"
        COMMAND ${CMAKE_COMMAND} -E copy ${OPENCV_FFMPEG_PLUGIN} ${DEPLOY_TARGET_APP_EXE_PATH}
    )


if (CMAKE_BUILD_TYPE STREQUAL "Release")
   # add_custom_command(
   #     TARGET ${TARGET_NAME}
   #     POST_BUILD
   #     COMMENT "Adding manifest..."
   #     COMMAND "mt.exe" -manifest \"${DEPLOY_MANIFEST_FILE}\" -outputresource:"${DEPLOY_TARGET_APP_EXE}"\;\#1
   # )


    set(INSTALL_DIR  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/install)
    set(INSTALL_PACKAGE_DATA_APP     ${INSTALL_DIR}/packages/io.facedetect.app/data)
    set(INSTALL_PACKAGE_DATA_REDIST  ${INSTALL_DIR}/packages/io.facedetect.redist/data)
    add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMENT "Copy dependents of ${DEPLOY_TARGET_APP_EXE}"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${INSTALL_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${QT_WIN_INSTALLER_TEMPLATE} ${INSTALL_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${DEPLOY_TARGET_APP_EXE}  ${INSTALL_PACKAGE_DATA_APP}
        COMMAND ${CMAKE_COMMAND} -E copy ${OPENCV_FFMPEG_PLUGIN}   ${INSTALL_PACKAGE_DATA_APP}
        COMMAND ${QT_WIN_DEPLOYQT_EXE} --release --verbose 2 --no-system-d3d-compiler --no-opengl-sw --no-translations --qmldir ${CMAKE_CURRENT_SOURCE_DIR}/qml ${INSTALL_PACKAGE_DATA_APP}/${TARGET_NAME}.exe --dir ${INSTALL_PACKAGE_DATA_APP}
        COMMAND ${CMAKE_COMMAND} -E copy ${INSTALL_PACKAGE_DATA_APP}/vc_redist.x64.exe  ${INSTALL_PACKAGE_DATA_REDIST}
        COMMAND ${CMAKE_COMMAND} -E remove ${INSTALL_PACKAGE_DATA_APP}/vc_redist.x64.exe

    )

    #set(QT_IFW_CONFIG   "C:/Users/aablin/work/Projects/Installer/config/config.xml")
    #set(QT_IFW_PACKAGES "C:/Users/aablin/work/Projects/Installer/packages")
    set(QT_IFW_CONFIG   "${INSTALL_DIR}/config/config.xml")
    set(QT_IFW_PACKAGES "${INSTALL_DIR}/packages")
    set(QT_IFW_OUTPUT   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}-Setup")

    add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMENT "Create Winsows installer with QtInstallerFramework (QT IFW: ${QT_WIN_INSTALLER_FRAMEWORK_EXE})"
                "IFW config:   ${QT_IFW_CONFIG}"
                "IFW packages: ${QT_IFW_PACKAGES}"
                "IFW exe: ${QT_WIN_INSTALLER_FRAMEWORK_EXE} --offline-only -c ${QT_IFW_CONFIG} -p ${QT_IFW_PACKAGES} ${QT_IFW_OUTPUT}"
        COMMAND ${QT_WIN_INSTALLER_FRAMEWORK_EXE} --offline-only -c ${QT_IFW_CONFIG} -p ${QT_IFW_PACKAGES} ${QT_IFW_OUTPUT}
    )

    
endif()


    function(CPACKIFW_COMMON)
        set(CPACK_PACKAGE_NAME qt_cpackifw)
        set(CPACK_PACKAGE_FILE_NAME installer)
        set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Installation Tool")
        set(CPACK_PACKAGE_VERSION "1.0.0") # Version of installer
        set(CPACK_COMPONENTS_ALL qt_cpackifw_installer)
        set(CPACK_IFW_PACKAGE_START_MENU_DIRECTORY Qt_CPackIFW)
        set(CPACK_GENERATOR IFW)
        set(CPACK_IFW_VERBOSE ON)

        include(CPack REQUIRED)
        include(CPackIFW REQUIRED)

        cpack_add_component(
            qt_cpackifw_installer
            DISPLAY_NAME "Qt CPackIFW"
            DESCRIPTION "Install me"
            REQUIRED
        )

        cpack_ifw_configure_component(
            qt_cpackifw_installer
            FORCED_INSTALLATION
            NAME qt.cpackifw.installer
            VERSION ${PROJECT_VERSION} # Version of component
            LICENSES License ${qt_cpackifw_SOURCE_DIR}/LICENSE
            DEFAULT TRUE
        )
    endfunction()


            install(
                    DIRECTORY ${CMAKE_BINARY_DIR}/windeployqt_stuff/
                    DESTINATION ${PROJECT_NAME}
                    COMPONENT qt_cpackifw_installer
                )

                CPACKIFW_COMMON()

endif()




















