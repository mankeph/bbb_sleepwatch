#include "watch.h"

void init_watch(void) {
  // init adc
  FILE* test = fopen(ADC_FILE0, "r");
  if(test == NULL){
    system("echo BB-ADC > /sys/devices/platform/bone_capemgr/slots");
  } else {
    fclose(test);
  }
  // init buffers
  char bufsizes[5] = {50, 200, 2, 2, 2};
  bufarr = (BUF *)malloc(5 * sizeof(BUF));
  char cntr = 0;
  for (; cntr < 5; cntr++) {
    (bufarr + cntr)->s_ptr = (int *)malloc(bufsizes[cntr] * sizeof(int));
    (bufarr + cntr)->r_ptr = (bufarr + cntr)->s_ptr;
    (bufarr + cntr)->w_ptr = (bufarr + cntr)->s_ptr;
    (bufarr + cntr)->bufsize = bufsizes[cntr];
  }
}

void watch_sensor(char sensnr) {
  int val;
  switch (sensnr) {
  case 0: {
    // light level
    val = read_adc(ADC_FILE0);
    if (val < 1) {
      logm("WARNING light sensor could not be read!");
    } else {
      add_to_buf(LSENS, &val);
    }
  } break;
  case 1: {
    // noise level
    val = read_adc(ADC_FILE1);
    if (val < 1) {
      logm("WARNING microphone could not be read!");
    } else {
      add_to_buf(NSENS, &val);
    }
  } break;
  case 2:
  case 3: {
    // temperature & humidity
    float hum, temp;
    int dht_res = read_dht(&hum, &temp);
    if (dht_res == DHT_SUCCESS) {
      val = temp * 100;
      add_to_buf(TSENS, &val);
      val = hum * 100;
      add_to_buf(HSENS, &val);
    }
  } break;
  case 4: {
    // air quality
    val = read_adc(ADC_FILE2);
    if (val < 1) {
      logm("WARNING gas sensor could not be read!");
    } else {
      add_to_buf(GSENS, &val);
    }
  } break;
  default: {
    logn("ERROR watch_sensor called with wrong sensnr!", sensnr);
  } break;
  }
}

int read_adc(char *adcfile) {
  FILE *file = fopen(adcfile, "r");

  if (file == NULL) {
    logc("ERROR opening ADC file failed! : ", adcfile);
    return -1;
  }
  char *cval = (char *)calloc(5, sizeof(char));
  //char cntr = 0;
  int out;

  fread(cval, 1, 4, file);
  /*for (; cntr < 4; cntr++) {
    if (cval[cntr] < 30 || cval[cntr] > 39) {
      cval[cntr] = '\0';
      cntr = 4;
    }
  }*/
  out = atoi(cval);
  free(cval);
  fclose(file);
  return out;
}

int read_dht(float *hum, float *temp) {
  int dht_res = bbb_dht_read(AM2302, DHT_GPIO_BASE, DHT_GPIO_NR, hum, temp);
  if (dht_res != DHT_SUCCESS) {
    switch (dht_res) {
    case DHT_ERROR_TIMEOUT: {
      logn("WARNING DHT did not return in give timeframe: DHT_ERROR_TIMEOUT!",
           dht_res);
    } break;
    case DHT_ERROR_CHECKSUM: {
      logn("INFO DHT checksum failed to validate: DHT_ERROR_CHECKSUM!",
           dht_res);
    } break;
    case DHT_ERROR_ARGUMENT: {
      logn("ERROR DHT read function failed! Wrong input to read function: "
           "DHT_ERROR_ARGUMENT!",
           dht_res);
    } break;
    case DHT_ERROR_GPIO: {
      logn("ERROR DHT is not connected correctly or wrong pin numbers are "
           "used: DHT_ERROR_GPIO!",
           dht_res);
    } break;
    default: {
      logm("ERROR unknown return value value from DHT!");
      return -5;
    } break;
    }
    return dht_res;
  }
  return dht_res;
}

void add_to_buf(char sensnr, int *val) {
  *((bufarr + sensnr)->w_ptr) = *val;
  (bufarr + sensnr)->w_ptr =
      (bufarr + sensnr)->s_ptr +
      (((bufarr + sensnr)->w_ptr - (bufarr + sensnr)->s_ptr + 1) %
       (bufarr + sensnr)->bufsize);
}

int grab_value(char sensnr) {
  int out = *((bufarr + sensnr)->r_ptr);
  (bufarr + sensnr)->r_ptr =
      (bufarr + sensnr)->s_ptr +
      (((bufarr + sensnr)->r_ptr - (bufarr + sensnr)->s_ptr + 1) %
       (bufarr + sensnr)->bufsize);
  return out;
}

void close_watch(void) {
  char cntr = 0;
  for (; cntr < 5; cntr++) {
    free((bufarr + cntr)->s_ptr);
  }
  free(bufarr);
}

int flush_buffer_to_db(void) {
  char cntr_o, cntr_i;
  int akk;
  for (cntr_o = 0; cntr_o < 5; cntr_o++) {
    akk = 0;
    for (cntr_i = 0; (bufarr + cntr_o)->r_ptr != (bufarr + cntr_o)->w_ptr;
         cntr_i++) {
      akk += grab_value(cntr_o);
    }
    if(cntr_i){
      akk /= cntr_i;
      insert_db(cntr_o, &akk);
    }
  }
}
