#include <GoalfinderApp.h>
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems

void setup() 
{
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
    GoalfinderApp::GetInstance()->Init();
}

void loop() 
{
    GoalfinderApp::GetInstance()->Process();
}