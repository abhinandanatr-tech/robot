#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "robotMain.h"

void app_main(void)
{
    while (true)
    {
        RobotMain();
        
        printf("ESP32 Starting...\n");
        sleep(1);
    }    
    
}
