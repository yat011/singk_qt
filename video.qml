import QtQuick 2.5
import QtMultimedia 5.5
Video {
    id: video

   // source: "file:///C:\\Users\\at\\Documents\\qt\\build-helloQML-Desktop_Qt_5_5_1_MSVC2013_64bit-Debug\\R2BUiErBRnY.mp4"
    anchors.fill: parent


    signal stateChanged(int state)
    signal mediaStatusChanged (int status)
    signal videoDurationChanged(int duration)
    signal videoPositionChanged(int pos)
    signal videoAvailabilityChanged(bool a)
    signal videoVolumeChanged(int a)

    onPlaybackStateChanged: {
        console.log("state "+video.playbackState)

        video.stateChanged(video.playbackState);

    }
    onStatusChanged:  {
        console.log("status " +video.status);
        video.mediaStatusChanged(video.status)
    }

    onDurationChanged: {
        video.videoDurationChanged(video.duration);
    }
    onPositionChanged: {

        video.videoPositionChanged(video.position);
    }
    onAvailabilityChanged: {

        //video.videoAvailabilityChanged(video.availability);
    }
    onHasVideoChanged: {

        video.videoAvailabilityChanged(video.hasVideo);
    }

    onVolumeChanged: {
       var t = video.volume*100;
        video.videoVolumeChanged(t)
    }
    onSourceChanged: {
        console.log("source:"+video.source);
    }

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
          duration: 500
          easing {type: Easing.Linear}
     }
    Rectangle{
        id:flashMessageBox
        x: 130
        y: 425
        width: 328
        height: 35
        color: "#808080"
        radius: 15
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 17
        opacity: 0
        Text {
            id:flashMessageText
            color: "#ffffff"
            text: "Hello World"
            font.pointSize: 12
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            anchors.fill: parent



        }
    }

    Text {
        id: infoText
        height: 32
        opacity: 0.7
        color: "#ffffff"
        text: ""
        anchors.right: parent.right
        anchors.rightMargin: 25
        anchors.left: parent.left
        anchors.leftMargin: 26
        font.bold: true
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        anchors.top: parent.top
        anchors.topMargin: 20
        font.pointSize: 11
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
        //showFlashMessage("hihi",5000);
    }

    Timer {
        id: flashTimer


   }
    function showFlashMessage(msg,duration){

        animateOpacity.from=0;
        animateOpacity.to=0.7;
        flashMessageText.text=msg;
        flashTimer.interval = duration;
        flashTimer.triggered.connect(function(){

            animateOpacity.from = 0.7;
            animateOpacity.to=0;
            animateOpacity.start();
        })
        animateOpacity.start()
        flashTimer.start()

    }

}
