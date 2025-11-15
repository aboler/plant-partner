#ifndef PLANTDATA_H
#define PLANTDATA_H

#include <stdint.h>

// Temporary structure for plant data for what could be recorded from sensors
// May add more/adjust data types as we investigate sensors more

struct plantData
{
    uint16_t lightData;
    uint16_t waterData;
    uint16_t nutrientData;
};

struct plantDataUpdate
{   char* plantName;
    float soilMoisture;
    float lightIntensity;
    float nLevel;
    float pLevel;
    float kLevel;
};

#endif