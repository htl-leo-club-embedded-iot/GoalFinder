#pragma once
#include "time.h"

class SNTP 
{
    public:
        virtual ~SNTP();
        void Init(); 
        struct tm GetLocalTime();
};
