#ifndef ___STATE
#define ___STATE
#include "utils.h"

/**
 * Sends a frame SET.
 * @param fd the connection
 * @param SET the frame
 * @return 0 if success, -1 if unsuccess
 */
int send_SET(int fd, char *SET);

/**
 * Sends a frame UA.
 * @param fd the connection
 * @param UA the frame
 * @return 0 if success, -1 if unsuccess
 */
int send_UA(int fd, char *UA);

/**
 * Sends a frame DISC.
 * @param fd the connection
 * @param DISC the frame
 * @return 0 if success, -1 if unsuccess
 */
int send_DISC(int fd, char *DISC);

/**
 * Sends a frame RR.
 * @param fd the connection
 * @param r frame identifier
 * @return 0 if success, -1 if unsuccess
 */
int send_RR(int fd, int r);

/**
 * Receives a frame SET.
 * @param fd the connection
 * @param SET the frame
 */
void receive_SET(int fd, char *SET);

/**
 * Receives a frame UA.
 * @param fd the connection
 * @param UA the frame
 */
void receive_UA(int fd, char *UA);

/**
 * Receives a frame DISC.
 * @param fd the connection
 * @param DISC the frame
 */
void receive_DISC(int fd, char *DISC);

/**
 * Receives a frame RR.
 * @param fd the connection
 * @param RR the frame
 * @param s frame identifier
 * @return 0 if it was a RR frame, -1 if it wasn't
 */
int receive_RR(int fd, char *RR, int s);

/**
 * A more general function to receive a frame.
 * @param fd the connection
 * @param FRAME the frame
 * @param maxFrameSize the maximum size of the frame
 * @return 0 if it was a valid frame, -1 if it wasn't
 */
int receive_FRAME(int fd, char *FRAME, int maxFrameSize);

/**
 * Checks if the frame is a frame UA.
 * @param sent the frame
 * @return 0 if it was a frame UA, -1 if it wasn't
 */
int check_UA(char *sent);

/**
 * Checks if the frame is a frame DISC.
 * @param DISC_rec the frame
 * @return 0 if it was a frame DISC, -1 if it wasn't
 */
int check_DISC(char *DISC_rec);

/**
 * Checks if the dataPacket is a frame I.
 * @param dataPacket the packet
 * @param s the frame identifier
 * @param frame the frame
 * @param frameSize the size of the frame
 * @return 0 if it was a frame I, -1 if it wasn't
 */
int check_I(char * dataPacket, int s, char *frame, int frameSize);

/**
 * Returns STOP_UA.
 * @return STOP_UA
 */
int getStopUA();

/**
 * Sets the STOP_UA.
 * @param st the new STOP_UA
 */
void setStopUA(int st);

/**
 * Returns STOP_SET.
 * @return STOP_SET
 */
int getStopSET();

/**
 * Sets the STOP_SET.
 * @param st the new STOP_SET
 */
void setStopSET(int st);

/**
 * Returns STOP_DISC.
 * @return STOP_DISC
 */
int getStopDISC();

/**
 * Sets the STOP_DISC.
 * @param st the new STOP_DISC
 */
void setStopDISC(int st);

/**
 * Sets the STOP_RR.
 * @param st the new STOP_RR
 */
void setStopRR(int st);

/**
 * Returns STOP_RR.
 * @return STOP_RR
 */
int getStopRR();

/**
 * Sets the STOP_FRAME.
 * @param st the new STOP_FRAME
 */
void setStopFRAME(int st);

/**
 * Returns STOP_FRAME.
 * @return STOP_FRAME
 */
int getStopFRAME();


#endif /* ___STATE */
