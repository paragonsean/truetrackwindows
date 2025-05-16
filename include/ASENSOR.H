BOOLEAN   analog_sensor_exists(  short analog_sensor_number );
BITSETYPE analog_sensor_vartype( short analog_sensor_number );
short analog_sensor_units( short analog_sensor_number );
void analog_sensor_shutdown( void );
BOOLEAN analog_sensor_startup( void );
TCHAR * analog_sensor_description( short analog_sensor_number );
short analog_sensor_number( TCHAR * analog_sensor_desc );
float analog_sensor_high_value(short analog_sensor_number);
float analog_sensor_low_value(short analog_sensor_number);
