#include "random.h"

/**
 * Seeds the default RNG with the generated seed.
 *
 * Seed value is calculated by using three separate entropy sources (floating analog pin, PIN diode and temp sensor).
 * LSB bits of each of those sources are XOR-ed together to produce random value.
 * 
 *
 * @return Generated seed values.
 */
int32_t seed_random()
{
    // little bit of hackery
    int32_t seed = 0;
    int16_t sample;
    uint8_t i;

    // le noise.
    // from floating pin, temp sensor and PIN diode,
    GPIO_Analog_Input(&GPIO_PORTK, _GPIO_PINMASK_ALL);
    GPIO_Analog_Input(&GPIO_PORTP, _GPIO_PINMASK_ALL);

    ADC_Set_Input_Channel(_ADC_CHANNEL_0);
    ADC_Set_Input_Channel(_ADC_CHANNEL_18);
    ADC_Set_Input_Channel(_ADC_CHANNEL_22);

    ADC1_Init_Advanced(_ADC_EXTERNAL_REF);
    for (i = 0; i < 32; ++i)
    {
        sample = ADC1_Get_Sample(0) ^ ADC1_Get_Sample(18) ^ ADC1_Get_Sample(22);
        seed |= (sample & 1) << i;
    }

    return seed;
}