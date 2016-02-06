import QtQuick 2.5
import QtMultimedia 5.5
Video {
    id: video

    source: "file:///C:\\Users\\at\\Documents\\qt\\build-helloQML-Desktop_Qt_5_5_1_MSVC2013_64bit-Debug\\R2BUiErBRnY.mp4"
    anchors.fill: parent

    onPlaybackStateChanged: console.log(video.playbackState)


    onStatusChanged:  console.log(video.status);


    MouseArea {
        anchors.fill: parent
        onClicked: {
            video.play()
        }
    }
    NumberAnimation {
          id: animateOpacity
          target: flashMessageBox
          properties: "opacity"
          from: 0.0
          to: 1.0
          duration: 1000
          easing {type: Easing.Linear}
     }
    Rectangle{
        id:flashMessageBox
        x: 130
        y: 550
        width: 560
        height: 44
        color: "#808080"
        radius: 15
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 17
        opacity: 0.7
        Text {
            id:flashMessageText
            color: "#ffffff"
            text: "Hello World"
            font.pointSize: 14
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            anchors.fill: parent



        }
    }

    Text {
        id: infoText
        x: 110
        width: 588
        height: 32
        opacity: 0.7
        color: "#ffffff"
        text: qsTr("hi")
        font.bold: true
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        font.pointSize: 13
    }
    function setInfoMessage(msg){
        infoText.text=msg
    }

    focus: true
    Keys.onSpacePressed: video.playbackState == MediaPlayer.PlayingState ? video.pause() : video.play()
    Keys.onLeftPressed: video.seek(video.position - 5000)
    Keys.onRightPressed: video.seek(video.position + 5000)
    Keys.onUpPressed: {
        //video.source = "file:///C:\\Users\\at\\Documents\\qt\\build-helloQML-Desktop_Qt_5_5_1_MSVC2013_64bit-Debug\\_sQSXwdtxlY.mp4"
        showFlashMessage("hihi",5000);
    }

    Timer {
        id: flashTimer


   }
    function showFlashMessage(msg,duration){
        console.log("welcome")
        animateOpacity.from=0;
        animateOpacity.to=0.7;
        flashMessageText.text=msg;
        flashTimer.interval = duration;
        flashTimer.triggered.connect(function(){
            console.log('hi')
            animateOpacity.from = 0.7;
            animateOpacity.to=0;
            animateOpacity.start();
        })
        animateOpacity.start()
        flashTimer.start()

    }

}
