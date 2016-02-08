import QtQuick 2.5
import QtMultimedia 5.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
Rectangle{
    id:container
    anchors.fill: parent
    color: "#000000"
    VideoOutput{
        source: video
        anchors.fill: parent

    }
    signal stateChanged(int state)
    signal mediaStatusChanged (int status)
    signal videoDurationChanged(int duration)
    signal videoPositionChanged(int pos)
    signal videoAvailabilityChanged(bool a)
    signal videoVolumeChanged(int a)
    signal playPasueButtonClicked()
    signal timeSliderReleased(int value)
    MediaPlayer {
    id: video
    //autoPlay: true
    //source: "file:///C:\\Users\\at\\Documents\\qt\\build-singk-Desktop_Qt_5_5_1_MSVC2013_64bit-Release\\videos\\wt0RKW3aC84.mp4"
    objectName: "player"


    onPlaybackStateChanged: {
        console.log("state "+video.playbackState)
        if (video.playbackState != MediaPlayer.PlayingState){
            playPauseImage.source="/img/1454880632_play-circle-outline.png"
        }else{
            playPauseImage.source="/img/1454880691_pause-circle-outline.png"
        }

        container.stateChanged(video.playbackState);

    }
    onStatusChanged:  {
        console.log("status " +video.status);
        container.mediaStatusChanged(video.status)
    }

    onDurationChanged: {
        timeSlider.maximumValue=video.duration;
        container.videoDurationChanged(video.duration);
    }
    function getTimeString(pos){
        var min = Math.floor(pos/60000);
        var sec = Math.floor(pos/1000)%60;
        var secText= sec>9?""+sec:"0"+sec;
        return min+":"+secText
    }

    onPositionChanged: {

        timer.text = getTimeString(video.position)+"/"+getTimeString(video.duration)
       if (!timeSlider.pressed)
            timeSlider.value = video.position
        container.videoPositionChanged(video.position);
    }
    onAvailabilityChanged: {

        //video.videoAvailabilityChanged(video.availability);
    }
    onHasVideoChanged: {

        container.videoAvailabilityChanged(video.hasVideo);
    }

    onVolumeChanged: {
       var t = video.volume*100;
        container.videoVolumeChanged(t)

    }
    onSourceChanged: {
        console.log("source:"+video.source);
        soundSlider.value = video.volume*100
    }
    onMutedChanged: {
        if (video.muted){
            soundImage.source="/img/1454880797_device-volume-loudspeaker-speaker-mute-glyph.png"
        }else{
            soundImage.source="/img/1454880805_device-volume-loudspeaker-speaker-up-glyph.png"
        }
    }
    onErrorStringChanged: {
        console.log(video.errorString)
    }



}
    MouseArea {
        z:1
        id: videoArea
        anchors.fill: parent
        hoverEnabled: true

        onEntered:  {
            videoPanel.state=''
        }
        onExited: {
             videoPanel.state ='hide'
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
            anchors.bottom: videoPanel.top
            anchors.bottomMargin: 15
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
        Rectangle {
            z:1
            id: videoPanel
            y: 271
            height: 80
            color: "#ffffff"
            radius: 15
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.rightMargin: 15
            anchors.leftMargin: 15
            anchors.right: parent.right
            anchors.left: parent.left
            opacity: 0.7

            Grid {
                id: grid1
                spacing: 0
                columns: 1
                anchors.fill: parent
                rows: 2

                Row {
                    id: row3
                    width: parent.width
                    height: parent.height/2

                    Slider {
                        id: timeSlider
                        height: parent.height/2
                        anchors.left: parent.left
                        anchors.leftMargin: 15
                        anchors.right: parent.right
                        anchors.rightMargin: 15
                        anchors.verticalCenter: parent.verticalCenter

                        onPressedChanged: {
                            if (!timeSlider.pressed)
                                container.timeSliderReleased(timeSlider.value)

                        }

                    }

                }

                Row {
                    id: row1
                    width: parent.width
                    height: parent.height/2
                    scale: 1
                    spacing: 20

                    ToolButton {
                        id: playPauseBtn
                        anchors.verticalCenter: parent.verticalCenter
                        onClicked: {
                            container.playPasueButtonClicked()
                        }

                        Image {
                            id: playPauseImage
                            fillMode: Image.PreserveAspectFit
                            anchors.fill: parent
                            source: "/img/1454880632_play-circle-outline.png"

                        }



                    }

                    Row {
                        id: row2
                        width: 200
                        height: 400
                        anchors.verticalCenter: parent.verticalCenter

                        ToolButton {
                            id: soundBtn
                            anchors.verticalCenter: parent.verticalCenter
                            onClicked: video.muted = !video.muted


                            Image {
                                id: soundImage
                                fillMode: Image.PreserveAspectFit
                                anchors.fill: parent
                                source: "/img/1454880805_device-volume-loudspeaker-speaker-up-glyph.png"

                            }
                        }

                        Slider {
                            id: soundSlider
                            width: 150
                            anchors.verticalCenter: parent.verticalCenter
                            maximumValue: 100
                            onValueChanged: {
                                video.volume = soundSlider.value/100
                            }
                        }

                    }

                    Label {
                        id: timer
                        text: qsTr("0:00/0:00")
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }
            states:[
                State{
                    name :"hide"
                    PropertyChanges {
                        target: videoPanel
                        anchors.bottomMargin: -videoPanel.height
                    }
                }



            ]
            transitions: [
                // This adds a transition that defaults to applying to all state changes

                Transition {

                    // This applies a default NumberAnimation to any changes a state change makes to x or y properties
                    NumberAnimation { properties: "anchors.bottomMargin"
                        id:panelAnimation
                        duration: 200
                        easing.type:  Easing.Linear


                    }
                }
            ]
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

    Timer {
        id: flashTimer


   }
    Keys.onUpPressed: {
        player.play()
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
