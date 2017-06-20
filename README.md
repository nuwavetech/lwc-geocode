Geocode
=======
This is the Geocode sample application for LightWave Client. It demonstrates calling the Google Geocode API (https://developers.google.com/maps/documentation/geocoding) from a NonStop application via LightWave Client.

### Prerequisites

* NonStop C Compiler
* LightWave Client (http://download.nuwavetech.com)
* Google API Key (https://console.developers.google.com/apis)

You can create a Google API Key for free. It must be enabled for the Google Maps Geocoding API.

### Installation

The Geocode application must be built from source. Upload the following files to any empty subvolume on your NonStop system using ASCII transfer mode:

| repository file | NonStop file |
| -- | -- |
| include/geocode.h | geocodeh |
| macros/build.txt | build |
| macros/geocode.txt | geocode|
| macros/loadddl.txt | loadddl |
| macros/logopts.txt | logopts |
| macros/startpw.txt | startpw |
| resources/api.json | api |
| resources/ddlsrc.txt | ddlsrc |
| src/geocode.c | geocodec |

### Building the application
To build the application, execute the following commands from a TACL prompt:
```
> RUN LOADDDL
> RUN BUILD
> TEDIT APIKEY (paste your Google API key, then save and exit)
```
### Running the application
Start LightWave Client
```
> RUN STARTPW
```
Run the application
```
> RUN GEOCODE
```
Assistance is available through the [NuWave Technologies Support Center](http://support.nuwavetech.com).
