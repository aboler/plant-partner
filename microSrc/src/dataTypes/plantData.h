#ifndef PLANTDATA_H
#define PLANTDATA_H

#include <stdint.h>

// Temporary structure for plant data for what could be recorded from sensors
// May add more/adjust data types as we investigate sensors more

struct plantData
{
    uint16_t lightData;
    uint8_t waterData; // Rough % of wetness
    uint16_t nitrogenData;
    uint16_t phosphorusData;
    uint16_t potassiumData;
};

#endif