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

static const char* pathmon_name;
static const char* server_class;

static void doGeocodeRequest(const char* address);
static const char* getEnvValue(const char* name, int required);
static void displayLocation(geocode_def* geocode);
static char* formatNumeric(long long value, short scale);
static void displayReplyError(lightwave_error_rp_def* error);
static void displaySendError(void);

int main(int argc, char** argv, char** env) {
  char address[256];

  printf("\nLightWave Client(tm) - Google Geocoding API "
         "- C Test Driver - 22AUG2019\n\n");

  /* Get variables from the environment. These are set by the SETENV macro. */
  if ((pathmon_name = getEnvValue("PATHMON-NAME", 1)) == NULL) {
    return 0;
  }

  if ((server_class = getEnvValue("SERVER-CLASS", 1)) == NULL) {
    return 0;
  }

  /* Get lookup address. */
  while (1) {
    int count;

    /* Get lookup address. */
    printf("Address? ");
    count = scanf("%239[^\n]", address);

    if (count < 0) {
      break;
    }

    /* Flush leftover input. */
    while (getchar() != '\n')
      ;

    if (count > 0) {
      doGeocodeRequest(address);
    }
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
    displaySendError();
    return;
  }

  if (message_buffer.rp_error.lightwave_rp_header.rp_code != 0) {
    displayReplyError(&message_buffer.rp_error);
    return;
  }

  if (message_buffer.rp_header.http_status != 200) {
    printf("Error: unexpected HTTP Status %d received.\n",
           message_buffer.rp_header.http_status);
    return;
  }

  if (strcmp(message_buffer.get_geocode_rp_200.geocode.status_rw, "OK") != 0) {
    printf("GEOCODE API error:\n");
    printf("    status:   %s\n", message_buffer.get_geocode_rp_200.geocode.status_rw);
    printf("    message:  %s\n", message_buffer.get_geocode_rp_200.geocode.error_message); 
    return;
  }

  displayLocation(&message_buffer.get_geocode_rp_200.geocode);

  return;
}

static void displayLocation(geocode_def* geocode) {

  int i, j, k;

  for (i = 0; i < geocode->results_count; i++) {

    printf(
        "\n================================================================================\n");
    printf(" %s", geocode->results[i].formatted_address);
    printf(
        "\n================================================================================\n");
    printf("\nlatitude: %s",
           formatNumeric(geocode->results[i].geometry.location.lat, 13));
    printf("  longitude: %s",
           formatNumeric(geocode->results[i].geometry.location.lng, 13));
    printf("  resolution: %s\n", geocode->results[i].geometry.location_type);

    printf(
        "\nShort Name -------------  Long Name ---------------------- Type(s) -------------\n");

    for (j = 0; j < geocode->results[i].address_components_count; j++) {

      address_components_type_def* address =
          &geocode->results[i].address_components[j];

      printf("%-24.24s  %-32.32s", address->short_name, address->long_name);

      for (k = 0; k < address->types_count; k++) {
        printf(" %s", address->types[k]);
      }

      printf("\n");
    }
  }

  printf("\n");

  return;
}

static void displayReplyError(lightwave_error_rp_def* error) {

  switch (error->lightwave_rp_header.rp_code) {
  case lw_rp_info:
    switch (error->lightwave_rp_header.info_code) {
    case lw_info_field_truncated:
      printf("Warning: a field was truncated at offset: %d\n",
             error->lightwave_rp_header.info_detail);
      break;
    case lw_info_array_truncated:
      printf("Warning: an array was truncated at offset: %d\n",
             error->lightwave_rp_header.info_detail);
      break;
    default:
      printf("Warning: unknown info code %d\n",
             error->lightwave_rp_header.info_code);
      break;
    }
    break;
  case lw_rp_error:
    printf("Error:\n");
    printf("    source:   %d\n", error->error_source);
    printf("    code:     %d\n", error->error_code);
    printf("    subcode:  %d\n", error->error_code);
    printf("    message:  %-256.256s\n", error->error_message);
    break;
  default:
    printf("Error: unknown error code %s\n",
           error->lightwave_rp_header.rp_code);
    break;
  }
}

static void displaySendError(void) {
  short prc;
  short frc;

  SERVERCLASS_SEND_INFO_(&prc, &frc);

  printf("\n\nSERVERCLASS_SEND_ error %hd:%hd occurred.\n\n", prc, frc);

  if (frc == 14) {
    printf("Did you start the pathway by running STARTPW?\n\n");
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

/* End of file. */
