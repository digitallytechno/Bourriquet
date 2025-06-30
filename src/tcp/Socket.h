/***************************************************************************
 Bourriquet MorphOS
 Copyright (C) 2024 digitally
 
***************************************************************************/
#ifndef SOCKET_H
#define SOCKET_H

#define CT_NAME        0x01
#define CT_VERSION  0x11
#define CT_PORT        0x0f

//ULONG eventmask;

struct timerequest *create_timer(ULONG unit);
void startTimer(struct timerequest *tr);
unsigned long timerclocks(void);
void timerwaits(int nb_sec);
unsigned long ClockUs(void);
void delete_timer(struct timerequest *tr);

unsigned char *NewID(void);
int get_response(long, char *, int);
struct sockaddr_in get_socket_address(char *ip, int port);
int get_connected(long sockfd, struct sockaddr *sa);
void send_msg(long, unsigned char *, int);
void sendPacket(long sockfd, unsigned char *buf, int buflen);
int handshake(long sockfd, int type);
void getErrno(int myErrno);
int establish_connection(struct Host *host);
int testRecv(long);
int testSend(long);
int acceptHandler(long sockfd);
int waitSend(long);
void waitRecv(long);
void readHandler(struct Host *host);
void writeHandler(struct Host *host);
int closeHandler(struct Host *host);
int errorHandler(struct Host *host);
int connectHandler(struct Host *host);
BOOL AsynchSocketEvents(struct Host *host);

#endif /* SOCKET_H */
