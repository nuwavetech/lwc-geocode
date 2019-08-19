/* SCHEMA PRODUCED DATE - TIME : 8/19/2019 - 08:33:49 */
#pragma section lightwave_rp_code_enum
/* Definition LIGHTWAVE-RP-CODE-ENUM created on 08/19/2019 at 08:33 */
enum
{
   lw_rp_success = 0,
   lw_rp_info = 1,
   lw_rp_error = 2
};
typedef short                           lightwave_rp_code_enum_def;
#pragma section lightwave_info_code_enum
/* Definition LIGHTWAVE-INFO-CODE-ENUM created on 08/19/2019 at 08:33 */
enum
{
   lw_info_field_truncated = 100,
   lw_info_array_truncated = 101
};
typedef short                           lightwave_info_code_enum_def;
#pragma section lightwave_error_src_enum
/* Definition LIGHTWAVE-ERROR-SRC-ENUM created on 08/19/2019 at 08:33 */
enum
{
   lw_error_src_lightwave = 1,
   lw_error_src_http = 2,
   lw_error_src_tcpip = 3,
   lw_error_src_ssl = 4
};
typedef short                           lightwave_error_src_enum_def;
#pragma section lightwave_error_enum
/* Definition LIGHTWAVE-ERROR-ENUM created on 08/19/2019 at 08:33 */
enum
{
   lw_error_invalid_license = 1,
   lw_error_invalid_header = 2,
   lw_error_invalid_rq_code = 3,
   lw_error_invalid_type = 4,
   lw_error_serialization_error = 5,
   lw_error_deserialization_error = 6,
   lw_error_request_timeout = 7,
   lw_error_response_not_defined = 12,
   lw_error_internal_error = 500
};
typedef short                           lightwave_error_enum_def;
#pragma section lightwave_rq_header
/* Definition LIGHTWAVE-RQ-HEADER created on 08/19/2019 at 08:33 */
#include <tnsint.h>
#pragma fieldalign shared2 __lightwave_rq_header
typedef struct __lightwave_rq_header
{
   short                           rq_code;
   unsigned short                  rq_version;
   __uint32_t                      rq_timeout;
   char                            reserved[24];
} lightwave_rq_header_def;
#define lightwave_rq_header_def_Size 32
#pragma section lightwave_rp_header
/* Definition LIGHTWAVE-RP-HEADER created on 08/19/2019 at 08:33 */
#pragma fieldalign shared2 __lightwave_rp_header
typedef struct __lightwave_rp_header
{
   short                           rp_code;
   short                           http_status;
   short                           info_code;
   short                           info_detail;
   char                            reserved[24];
} lightwave_rp_header_def;
#define lightwave_rp_header_def_Size 32
#pragma section lightwave_error_rp
/* Definition LIGHTWAVE-ERROR-RP created on 08/19/2019 at 08:33 */
#include <tnsint.h>
#pragma fieldalign shared2 __lightwave_error_rp
typedef struct __lightwave_error_rp
{
   lightwave_rp_header_def         lightwave_rp_header;
   __int32_t                       error_source;
   __int32_t                       error_code;
   __int32_t                       error_subcode;
   char                            error_message[4096];
} lightwave_error_rp_def;
#define lightwave_error_rp_def_Size 4140
#pragma section google_geocode_val
/* Definition GOOGLE-GEOCODE-VAL created on 08/19/2019 at 08:33 */
#pragma fieldalign shared2 __google_geocode_val
typedef struct __google_geocode_val
{
   short                           rq_get_geocode;
   /*value is 1*/
   short                           rq_reverse_geocode;
   /*value is 2*/
} google_geocode_val_def;
#define google_geocode_val_def_Size 4
#pragma section google_geocode_enm
/* Definition GOOGLE-GEOCODE-ENM created on 08/19/2019 at 08:33 */
enum
{
   rq_get_geocode = 1,
   rq_reverse_geocode = 2
};
typedef short                           google_geocode_enm_def;
#pragma section address_components_type
/* Definition ADDRESS-COMPONENTS-TYPE created on 08/19/2019 at 08:33 */
#pragma fieldalign shared2 __address_components_type
typedef struct __address_components_type
{
   char                            long_name[64];
   char                            short_name[24];
   short                           types_count;
   char                            types[4][32];
} address_components_type_def;
#define address_components_type_def_Size 218
#pragma section location
/* Definition LOCATION created on 08/19/2019 at 08:33 */
#pragma fieldalign shared2 __location
typedef struct __location
{
   long long                       lat;
   long long                       lng;
} location_def;
#define location_def_Size 16
#pragma section viewport_type
/* Definition VIEWPORT-TYPE created on 08/19/2019 at 08:33 */
#pragma fieldalign shared2 __viewport_type
typedef struct __viewport_type
{
   location_def                    northeast;
   location_def                    southwest;
} viewport_type_def;
#define viewport_type_def_Size 32
#pragma section geometry_type
/* Definition GEOMETRY-TYPE created on 08/19/2019 at 08:33 */
#pragma fieldalign shared2 __geometry_type
typedef struct __geometry_type
{
   location_def                    location;
   char                            location_type[24];
   viewport_type_def               viewport;
} geometry_type_def;
#define geometry_type_def_Size 72
#pragma section results_type
/* Definition RESULTS-TYPE created on 08/19/2019 at 08:33 */
#pragma fieldalign shared2 __results_type
typedef struct __results_type
{
   short                           address_components_count;
   address_components_type_def     address_components[8];
   char                            formatted_address[128];
   geometry_type_def               geometry;
   char                            place_id[36];
   short                           types_count;
   char                            types[4][32];
} results_type_def;
#define results_type_def_Size 2112
#pragma section geocode
/* Definition GEOCODE created on 08/19/2019 at 08:33 */
#pragma fieldalign shared2 __geocode
typedef struct __geocode
{
   char                            status_rw[24];
   char                            error_message[128];
   short                           results_count;
   results_type_def                results[4];
} geocode_def;
#define geocode_def_Size 8602
#pragma section get_geocode_rq
/* Definition GET-GEOCODE-RQ created on 08/19/2019 at 08:33 */
#pragma fieldalign shared2 __get_geocode_rq
typedef struct __get_geocode_rq
{
   lightwave_rq_header_def         lightwave_rq_header;
   char                            address_rw[256];
   char                            bounds[25];
   char                            language[25];
   char                            region[25];
   char                            components[25];
} get_geocode_rq_def;
#define get_geocode_rq_def_Size 388
#pragma section get_geocode_200_rp
/* Definition GET-GEOCODE-200-RP created on 08/19/2019 at 08:33 */
#pragma fieldalign shared2 __get_geocode_200_rp
typedef struct __get_geocode_200_rp
{
   lightwave_rp_header_def         lightwave_rp_header;
   geocode_def                     geocode;
} get_geocode_200_rp_def;
#define get_geocode_200_rp_def_Size 8634
#pragma section reverse_geocode_rq
/* Definition REVERSE-GEOCODE-RQ created on 08/19/2019 at 08:33 */
#pragma fieldalign shared2 __reverse_geocode_rq
typedef struct __reverse_geocode_rq
{
   lightwave_rq_header_def         lightwave_rq_header;
   char                            latlng[25];
   char                            location_type[25];
   char                            result_type[25];
} reverse_geocode_rq_def;
#define reverse_geocode_rq_def_Size 107
#pragma section reverse_geocode_200_rp
/* Definition REVERSE-GEOCODE-200-RP created on 08/19/2019 at 08:33 */
#pragma fieldalign shared2 __reverse_geocode_200_rp
typedef struct __reverse_geocode_200_rp
{
   lightwave_rp_header_def         lightwave_rp_header;
   geocode_def                     geocode;
} reverse_geocode_200_rp_def;
#define reverse_geocode_200_rp_def_Size 8634
