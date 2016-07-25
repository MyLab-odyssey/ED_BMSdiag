#ifndef __H_TIMEOUT_
#define __H_TIMEOUT_

#include "Arduino.h"

#define TIMEOUT_LENGTH_INFINITE ((unsigned long)-1)

class CTimeout
{
public:
    CTimeout(unsigned long timeoutMS);

    bool Expired(bool fAutoReset = false);

    void Reset();

    void Reset(unsigned long timeoutMS);

private:
    volatile static unsigned long s_currentTime;

    unsigned long m_startTime;
    unsigned long m_timeout;
};

#endif // #ifndef __H_TIMEOUT_
