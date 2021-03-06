************************************************************************
*  Copyright (c) 2019 NuWave Technologies, Inc. All rights reserved.
*
*  Note: The API definition uses stringPadding="zeroes". All strings
*  returned in API responses are padded with LOW-VALUES.
*
************************************************************************
? env common
? save param, startup
 IDENTIFICATION DIVISION.
 PROGRAM-ID.  GEOCODE-COBOL.  
 AUTHOR. NuWave Technologies, Inc.
 ENVIRONMENT DIVISION.

 CONFIGURATION SECTION.
 SOURCE-COMPUTER.  HPE NonStop Server.
 OBJECT-COMPUTER.  HPE NonStop Server.
 SPECIAL-NAMES.

 INPUT-OUTPUT SECTION.  
 FILE-CONTROL. 
   SELECT MY-TERM  
   ASSIGN TO "#DYNAMIC"
   ORGANIZATION IS SEQUENTIAL
   ACCESS MODE IS SEQUENTIAL.  

 DATA DIVISION.
 FILE SECTION.
   FD  MY-TERM
       RECORD CONTAINS 1 TO 79 CHARACTERS
       LABEL RECORDS ARE OMITTED.

   01 MY-TERM-RECORD PICTURE X(79).

 WORKING-STORAGE SECTION.
   01 I                                NATIVE-2.
   01 J                                NATIVE-2.
   01 K                                NATIVE-2.
   01 WS-FORMAT-LAT-LNG                PIC ------9.9(8).      
   01 WS-STRING                        PIC X(256).
   01 WS-ADDRESS                       PIC X(256).
   01 WS-PROMPT                        PIC X(9) VALUE "Address? ".
   01 WS-RC                            NATIVE-2.
   01 WS-ENV-PATHMON-NAME              PIC X(12) VALUE "PATHMON-NAME".  
   01 WS-ENV-SERVER-CLASS              PIC X(12) VALUE "SERVER-CLASS". 
   01 WS-MY-TERM-FILE-NAME             PIC X(64).
   01 WS-MY-TERM-EOF-IND               PIC 9 VALUE 0.
       88 WS-MY-TERM-EOF               VALUE 1. 
   01  WS-PATHMON-NAME                 PIC X(15).
   01  WS-PATHMON-NAME-LEN             NATIVE-2.  
   01  WS-SERVER-CLASS                 PIC X(15).
   01  WS-SERVER-CLASS-LEN             NATIVE-2.
   01  WS-MESSAGE-BUFFER               PIC X(32000).
   01  WS-REQUEST-LEN                  NATIVE-2.
   01  WS-MAXIMUM-REPLY-LEN            NATIVE-2.
   01  WS-ACTUAL-REPLY-LEN             NATIVE-2.
   01  WS-TIMEOUT                      NATIVE-2.
   01  WS-PATHSEND-ERROR               NATIVE-2.
   01  WS-FILE-SYSTEM-ERROR            NATIVE-2.

   COPY GOOGLE-GEOCODE-VAL             IN GEOCOPY.
   COPY GET-GEOCODE-RQ                 IN GEOCOPY.
   COPY GET-GEOCODE-200-RP             IN GEOCOPY.
   COPY LIGHTWAVE-ERROR-RP             IN GEOCOPY.
   COPY LIGHTWAVE-RP-CODE-ENUM         IN GEOCOPY.
   COPY LIGHTWAVE-INFO-CODE-ENUM       IN GEOCOPY.
   COPY RESULTS-TYPE                   IN GEOCOPY.
   COPY ADDRESS-COMPONENTS-TYPE        IN GEOCOPY.

 PROCEDURE DIVISION.
  
 MAIN.
                                                                                                                                                
   DISPLAY " ".
   DISPLAY "LightWave Client(tm) - Google Geocoding API ",
               " - COBOL Test Driver - 22AUG2019".
   DISPLAY " ".
    
*  Get variables from the environment. These are set by the SETENV macro.
   ENTER "SMU_Param_GetText_" USING WS-ENV-PATHMON-NAME, WS-PATHMON-NAME
       GIVING WS-PATHMON-NAME-LEN.  
   IF WS-PATHMON-NAME-LEN = -1
       DISPLAY "PARAM PATHMON-NAME not set. Did you run SETENV?"
       GO TO MAIN-EXIT
   END-IF.

  ENTER "SMU_Param_GetText_" USING WS-ENV-SERVER-CLASS, WS-SERVER-CLASS
       GIVING WS-SERVER-CLASS-LEN.  
   IF WS-SERVER-CLASS-LEN = -1
       DISPLAY "PARAM SERVER-CLASS not set. Did you run SETENV?"
       GO TO MAIN-EXIT
   END-IF.

*  Open the terminal.
   ENTER "MYTERM" USING WS-STRING.
   MOVE SPACES TO WS-MY-TERM-FILE-NAME.
   ENTER "FNAMECOLLAPSE" USING WS-STRING, WS-MY-TERM-FILE-NAME.
   ENTER "COBOL_ASSIGN_" USING MY-TERM
       WS-MY-TERM-FILE-NAME GIVING WS-RC.
   OPEN I-O MY-TERM.

   PERFORM UNTIL WS-MY-TERM-EOF  
       READ MY-TERM WITH PROMPT WS-PROMPT
           AT END
               SET WS-MY-TERM-EOF TO TRUE
           NOT AT END
               MOVE MY-TERM-RECORD TO WS-ADDRESS
               IF WS-ADDRESS NOT = SPACES
                   PERFORM DO-GEOCODE-REQUEST THRU DO-GEOCODE-REQUEST-EXIT
               END-IF                   
       END-READ           
   END-PERFORM.

 MAIN-EXIT.
   EXIT PROGRAM.
   STOP RUN.

 DO-GEOCODE-REQUEST.

   MOVE LOW-VALUES TO GET-GEOCODE-RQ.
   MOVE RQ-GET-GEOCODE TO RQ-CODE OF GET-GEOCODE-RQ.
   MOVE WS-ADDRESS TO ADDRESS-RW OF GET-GEOCODE-RQ.
   MOVE GET-GEOCODE-RQ TO WS-MESSAGE-BUFFER.
   MOVE FUNCTION LENGTH(GET-GEOCODE-RQ) TO WS-REQUEST-LEN. 
   MOVE FUNCTION LENGTH(WS-MESSAGE-BUFFER) TO WS-MAXIMUM-REPLY-LEN.
   MOVE -1 TO WS-TIMEOUT.

   ENTER "SERVERCLASS_SEND_" USING
      WS-PATHMON-NAME
      WS-PATHMON-NAME-LEN
      WS-SERVER-CLASS
      WS-SERVER-CLASS-LEN
      WS-MESSAGE-BUFFER
      WS-REQUEST-LEN
      WS-MAXIMUM-REPLY-LEN
      WS-ACTUAL-REPLY-LEN
      WS-TIMEOUT
      GIVING WS-RC.

   IF WS-RC NOT = 0
       PERFORM DISPLAY-SEND-ERROR THRU DISPLAY-SEND-ERROR-EXIT
       GO TO DO-GEOCODE-REQUEST-EXIT
   END-IF.

   MOVE WS-MESSAGE-BUFFER TO LIGHTWAVE-ERROR-RP.

   IF RP-CODE OF LIGHTWAVE-ERROR-RP NOT = 0
       MOVE WS-MESSAGE-BUFFER TO LIGHTWAVE-ERROR-RP
       PERFORM DISPLAY-REPLY-ERROR THRU DISPLAY-REPLY-ERROR-EXIT
       GO TO DO-GEOCODE-REQUEST-EXIT
   END-IF.

   MOVE WS-MESSAGE-BUFFER TO GET-GEOCODE-200-RP

   IF HTTP-STATUS OF GET-GEOCODE-200-RP NOT = 200
       DISPLAY "Error: unexpected HTTP Status ",
           HTTP-STATUS OF GET-GEOCODE-200-RP, "received."
       GO TO DO-GEOCODE-REQUEST-EXIT               
   END-IF.
  
   UNSTRING STATUS-RW OF GET-GEOCODE-200-RP DELIMITED BY LOW-VALUES   
       INTO WS-STRING
   if WS-STRING NOT = "OK"
       DISPLAY "GEOCODE API error:"
       DISPLAY "    status:   ", STATUS-RW OF GET-GEOCODE-200-RP
       DISPLAY "    message:  ", ERROR-MESSAGE OF GET-GEOCODE-200-RP
       GO TO DO-GEOCODE-REQUEST-EXIT
   END-IF.
     
   PERFORM DISPLAY-LOCATION THRU DISPLAY-LOCATION-EXIT.

 DO-GEOCODE-REQUEST-EXIT.
   EXIT.

 DISPLAY-LOCATION.

   PERFORM VARYING I FROM 1 BY 1 UNTIL I > RESULTS-COUNT OF GET-GEOCODE-200-RP
       MOVE RESULTS OF GET-GEOCODE-200-RP(I) TO RESULTS-TYPE

       DISPLAY " "
       DISPLAY "================================================================================"
       DISPLAY "  ", FORMATTED-ADDRESS OF RESULTS-TYPE(1:75)
       DISPLAY "================================================================================"
       DISPLAY " "           

       MOVE LAT OF LOCATION OF RESULTS-TYPE TO WS-FORMAT-LAT-LNG
       DISPLAY "latitude: ", WS-FORMAT-LAT-LNG NO ADVANCING
       MOVE LNG OF LOCATION OF RESULTS-TYPE TO WS-FORMAT-LAT-LNG
       DISPLAY "  longitude: ", WS-FORMAT-LAT-LNG NO ADVANCING   
       DISPLAY "  resolution: ", LOCATION-TYPE OF RESULTS-TYPE
       DISPLAY " "

       DISPLAY "Short Name -------------  Long Name ---------------------- Type(s) -------------"

       PERFORM VARYING J FROM 1 BY 1 UNTIL J > ADDRESS-COMPONENTS-COUNT OF RESULTS-TYPE

           MOVE ADDRESS-COMPONENTS OF RESULTS-TYPE(J) TO ADDRESS-COMPONENTS-TYPE
           UNSTRING SHORT-NAME OF ADDRESS-COMPONENTS-TYPE DELIMITED BY LOW-VALUES
               INTO WS-STRING
           DISPLAY WS-STRING(1:26) NO ADVANCING    
           UNSTRING LONG-NAME OF ADDRESS-COMPONENTS-TYPE DELIMITED BY LOW-VALUES
               INTO WS-STRING
           DISPLAY WS-STRING(1:33) NO ADVANCING    

           PERFORM VARYING K FROM 1 BY 1 UNTIL K > TYPES-COUNT OF ADDRESS-COMPONENTS-TYPE 
               DISPLAY TYPES OF ADDRESS-COMPONENTS-TYPE(K) NO ADVANCING
               DISPLAY " " NO ADVANCING
           END-PERFORM

           DISPLAY " "
       END-PERFORM
    
      DISPLAY " "

   END-PERFORM.

 DISPLAY-LOCATION-EXIT.
   EXIT.

 DISPLAY-REPLY-ERROR.

   MOVE RP-CODE OF LIGHTWAVE-ERROR-RP TO LIGHTWAVE-RP-CODE-ENUM.

   IF LW-RP-INFO
       MOVE INFO-CODE OF LIGHTWAVE-ERROR-RP TO LIGHTWAVE-INFO-CODE-ENUM
       IF LW-INFO-FIELD-TRUNCATED
           DISPLAY "Warning: A field was truncated at offset: ",
               INFO-DETAIL OF LIGHTWAVE-ERROR-RP
       ELSE IF LW-INFO-ARRAY-TRUNCATED
         DISPLAY "Warning: An array was truncated at offset: ",
               INFO-DETAIL OF LIGHTWAVE-ERROR-RP
       END-IF
   ELSE IF LW-RP-ERROR
*      Truncate the error message
       MOVE ERROR-MESSAGE OF LIGHTWAVE-ERROR-RP TO WS-STRING
       DISPLAY "Error:"
       DISPLAY "    source:   ", ERROR-SOURCE OF LIGHTWAVE-ERROR-RP
       DISPLAY "    code:     ", ERROR-CODE OF LIGHTWAVE-ERROR-RP
       DISPLAY "    subcode:  ", ERROR-SUBCODE OF LIGHTWAVE-ERROR-RP       
       DISPLAY "    message:  ", WS-STRING
   ELSE
       DISPLAY "Unknown reply code: ", RP-CODE OF LIGHTWAVE-ERROR-RP   
   END-IF.

 DISPLAY-REPLY-ERROR-EXIT.
   EXIT.

 DISPLAY-SEND-ERROR.

   ENTER "SERVERCLASS_SEND_INFO_" USING
       WS-PATHSEND-ERROR
       WS-FILE-SYSTEM-ERROR
       GIVING WS-RC.

   DISPLAY " ".
   DISPLAY "SERVERCLASS_SEND_ error ", WS-PATHSEND-ERROR, ":",
       WS-FILE-SYSTEM-ERROR.

   IF WS-FILE-SYSTEM-ERROR = 14
       DISPLAY "Did you start the pathway by running STARTPW?"
   END-IF.
      
   DISPLAY " ".

 DISPLAY-SEND-ERROR-EXIT.
   EXIT.    