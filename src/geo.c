/**
 *
 *  Copyright (c) 2019 NuWave Technologies, Inc. All rights reserved.
 *
 */

#pragma nolist
#include <cextdecs>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#pragma list
  
#include "geo.h"

#define min(x, y) (x < y ? x : y)

static const char* pathmon_name;
static const char* server_class;

static void doGeocodeRequest(const char* address);
static const char* getEnvValue(const char* name, int required);
static void printLocation(geocode_def* geocode);
static char* formatNumeric(long long value, short scale);
static void printReplyError(lightwave_error_rp_def* error);
static void printSendError(void);

int main(int argc, char** argv, char** env) {
  char address[256];

  printf("\nLightWave Client(tm) - Google Geocoding API "
         "- Test Driver - 19AUG2019\n");

  /* Get variables from the environment. These are set by the SETENV macro. */
  if ((pathmon_name = getEnvValue("PATHMON-NAME", 1)) == NULL) {
    return 0;
  }

  if ((server_class = getEnvValue("SERVER-CLASS", 1)) == NULL) {
    return 0;
  }

  /* Get lookup address. */
  while (1) {

    /* Get lookup address. */
    printf("\nAddress? ");
    if (scanf("%239[^\n]", address) <= 0) {
      break;
    }
  
    if (strcasecmp(address, "exit") == 0 || strcasecmp(address, "quit") == 0) {
      break;
    }

    /* Flush leftover input. */ 
    while (getchar() != '\n')
      ;

    doGeocodeRequest(address);
  }

  printf("\n");
  return 0;
}

static void doGeocodeRequest(const char* address) {

  int rc;
  union {
    lightwave_rp_header_def rp_header;
    lightwave_error_rp_def rp_error;
    get_geocode_rq_def get_geocode_rq;
    get_geocode_200_rp_def get_geocode_rp_200;
  } message_buffer;

  /*  initialize the message buffer and set request code  */
  memset(&message_buffer, 0, sizeof(message_buffer));
  message_buffer.get_geocode_rq.lightwave_rq_header.rq_code = rq_get_geocode;

  /*  set api request parameters  */
  strncpy(message_buffer.get_geocode_rq.address_rw, address,
          sizeof(message_buffer.get_geocode_rq.address_rw) - 1);

  /*  send request to LightWave  */
  rc = SERVERCLASS_SEND_((char*)pathmon_name, (short)strlen(pathmon_name),
                         (char*)server_class, (short)strlen(server_class),
                         (char*)&message_buffer, sizeof(get_geocode_rq_def),
                         sizeof(message_buffer));

  if (rc != 0) {
    printSendError();
    return;
  }

  if (message_buffer.rp_header.rp_code == lw_rp_error) {
    printReplyError(&message_buffer.rp_error);
  } else {
    switch (message_buffer.rp_header.http_status) {
    case 200:
      printLocation(&message_buffer.get_geocode_rp_200.geocode);
      break;
    default:
      break;
    }
  }

  return;
}

static void printLocation(geocode_def* geocode) {

  int x, y, z;

  if (strcmp(geocode->status_rw, "OK")) {

    printf("status=%s\n", geocode->status_rw);
    printf("message=%s\n", geocode->error_message);

  } else {

    int maxsize = sizeof(geocode->results) / sizeof(geocode->results[0]);
    if (geocode->results_count > maxsize) {
      printf("%d results available, %d returned\n", geocode->results_count,
             maxsize);
    }

    for (x = 0; x < min(geocode->results_count, maxsize); x++) {

      int maxsize;

      printf("\n==============================================================="
             "=================\n");
      printf(" %s", geocode->results[x].formatted_address);
      printf("\n==============================================================="
             "=================\n");
      printf("\nlatitude: %s",
             formatNumeric(geocode->results[x].geometry.location.lat, 13));
      printf("  longitude: %s",
             formatNumeric(geocode->results[x].geometry.location.lng, 13));
      printf("  resolution: %s\n", geocode->results[x].geometry.location_type);

      maxsize = sizeof(geocode->results[x].address_components) /
                sizeof(geocode->results[x].address_components[0]);

      printf("\nShort Name -------------  "
             "Long Name ---------------------- "
             "Type(s) -------------\n");

      for (y = 0;
           y < min(geocode->results[x].address_components_count, maxsize);
           y++) {

        address_components_type_def* address =
            &geocode->results[x].address_components[y];
        int maxsize;

        printf("%-24.24s  %-32.32s", address->short_name, address->long_name);

        maxsize = sizeof(address->types) / sizeof(address->types[0]);
        for (z = 0; z < min(address->types_count, maxsize); z++) {
          printf(" %s", address->types[z]);
        }

        printf("\n");
      }
    }
  }

  return;
}

static char* formatNumeric(long long value, short scale) {

  short RC;
  char iformat[256]; /* describes the output in internal format */
  short count = 0;
  char* eformat = "M'ZZZ9.9999999'";

  static char buffer[24];
  char* string = buffer;
  short length = 0;

  RC = FORMATCONVERTX(iformat, (short)sizeof(iformat), eformat,
                      (short)strlen(eformat), NULL, &count, 1);
  if (RC <= 0) {
    printf("FORMATCONVERTX=%d\n", RC);
  } else {

    struct {
      long long* dataptr;
      short datatype;
      short databytes;
      short occurs;
    } list[1];

    list[0].dataptr = &value;
    list[0].datatype = (short)(((scale & 0XFF) << 8) | 6);
    list[0].databytes = 8;
    list[0].occurs = 1;

    RC = FORMATDATAX(buffer, (short)sizeof(buffer), 1, &length, (short*)iformat,
                     (short*)list, 1, 0);
    if (RC != 0) {
      printf("FORMATDATAX=%d\n", RC);
    }
  }

  buffer[length] = 0;
  while (*string == ' ') {
    string++;
  }

  return string;
}

static const char* getEnvValue(const char* name, int required) {
  const char* v = getenv(name);
  if (v == NULL && required != 0) {
    printf("PARAM \"%s\" not set. Did you run SETENV?\n", name);
  }
  return v;
}

static void printReplyError(lightwave_error_rp_def* error) {

  static char errortext[lightwave_error_rp_def_Size];
  const char* message = (const char*)&errortext;
  lightwave_rp_header_def* header = &error->lightwave_rp_header;

  switch (header->rp_code) {
  case lw_rp_success:
    message = "Successful completion";
    break;

  case lw_rp_info:
    switch (header->info_code) {
    case lw_info_field_truncated:
      message = "warning: field was truncated";
      break;
    case lw_info_array_truncated:
      message = "warning: array was truncated";
      break;
    default:
      sprintf(errortext, "warning: unknown info code %d", header->info_code);
      break;
    }
    break;

  case lw_rp_error:
    sprintf(errortext, "error: source=%d code=%d subcode=%d\n%s",
            (int)error->error_source, (int)error->error_code,
            (int)error->error_subcode, error->error_message);
    break;

  default:
    sprintf(errortext, "error: unknown reply code %d", header->rp_code);
    break;
  }

  printf("%s\n", message);
}

static void printSendError(void) {
  short prc;
  short frc;

  SERVERCLASS_SEND_INFO_(&prc, &frc);

  printf("\n\nSERVERCLASS_SEND_ error %hd:%hd occurred.\n\n", prc, frc);
  switch (frc) {
  case 14:
    printf("Did you start the pathway by running STARTPW?\n\n");
    break;
  default:
    break;
  }

  return;
}

/* End of file. */
