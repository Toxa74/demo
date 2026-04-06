import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import "FaceResult"


Rectangle{
    id: root
    color: "#F8F9FD"
    width: 1280
    height: 640

    signal webcameraSelected(var value)
    signal phoneSelected(var value)
    signal startClicked()
    signal setStop()
    signal modeChanged(var value)


    property bool is_worked: imageReloadTimer.running

    function selectPhone() {
        setWorking(false)
        sourceControl.selectPhone()
        visible = true
    }

    function selectWebcam() {
        setWorking(false)
        sourceControl.selectWebcam()
        visible = true
    }

    function setWorking(state) {
        if(state){
            sourceControl.setWorking(true)
            console.log("FeatureScreen setStateStop()")
            imageReloadTimer.running = true
            imageView.source = "image://PhotosProvider/image.png"
        }
        else {
            sourceControl.setWorking(false)
            imageReloadTimer.running = false
            imageView.source = "images/watermark.svg"
            imageView.reload()
            faceProcessResult.visible=false
        }
    }

    function setFaceEstimations(a1, a1_t, a2, a2_t, c, c_t) {
        face_result.setSpoofingV1(a1, a1_t)
        face_result.setSpoofingV2(a2, a2_t)
        face_result.setCosmetic(c, c_t)
    }

    function setResult(is_real){
        //faceProcessResult.setResult(is_real)
        //faceProcessResultTimer.stop()
        //faceProcessResultTimer.interval=duration_ms;
        //faceProcessResultTimer.start()
        //console.info("++++ setResult: timer interval: " + faceProcessResultTimer.interval)

        face_final_result.visible=true
        face_final_result.setAngry(10)
        face_final_result.setSurprised(6)
        face_final_result.setAntispoofingV1(0.5)
        face_final_result.setFaceQuality(50)
        if(is_real)
            face_final_result.setResultReal()
        else
            face_final_result.setResultFake()
    }

    function setFaceProgres(value){
        faceProcessResult.setProgress(value)
    }

    function reset(){
        face_result.reset()
        faceProcessResult.visible=false
        face_final_result.visible=false
        setWorking(true)
    }

    Component.onCompleted: {
        sourceControl.setWorking(false)
        console.log("FeatureScreen setStart()")
    }

    TabBar {
        id: tabBar
        anchors.left: parent.left
        anchors.right: control_frame.left
        anchors.top: parent.top
        anchors.rightMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: 0

        background: Rectangle {
            color: "transparent"
        }

        CustomTabButton{
            text:  "FACE"
        }
        CustomTabButton {
            text: "FRONT ID"
        }
        CustomTabButton {
            text: "BACK ID"
        }

        onCurrentIndexChanged: {
            root.modeChanged(tabBar.currentIndex)
            setTextHint("") //Clear hint
        }
    }

    Frame {
        id: control_frame
        width: 350
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        background: Rectangle{
            border.color:"#CACACA"
            border.width:1
        }

        SourceControl {
            id: sourceControl
            height: 250
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top

            onStartClicked: {
                root.startClicked()
            }


            onWebcameraSelected: {
                console.log("+++++onWebcameraSelected : " + value)
                root.webcameraSelected(value)
            }

            onPhoneSelected: {
                root.phoneSelected(value)
            }
        }

        Rectangle {
            id: separator_1
            height: 1
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: sourceControl.bottom
            anchors.rightMargin: 0
            anchors.leftMargin: 0
            anchors.topMargin: 0
            color:"#CACACA"
        }

        StackLayout {
            anchors.top: sourceControl.bottom
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.topMargin: 0
            currentIndex: tabBar.currentIndex

            FaceResults {
                id: face_result
            }

            Item {
                id: discoverTab
            }
            Item {
                id: activityTab
            }
        }

    }

    function imageReload(){
           // mainWindow.show()
           // mainWindow.raise()
           // mainWindow.requestActivate()
            //console.log("imageReload: " + imageView.source)

            imageView.show()
            imageView.reload()
    }

    function setTextHint(hint_text){
        hintText.hint=hint_text
        //console.log("++++ set hint " + hint_text)
    }

    Timer {
        id: imageReloadTimer
        interval: cppHandler.getDrawFps()
        running: false;
        repeat: true
        onTriggered: {
            imageReload()
        }
    }

    PhotoImg {
        id: imageView
        anchors.left:   parent.left
        anchors.right:  control_frame.left
        anchors.top:    tabBar.bottom
        anchors.bottom: parent.bottom


        Text {
            property string hint: ""
            id: hintText
            y: 511
            text: (root.is_worked && tabBar.currentIndex==0)?hint:""
            anchors.bottom: parent.bottom
            font.pixelSize: 44
            font.family: "Aileron"
            font.bold: true
            color: "white"
            anchors.bottomMargin: 50
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    ProcessResult {
        id: faceProcessResult
        anchors.left:  parent.left
        anchors.right: control_frame.left
        anchors.top:   tabBar.bottom
        anchors.topMargin: 10
        height: 40
    }


    FaceFinalResultFrame{
        id: face_final_result
        anchors.horizontalCenter: imageView.horizontalCenter
        anchors.verticalCenter: imageView.verticalCenter
        visible: false
    }

    Timer {
        id: faceProcessResultTimer
        interval: 3000
        running: false;
        repeat: false;
        onTriggered: {
            faceProcessResult.visible=false
            console.info("++++ finish Show result")
        }
    }

    InstructionFigma {
        //property var is_show_instruction : sourceControl.is_phone_source && !root.is_worked

        visible: sourceControl.is_phone_source && !root.is_worked
        anchors.left: parent.left
        anchors.right: control_frame.left
        anchors.top: tabBar.bottom
        anchors.bottom: parent.bottom
    }
}







/*##^##
Designer {
    D{i:0;formeditorZoom:0.66}
}
##^##*/
