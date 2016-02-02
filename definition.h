#ifndef DEFINITION_H
#define DEFINITION_H
enum { HELLO=0, OK, PLAY, PRE_PLAY,PAUSE, PLAYING,STOP} ;
enum{IDLE, WAITING};
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
