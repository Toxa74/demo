import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3
import "FaceResult"

Window {
    id: window
    width: 1280
    height: 720
    visible: true
    title: qsTr("Demo")

    flags: Qt.FramelessWindowHint | Qt.CustomizeWindowHint |Qt.Window | Qt. WindowTitleHint

    TitleBar {
        z:1
        id:title_bar
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 0

        onBackClicked:{
            title_bar.back_arrow_visible=false
            first_scr.visible   = true

            if(feature_scr.visible) {
                feature_scr.visible = false
                feature_scr.setWorking(false)
            }
            cppHandler.onStop()
        }
        onMinimizeClicked: {
            window.hide()
        }
        onCloseClicked: {
            window.close()
        }

        onMouseXChanged:{
            window.setX(window.x + x)
        }
        onMouseYChanged:{
            window.setY(window.y + y)
        }
    }

    SelectSourceSrc {
        id: first_scr
        visible: true
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: title_bar.bottom
        anchors.bottom: buttom_frame.top
        onWebcameraSelected:{
            feature_scr.selectWebcam()
            visible = false
            title_bar.back_arrow_visible=true
        }
        onPhoneSelected:{
            feature_scr.selectPhone()
            visible = false
            title_bar.back_arrow_visible=true
        }
    }

    Component.onCompleted: {
        console.log("+++++++++++++:  cppHandler.getCurrentDateTime() : " + cppHandler.getCurrentDateTime())
    }

    MessageDialog {
        function setMessage(value) {
            text = value
            visible = true
        }

        id:critical_message
        visible: false

        onAccepted: {
            visible= false
        }
    }

    Connections {
        target: cppHandler
        function onSignalCantOpenSource() {
            critical_message.setMessage("Can't open source")
        }

        function onSignalImgThreadState(state) {
            feature_scr.setWorking(state)
        }

        function onSignalFaceEstimations(a1, a1_t, a2, a2_t, c, c_t) {
            feature_scr.setFaceEstimations(a1, a1_t, a2, a2_t, c, c_t)
        }

        function onSignalFaceResult(is_real){
            console.info("++++ onSignalFaceResultReal  : " + is_real)
            feature_scr.setResult(is_real)
        }
        function onSignalFaceStartCapture(){
            feature_scr.reset();
        }
        function onSignalSetFaceProgress(value){
            feature_scr.setFaceProgres(value)
        }

        function onSignalSetTextHint(hint_text) {
            feature_scr.setTextHint(hint_text)
        }
    }

    FeatureScreen {
        id: feature_scr
        visible: false
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: title_bar.bottom
        anchors.bottom: buttom_frame.top
        onWebcameraSelected:{
            console.info("main: onWebcameraSelected: " + value)
            cppHandler.onWebcameraSelected(value)
        }
        onPhoneSelected:{
            console.info("main: onPhoneSelected: " + value)
            cppHandler.onPhoneSelected(value)
        }

        onStartClicked:{
            console.info("main: onStartClicked")
            cppHandler.onStartClicked()
        }

        onModeChanged:{
            console.info("main: onModeChanged: " + value)
            cppHandler.onModeChanged(value)
        }
    }

    BottomFrame {
        id: buttom_frame
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
    }

/*
    FaceFinalResultFrame{
        id: face_final_result
        x:100
        y:100
    }

    Timer {
        id: textTimer
        interval: 10000
        repeat: false
        running: true
        triggeredOnStart: true
        onTriggered: {
            face_final_result.setAngry(10)
            face_final_result.setSurprised(6)
            face_final_result.setAntispoofingV1(0.5)
            face_final_result.setFaceQuality(50)
            face_final_result.setResultFake()
        }
    }
    */
/*

    Webcamera_button_small {
        id: web_btn_small
        x:700
        y:650
    }

    Phone_button_small {
        id: phone_btn_small
        x:700+web_btn_small.width+10
        y:650
    }

    SelectSourceSrc_small {
        id: first_scr_small
       // anchors.fill: parent

        x:900
        y:600

        width: 320
        height: 50
        onWebcameraSelected:{
            console.info("onWebcameraSelected")
        }
        onPhoneSelected:{
            console.info("onPhoneSelected")
        }
    }

    SourceControl {
        x:600
        y:400
    }

    FaceSpoofing {
        id:spoofing
        x:600
        y:550

        text: "FACE SPOOFING V1"
        Component.onCompleted: {
            setResult(99)
        }
    }

    FaceResults {
        id:face_result
        x:600
        y:350

        Component.onCompleted: {
            face_result.setSpoofingV1(9)
            face_result.setSpoofingV2(99)
            face_result.setCosmetic(10)
        }
    }

    Timer {
        property real counter :0
           id: textTimer
           interval: 100
           repeat: true
           running: true
           triggeredOnStart: true
           onTriggered: {
               counter+=0.01;
               if(counter > 1)
                   counter=0

               spoofing.setResult(counter)
           }
       }
       */
}
