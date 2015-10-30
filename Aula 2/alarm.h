#ifndef ___ALARM
#define ___ALARM

/**
 * The main handler of all the existent alarms.
 */
void atende();

/**
 * Returns the flag used for the alarm.
 * @return flag
 */
int getFlag();

/**
 * Returns the number of tries made by the current process.
 * @return tries
 */
int getTries();

/**
 * Sets the flag used for the alarm (ON or OFF).
 * @param f the new flag
 */
void setFlag(int f);

/**
 * Sets the initial number of tries in which the process starts.
 * @param t tries
 */
void setTries(int t);


#endif /* __ALARM */


