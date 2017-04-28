#if defined __TANDEM
#pragma nolist
#pragma push warn
#pragma nowarn( 262 )
#pragma pop warn
#define OMIT
#else
#define OMIT 0
#endif

#include <cextdecs>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "geocode.h"

#define min(x,y) ( x < y ? x : y )

#ifdef __TANDEM
#pragma list
#endif

char *pathmon;
char *serverclass = "GOOGLE-GEOCODE";

static void geocode( char *address );
static void OutputLocation( geocode_def *geogcode );
static char *FormatNumeric( long long value, short scale );
static const char *LightWaveErrorToString( lightwave_error_rp_def *error );

#define INPUT_SIZE 256
#define stringize(x) #x
#define TOSTR(x) stringize(x)

int main( int argc, char** argv, char** env ) {

  char address[INPUT_SIZE+1];
  pathmon = getenv( "GEOCODE-PATHMON" );
  if ( !pathmon ) {
    printf( "PARAM \"GEOCODE-PATHMON\" not set for LightWave Client Service\n" );
    return 0;
  }

  printf( "\n"
      "> LightWave Client(tm) 'geocode' Sample Application\n"
      ": Place lookup using Google Geocode API\n" );

  while ( 1 ) {

    fputs( "\nAddress? ", stdout );
    if ( scanf( "%" TOSTR(INPUT_SIZE) "[^\n]", address ) <= 0 ) {
      break;
    }

    while( getchar() != '\n' );  /* consume any leftover input */
    putchar( '\n' );

    geocode( address );

  }

  putchar( '\n' );
  return 0;

}  /*  main  */



void geocode( char *address ) {

  int RC;
  union {
    get_geocode_rq_def request;
    get_geocode_200_rp_def success;
    lightwave_rp_header_def reply;
    lightwave_error_rp_def failure;
  } buffer;


  /*  initialize buffer and set request code  */
  memset( &buffer, 0, sizeof( buffer ) );
  buffer.request.lightwave_rq_header.rq_code = rq_get_geocode;

  /*  set api request parameters  */
  strncpy( buffer.request.address_rw, address, sizeof( buffer.request.address_rw ) - 1 );
  buffer.request.address_rw[ sizeof( buffer.request.address_rw ) - 1 ] = 0;

  /*  send request to LightWave  */

  RC = SERVERCLASS_SEND_( pathmon, (short)strlen( pathmon ), serverclass, (short)strlen( serverclass ), ( char * ) &buffer,
    sizeof( get_geocode_rq_def ), sizeof( buffer ) );

  if ( RC != 0 ) {
    short pse, fse;
    SERVERCLASS_SEND_INFO_( &pse, &fse );
    printf( "SERVERCLASS_SEND_ ERROR=%d,%d\n", pse, fse );
    return;
  }

  /*  check response  */

  if ( buffer.reply.rp_code == lw_rp_error ) {
    printf( "%s\n", LightWaveErrorToString( &buffer.failure ) );
  } else {

    switch( buffer.reply.http_status ) {
      case 200:
        OutputLocation( &buffer.success.geocode );
        break;
      default:
        break;
    }   /*  switch  */

  }

  return;

}   /*  function  */


void OutputLocation( geocode_def *geocode ) {

  int x, y, z;

  if ( strcmp( geocode->status_rw, "OK" ) ) {

    printf( "status=%s\n", geocode->status_rw );
    printf( "message=%s\n", geocode->error_message );

  } else {

    int maxsize = sizeof( geocode->results ) / sizeof( geocode->results[0] );
    if ( geocode->results_count > maxsize ) {
      printf( "%d results available, %d returned\n", geocode->results_count, maxsize );
    }

    for ( x = 0; x < min( geocode->results_count, maxsize ); x++  ) {

      int maxsize;

      printf( "\n================================================================================\n" );
      printf( " %s", geocode->results[x].formatted_address );
      printf( "\n================================================================================\n" );
      printf( "\nlatitude: %s", FormatNumeric( geocode->results[x].geometry.location.lat, 13 ) );
      printf( "  longitude: %s", FormatNumeric( geocode->results[x].geometry.location.lng, 13 ) );
      printf( "  resolution: %s\n", geocode->results[x].geometry.location_type );

      maxsize = sizeof( geocode->results[x].address_components ) / sizeof( geocode->results[x].address_components[0] );

      printf( "\nShort Name -------------  Long Name ---------------------- Type(s) -------------\n" );

      for ( y = 0; y < min( geocode->results[x].address_components_count, maxsize ); y++ ) {

        address_components_type_def *address = &geocode->results[x].address_components[y];
        int maxsize;

        printf( "%-24.24s  %-32.32s", address->short_name, address->long_name );

        maxsize = sizeof( address->types ) / sizeof( address->types[0] );
        for ( z = 0; z < min( address->types_count, maxsize ); z++ ) {
          printf( " %s", address->types[z] );
        }

        printf( "\n" );

      }

    }

  }

  return;

}  /* function */


const char *LightWaveErrorToString( lightwave_error_rp_def *error ) {

  static char errortext[384];
  const char *message = (const char *) &errortext;
  lightwave_rp_header_def *header = &error->lightwave_rp_header;

  switch( header->rp_code ) {
    case lw_rp_success:
      message = "Successful completion";
      break;

    case lw_rp_info:
      switch( header->info_code ) {
        case lw_info_field_truncated:
          message = "warning: field was truncated";
          break;
        case lw_info_array_truncated:
          message = "warning: array was truncated";
          break;
        default:
          sprintf( errortext, "warning: unknown info code %d", header->info_code );
          break;
      }
      break;

    case lw_rp_error:
      sprintf( errortext, "error: source=%d code=%d subcode=%d\n%s",
            (int)error->error_source, (int)error->error_code, (int)error->error_subcode, error->error_message );
      break;

    default:
      sprintf( errortext, "error: unknown reply code %d", header->rp_code );
      break;
  }

  return message;

}   /*  function  */



char *FormatNumeric( long long value, short scale ) {

  short RC;
  char iformat[256];                 /* describes the output in internal format */
  short count = 0;
  char *eformat = "M'ZZZ9.9999999'";

  static char buffer[24];
  char *string = buffer;
  short length = 0;

  RC = FORMATCONVERTX( iformat, (short)sizeof(iformat), eformat, (short)strlen(eformat), NULL, &count, 1 );
  if ( RC <= 0 ) {
    printf( "FORMATCONVERTX=%d\n", RC );
  } else {

    struct {
      long long *dataptr;
      short datatype;
      short databytes;
      short occurs;
    } list[1];

    list[0].dataptr = &value;
    list[0].datatype = (short)( ( ( scale & 0XFF ) << 8 ) | 6 );
    list[0].databytes = 8;
    list[0].occurs = 1;

    RC = FORMATDATAX( buffer, (short)sizeof(buffer), 1, &length, (short *)iformat, (short *)list, 1, 0 );
    if ( RC != 0 ) {
      printf( "FORMATDATAX=%d\n", RC );
    }

  }

  buffer[length] = 0;
  while ( *string == ' ' ) {
    string++;
  }

  return string;

}   /*  function  */


