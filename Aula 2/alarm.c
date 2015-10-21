#include "alarm.h"

static int flag = -1;
static int tries = -1;

void atende()  {
	flag=1;
	tries++;
	 // instala  rotina que atende interrupcao
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