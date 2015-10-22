#include "alarm.h"
#include "signal.h"

static int flag = 1;
static int tries = 1;
static pthread_t* threadsTid;

void atende()  {
	flag=1;
	pthread_cancel(threadsTid[tries - 1]);
	tries++;
}

int getFlag() {
  return flag;
}

int getTries() {
  return tries;
}

void setFlag(int f) {
  flag = f;
}

void setTid(pthread_t * tid){
	threadsTid = tid;
}