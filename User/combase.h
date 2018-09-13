#ifndef COMBASE_H
#define COMBASE_H


class Combase
{
public:
    Combase();

    void setIsHexSend(bool isHexSend)
    {
        m_isHexSend = isHexSend;
    }
    void setIsHexDisplay(bool isHexDisplay)
    {
        m_isHexDisplay = isHexDisplay;
    }

public:

    bool m_isHexSend;
    bool m_isHexDisplay;
};

#endif // COMBASE_H
