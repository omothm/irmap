/*
   ETE3007-IRMAP - Infrared Map Constructor Robot

   Copyright (C) 2020  Omar Othman

   This program is free software: you can redistribute it and/or modify it under
   the terms of the GNU Affero General Public License as published by the Free
   Software Foundation, either version 3 of the License, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
   details.

   You should have received a copy of the GNU Affero General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

/*
   ETE3007-IRMAP - Infrared Map Constructor Robot

   Project by: Aydin Altun, Burak Sincek, Omar Othman
   Date:       29.05.2020

   This script is part of the ETE3007-IRMAP project undergone as the term
   project for the ETE 3007 (Fundamental Robotics) class of spring 2020 (during
   the COVID-19 pandemic) at Dokuz Eylul University, Turkey.

   The components required for this code to execute are described in the
   accompanying report (part of which may be present in the README file of the
   repository).

   This script is loaded onto a Wemos board (D1 mini). The board will start a
   soft access point that other devices can connect to using the SSID and
   passphrase defined below. It will also start an HTTP server that listens on
   the IP and port defined below. The expected request format is

       http://192.168.30.7:80/?angle=[ANGLE]

   where [ANGLE] is the desired angle for the robot's IR sensor, between 0 and
   180. (Please refer to the definitions below for the actual IP, port, and
   argument name and limits).

   The response will be in JSON format. An example successful response:

      {
          "angle": "ANGLE",
          "distance": "DISTANCE"
      }

   where DISTANCE is the measured distance as an integer value between 10 and
   80. An example unsuccessful response:

      {
          "error": "ERROR_MSG"
      }

   where ERROR_MSG explains the reason for the error. (Please refer to the
   definitions below for the actual attribute names and limits).
   
*/

#include <assert.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <Servo.h>

// Hardware definitions:

// Board (Wemos D1 mini)
#define SERIAL_BAUD 115200 // arbitrary
// Servo pin is arbitrary but must support PWM. All IO pins of Wemos D1 mini
// support PWM except D0
#define PIN_SERVO D1
// Sensor pin must be analog
#define PIN_SENSOR A0

// Servo motor (Tower Pro SG-5010)
#define SERVO_MIN_ANGLE 0 
#define SERVO_MAX_ANGLE 180
// The servo requires around 580 milliseconds to go all the way from min to max
// angle
#define WIDEST_ROTATION_DELAY 580

// IR sensor (Sharp GP2Y0A21YK)
#define IR_MIN_DISTANCE 10
#define IR_MAX_DISTANCE 80

// WiFi definitions
#define SSID "ETE3007-IRMAP"  // max 31 chars
#define PASSPHRASE "wemosmap" // max 63 chars (and for WPA2 min 8 chars)

// Server definitions (arbitrary values except for the default port)
#define IP "192.168.30.7"
#define GATEWAY "192.168.30.1"
#define SUBNET "255.255.255.0"
#define PORT 80 // default http port

// Request definitions
#define ARG_ANGLE "angle"

// Response definitions
#define HTTP_SUCCESS 200
#define HTTP_BAD_REQUEST 400
#define RESPONSE_TYPE "application/json"
#define RESPONSE_ATTR_ANGLE "angle"
#define REPSONSE_ATTR_DISTANCE "distance"

// Error response definitions
String errorAngleLimits;
String errorNoAngle;
String errorNonIntegerAngle;

// Initialize global objects
Servo servo;
ESP8266WebServer server(PORT);

// Global variables
bool wiFiError = false;

void setup() {
  delay(3000); // for debugging purposes only, should be deleted

  // Start the Serial
  Serial.begin(SERIAL_BAUD);

  // Servo
  uint8_t p = setupServo();
  Serial.print("Servo ready on pin with ID ");
  Serial.println(p);

  // Wi-Fi:
  // We need to set up the on-board Wi-Fi chip as an access point to allow other
  // devices to connect to it.
  bool b = setupWiFi();
  if (b) {
    Serial.print("Wi-Fi access point ready. IP: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("Failed to setup Wi-Fi");
    wiFiError = true;
    return;
  }

  // Server:
  // We then need to start an HTTP server that accepts requests from local
  // clients and sends back a response.
  setupServer();
  Serial.println("HTTP server ready");
}

void loop() {
  // If we have a Wi-Fi error, print a warning and return
  if (wiFiError) {
    Serial.print("Wi-Fi error occurred. Review the Serial.");
    delay(3000);
    return;
  }

  // Handle server requests
  server.handleClient();
}

/**
   Sets up the Wi-Fi access point and starts it.

   @return true if the setup was successful, false otherwise and the error is
           printed to the Serial.
*/
void setupIrSensor() {
  // TODO
}

/**
 * Sets up the Wi-Fi access point and starts it.
 *
 * @return true if the setup was successful, false otherwise and the error is
 *         printed to the Serial.
 */
bool setupWiFi() {
  // A generic boolean to hold the result of operations
  bool ret;

  // Prepare some strings describing error responses sent to the client.
  defineErrorResponses();

  // The ESP8266 Wi-Fi library has default IP address used for the access point,
  // but we want to configure it to use our own values for consistency across
  // the project.
  IPAddress ip, gateway, subnet;
  ret = ip.fromString(IP);
  if (!ret) {
    Serial.println("Error: invalid IP address");
    return false;
  }
  ret = gateway.fromString(GATEWAY);
  if (!ret) {
    Serial.println("Error: invalid gateway address");
    return false;
  }
  ret = subnet.fromString(SUBNET);
  if (!ret) {
    Serial.println("Error: invalid subnet address");
    return false;
  }

  // Disconnent from previously connected networks if any. We need this because
  // the WiFi API saves previous network details and attempts connecting to them
  // once the device is powered up.
  ret = WiFi.disconnect();
  if (!ret) {
    Serial.println("Error: failed to disconnect from previous network");
    return false;
  }

  // Set the Wi-Fi mode to access point
  ret = WiFi.mode(WIFI_AP);
  if (!ret) {
    Serial.println("Error: failed to set the Wi-Fi mode");
    return false;
  }

  // Configure the access point using the IP addresses (instead of default
  // values)
  ret = WiFi.softAPConfig(ip, gateway, subnet);
  if (!ret) {
    Serial.println("Error: failed to configure access point");
    return false;
  }

  // Start the access point using the configured SSID and passphrase. "soft"
  // means the access point acts as its own gateway, i.e. it is not a "bridge"
  // to some other router
  ret = WiFi.softAP(SSID, PASSPHRASE);
  if (!ret) {
    Serial.println("Error: failed to start the access point");
    return false;
  }

  // All commands succeeded
  return true;
}

/**
   Starts the HTTP server and assigns request handlers.
*/
void setupServer() {
  // Register a function to handle requests with a URI of "/" (root), i.e.
  // requests at "http://192.168.30.7:80/" with no path after the final slash.
  server.on("/", handleRoot);

  // Start the server
  server.begin();
}

/**
   Prepares some strings describing error responses sent to the client.
*/
void defineErrorResponses() {
  errorAngleLimits = "The angle must be between ";
  errorAngleLimits += SERVO_MIN_ANGLE;
  errorAngleLimits += " and ";
  errorAngleLimits += SERVO_MAX_ANGLE;

  // An error to send when no angle argument is found in the request
  errorNoAngle = "The request URL must contain the argument \"" ARG_ANGLE "\"";

  // An error to send when the angle is noninteger
  errorNonIntegerAngle = "The angle must an integer value";
}

/**
   Handles requests at the root URI, i.e. at "http://192.168.30.7:80/" with
   no path after the final slash.
*/
void handleRoot() {
  Serial.print("Request received: ");

  // The request must contain an argument "angle" that requests a distance
  // measurement at that angle. If the request does not contain that argument,
  // send a "no angle" error.
  if (!server.hasArg(ARG_ANGLE)) {
    Serial.println(" no angle");
    badRequest(errorNoAngle);
    return;
  }

  // Get the angle arg: intVal will put the integer value in angle and return
  // a boolean indicating whether conversion is successful
  int angle;
  bool b = intVal(server.arg(ARG_ANGLE), &angle);

  // If a conversion was erroneous, send a bad request response
  if (!b) {
    badRequest(errorNonIntegerAngle);
    return;
  }

  // If the angle is out of bounds, send a "angle limits" error
  if (angle < SERVO_MIN_ANGLE || angle > SERVO_MAX_ANGLE) {
    Serial.print(" angle out of bounds (");
    Serial.print(angle);
    Serial.println(")");
    badRequest(errorAngleLimits);
    return;
  }

  // Log the received angle
  Serial.print(" angle ");
  Serial.println(angle);

  // Get the distance for that angle
  int distance = getDistance(angle);
  Serial.print("Measured distance: ");
  Serial.println(distance);

  // Send the JSON response
  String response = "{'angle': ";
  response += angle;
  response += ", 'distance': ";
  response += distance;
  response += "}";
  server.send(HTTP_SUCCESS, RESPONSE_TYPE, response);
}

/**
   Gets the distance in centimeters measured at the given angle.

   @param angle the angle at which the measurement is done.

   @return the measured distance in centimeters.
 */
int getDistance(const int angle) {
  
  // First, reorient the servo to the desired angle
  servoGoTo(angle);
  return irMeasureDistance();
}

/**
   Orients the servo motor to the given angle.

   @param angle the angle to orient the servo motor to.
 */
void servoGoTo(const int angle) {
  // At this point, the angle should have been checked for validity. So we're
  // using assert here (which fails the program ungracefully)
  assert(angle >= SERVO_MIN_ANGLE && angle <= SERVO_MAX_ANGLE);
  
  // Rotate the servo to the desired angle
  servo.write(angle);

  // Wait for the servo to reach its destination
  delay(WIDEST_ROTATION_DELAY);
}

/**
   Sends a bad request response with the given error.

   @param error the error to attach to the bad request response.
 */
void badRequest(const String error) {
  server.send(HTTP_BAD_REQUEST, RESPONSE_TYPE, "{'error': '" + error + "'}");
}

/**
   Converts the string in `line` to integer and puts it in `val`. Returns true
   if conversion did not encounter an unexpected character in the string.

   This function was written because the standard C++ library does not offer a
   simple way to convert string to int.

   @param line the string to convert.
   @param val a pointer to an int variable to put the result in.

   @return true if the string does not contain any trailing nonnumeric
           characters, false otherwise.
*/
bool intVal(String line, int *val) {
  // A pointer to the first encountered nonnumeric character in the string
  char *p;
  // Converts the string to long (coerced to int) and puts it in the int pointer
  *val = strtol(line.c_str(), &p, 10);
  // Returns true if the char pointer is null (i.e. nonnumeric character found),
  // false otherwise
  return *p == 0;
}