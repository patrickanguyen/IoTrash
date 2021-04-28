#include <ultrasonic_serial.h>

int app_main()
{
    ultrasonic_serial_t dist_sensor = {.uart_num = UART_NUM_1};
    
    ultrasonic_serial_init(&dist_sensor);

    while (1)
    {
        uint32_t distance;
        ultrasonic_measure(&dist_sensor, &distance);

    }
}