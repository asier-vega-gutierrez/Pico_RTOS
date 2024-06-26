//Free RTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
//Pico
#include "pico/stdlib.h"
//C std
#include <stdio.h>
//Own
#include "const/const.h"
#include "globals/globals.h"
#include "task/task_get_orientation.c"
#include "task/task_serial.c"
#include "task/task_control_servo.c"
#include "task/task_PID.c"
#include "task/task_emergency.c"

//Tarea para leer el sensor mpu6050
void task_get_orientation(void *parameters);
//Tarea para usar el serial
void task_serial(void *parameters);
//Tarea para usar el servo
void control_servo(void *parameters);
//Tarea para calcular el movimiento con el PID
void pid(void *parameters);
//Tarea para ejecutar el ciclo de emrgencia
void task_emergency(void *pvParameters);


int main(){

    //Iniciar stdio para usar la terminal
    stdio_init_all();

    //Iniciacion de colas
    queue_serial = xQueueCreate(QUEUE_SERIAL_SIZE, sizeof(QUEUE_SERIAL_DATA));
    queue_sensor = xQueueCreate(QUEUE_SENSOR_SIZE, sizeof(QUEUE_SENSOR_DATA));
    queue_servo = xQueueCreate(QUEUE_SERVO_SIZE, sizeof(QUEUE_SERVO_DATA));

    //Iniciacion de mutex y semaforos
    mutex_queue_sensor_recieve = xSemaphoreCreateMutex();

    //Iniciacion del boton de emergencia por ISR
    gpio_set_irq_enabled_with_callback(PIN_EM, GPIO_IRQ_EDGE_RISE , true, &emergency_callback);

    //Iniciacion de la tarea del servo
    xTaskCreate(control_servo, "control_servo", 256, NULL, 4, &xTaskHandleControlServo);
    //Iniciacion de la tarea del mpu6050
    xTaskCreate(task_get_orientation, "task_get_orientation", 256, NULL, 3, &xTaskHandleGetOrientation);
    //Iniciacion de la tarea del PID
    xTaskCreate(pid, "PID", 256, NULL, 2, &xTaskHandlePID);
    //Iniciacion de la tarea para usar el serial
    xTaskCreate(task_serial, "task_serial", 256, NULL, 1, &xTaskHandleSerial);
    //Iniciacion de la tarea a jecutar dspues de interrupcion de emergencia
    xTaskCreate(task_emergency, "task_emergency", 256, NULL, 31, &xTaskHandleEmergency);

    vTaskStartScheduler();
    while(1){};
}