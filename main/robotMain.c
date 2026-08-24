#include "robotMain.h"

#include "wifi.h"

void RobotMain()
{

    wifi_init();
    while (1)
    {
        printf("Hallo from robot main\r\n");


        sleep(1);
       
    }
}