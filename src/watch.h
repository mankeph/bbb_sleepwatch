#ifndef SW_WATCH_H
#define SW_WATCH_H

#include "dht/BBB/bbb_dht_read.h"
#include "sqlite_db.h"
#include <stdlib.h>

// light sensor
#define ADC_FILE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
// microphone
#define ADC_FILE1 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
// gas sensor
#define ADC_FILE2 "/sys/bus/iio/devices/iio:device0/in_voltage2_raw"

// gpio numbers for the one wire communication with the AM2302 sensor
#define DHT_GPIO_BASE 0
#define DHT_GPIO_NR 2

// ringbuffer to temporarily hold sensor data
typedef struct data_buffer {
  int bufsize;
  int *r_ptr;
  int *w_ptr;
  int *s_ptr;
} BUF;

BUF *bufarr;

// initialize buffer, enable adc
void init_watch(void);

// gather datapoint from sensor sensnr
void watch_sensor(char sensnr);

// grab a datapoint from the buffer
int grab_value(char sensnr);

// add datapoint to the buffer
void add_to_buf(char sensnr, int *val);

// read from the adc file 
int read_adc(char *adcfile);

// read the AM2302 sensor
int read_dht(float *hum, float *temp);

// read all datapoints in the buffer, store the average to the database
int flush_buffer_to_db(void);

// free the buffer
void close_watch(void);

#endif // SW_WATCH_H
