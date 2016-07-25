#include "Timeout.h"

// Define static members
volatile unsigned long CTimeout::s_currentTime = 0;

CTimeout::CTimeout(unsigned long timeoutMS)
{
    m_timeout = timeoutMS;
    m_startTime = millis() - timeoutMS;
}

bool CTimeout::Expired(bool fAutoReset)
{
    if (m_timeout != TIMEOUT_LENGTH_INFINITE)
    {
        CTimeout::s_currentTime = millis();

        if ((CTimeout::s_currentTime - m_startTime) >= m_timeout)
        {
            if (fAutoReset)
            {
                Reset();
            }

            return true;
        }
    }

    return false;
}

void CTimeout::Reset()
{
    m_startTime = millis();
}

void CTimeout::Reset(unsigned long timeoutMS)
{
    m_timeout = timeoutMS;
    m_startTime = millis();
}
