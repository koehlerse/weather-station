#ifndef SENSORS_H
#define SENSORS_H

typedef struct 
{
    float temperature;
    float humidity;
    float pressure;
    float air_quality;
    float lux;
} SensorData;

void sensors_init(void);

SensorData sensors_read(void);

#endif