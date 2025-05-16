#ifndef _PLOT_ANALOG_SENSOR_
#define _PLOT_ANALOG_SENSOR_

class PART_ANALOG_SENSOR_ENTRY
    {
    public:
    int    number;  /* Sensor number */
    int    units;   /* Units index */
    double adder;
    double subtrahend;
    double multiplier;
    double divisor;

    PART_ANALOG_SENSOR_ENTRY() { number=NO_ANALOG_SENSOR_NUMBER; units=VOLTS_UNITS; adder=0.0; subtrahend=0.0; multiplier=5.0; divisor=4095.0; }
    ~PART_ANALOG_SENSOR_ENTRY();
    PART_ANALOG_SENSOR_ENTRY & operator=( const PART_ANALOG_SENSOR_ENTRY & sorc );
    BOOLEAN read( DB_TABLE & sorc );
    double converted_value( unsigned raw_analog_counts );
    double raw_count( double x );
    int units_index() { return units; }
    };

class PART_ANALOG_SENSOR
    {
    private:
    PART_ANALOG_SENSOR_ENTRY * as;
    int n;

    public:

    PART_ANALOG_SENSOR() { as=0; n=0; }
    ~PART_ANALOG_SENSOR();
    int count() { return n; }
    BOOLEAN get();
    int     index( int number_to_find );
    PART_ANALOG_SENSOR_ENTRY & operator[]( int i );
    };

#endif

