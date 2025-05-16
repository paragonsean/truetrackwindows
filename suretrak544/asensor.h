#ifndef _ANALOG_SENSOR_CLASS_
#define _ANALOG_SENSOR_CLASS_

class ANALOG_SENSOR_CLASS
    {
    public:

    double low_volts;
    double low_pressure;
    double delta_volts;
    double delta_pressure;

    ANALOG_SENSOR_CLASS() { low_volts = 0.0; low_pressure = 0.0; delta_volts = 5.0;
                            delta_pressure = 10000.0; }
    ~ANALOG_SENSOR_CLASS() {}
    double true_value( BOARD_DATA x );
    BOARD_DATA board_value( double y );
    BOOLEAN read( DB_TABLE & t );
    };

#endif

