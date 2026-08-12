#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "robotMain.h"

void app_main(void)
{
    while (true) {
		
        RobotMain();
        printf("Hello from abi_main!\n");
        sleep(1);
    }
}
