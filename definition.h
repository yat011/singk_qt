#ifndef DEFINITION_H
#define DEFINITION_H
enum { HELLO=0, OK, SEEK, PLAY, PRE_PLAY,PAUSE, PLAYING,STOP, HEART_BEAT, ADD_VIDEO,BUFFER,NONE,REMOVE_CURRENT,CHANGE_TO,ECHO} ;
enum{IDLE, WAITING};
enum{DOWNLOADING=4,NO_MEDIA,EndOfMedia,UNKNOWN=-1};
typedef QPair<QString, QString> MyPair;
Q_DECLARE_METATYPE(MyPair);

/*
 * play: msg(PRE_PLAY, seq) -> clients -> ok(seq) -> server -> msg(PLAY,seq ++) - >clients
 * 			server-status->WAITING ----------------> IDLE
 * 	if other msg, determined by time/seq
 *
 *
 *
 *
 *
 *
 *
 */



#endif // DEFINITION_H
