# LightWave Client - Google Geocoding API
This LightWave Client sample application illustrates how to use the Google Geocoding API. Geocoding is the process of converting addresses (like a street address) into geographic coordinates (like latitude and longitude), which you can use to place markers on a map, or position the map. The Geocoding API is a service that provides geocoding and reverse geocoding of addresses.<sup>[1](https://developers.google.com/maps/documentation/geocoding/start)</sup>. For more information on the Google Geocoding API, see [Google Maps Platform | Geocoding API](https://developers.google.com/maps/documentation/geocoding/start)
 
## Prerequisites

+ NonStop C Compiler.
+ An installed instance of [LightWave Client](https://docs.nuwavetech.com/display/LWCLIENT) version 1.0.4 or greater.
+ A Google API Key enabled for the Geocoding API. See [Get an API Key](https://developers.google.com/maps/documentation/geocoding/get-api-key).

The following values must be configured in the SETUP TACL macro after installation of the sample:

+ lwc-isv - The installation subvolume of your LightWave Client software.
+ api-key - The Google API key.

## Install & Build

The application must be built from source. The source files are present in the repository for convenient viewing. 
In addition, a PAK archive containing all of the source files is available for transfer to your NonStop system.

| Repository File | NonStop File | 
| -- | -- |
| macros/build.txt | build |
| macros/loadddl.txt | loadddl |
| macros/logcfg.txt | logcfg |
| macros/setenv.txt | setenv |
| macros/startpw.txt | startpw |
| macros/stoppw.txt | stoppw |
| macros/setenv.txt | unsetenv |
| resources/geoapi.json | geoapi |  
| resources/geoddl.txt | geoddl |
| src/geo.c | geoc |
| geopak.bin | geopak |
  
#### Transfer the PAK archive to your NonStop System

Download `geopak.bin` from this repository to your local system, then upload to your NonStop server using binary transfer mode.

Note: to download the PAK file, click `geopak.bin` in the file list to display the file details, then click the *Download* button.

Logon to TACL on your NonStop system to peform the installation and build steps.

#### Unpak the PAK archive
```
TACL > UNPAK GEOPAK ($*.*.*), VOL $your-volume.your-subvolume.*, LISTALL, MYID
```

#### Build the application 
```
TACL > RUN LOADDDL
TACL > RUN BUILD
```
## Running the application
Customize the SETENV macro for your environment and run it to set the required PARAMs.
```
TACL> T/EDIT SETENV
TACL> RUN SETENV
```
Start the LightWave Client Pathway
```
TACL > RUN STARTPW
```
Run the application
```
TACL > RUN GEO

LightWave Client(tm) - Google Geocoding API - Test Driver - 19AUG2019

Address? 301 edgewater place, suite 100, wakefield, ma

================================================================================
 301 Edgewater Pl #100, Wakefield, MA 01880, USA
================================================================================

latitude: 42.5215689  longitude: -71.0417195  resolution: ROOFTOP

Short Name -------------  Long Name ---------------------- Type(s) -------------
100                       100                              subpremise
301                       301                              street_number
Edgewater Pl              Edgewater Place                  route
Wakefield                 Wakefield                        locality political
Middlesex County          Middlesex County                 administrative_area_level_2 political
MA                        Massachusetts                    administrative_area_level_1 political
US                        United States                    country political
01880                     01880                            postal_code

Address?
```
Assistance is available through the [NuWave Technologies Support Center](http://support.nuwavetech.com).
