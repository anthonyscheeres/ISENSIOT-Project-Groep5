/*
 * Google's Firebase Realtime Database Arduino Library for ESP8266, version 3.0.3
 * 
 * November 12, 2020
 * 
 * Updates:
 * - Fix the internal find string function error caused wdt reset. 
 * - Fix the response buffer size issue.
 * 
 * 
 * This library provides ESP8266 to perform REST API by GET PUT, POST, PATCH, DELETE data from/to with Google's Firebase database using get, set, update
 * and delete calls. 
 * 
 * The library was tested and work well with ESP8266 based module and add support for multiple stream event paths.
 * 
 * The MIT License (MIT)
 * Copyright (c) 2019 K. Suwatchai (Mobizt)
 * 
 * 
 * Permission is hereby granted, free of charge, to any person returning a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef FirebaseESP8266_H
#define FirebaseESP8266_H

#ifdef ESP8266

#include <Arduino.h>
#include <SPI.h>
#include <time.h>
#include <SD.h>
#include <vector>
#include <functional>
#include <Schedule.h>
#include <ets_sys.h>
#include <ESP8266WiFi.h>
#include "FirebaseESP8266HTTPClient.h"

#define FS_NO_GLOBALS
#include <FS.h>
#include "FirebaseJson.h"

#define FIREBASE_PORT 443
#define KEEP_ALIVE_TIMEOUT 45000
#define STREAM_ERROR_NOTIFIED_INTERVAL 3000
#define STREAM_RECONNECT_INTERVAL 1000
#define SD_CS_PIN 15
#define MAX_REDIRECT 5
#define WIFI_RECONNECT_TIMEOUT 10000
#define STEAM_STACK_SIZE 8192
#define QUEUE_TASK_STACK_SIZE 8192
#define MAX_BLOB_PAYLOAD_SIZE 1024

struct StorageType
{
  static const uint8_t FLASH = 0;
  static const uint8_t SD = 1;
};

enum fb_esp_fcm_msg_type
{
  msg_single,
  msg_multicast,
  msg_topic
};

enum fb_esp_data_type
{
  d_any,
  d_null,
  d_integer,
  d_float,
  d_double,
  d_boolean,
  d_string,
  d_json,
  d_array,
  d_blob,
  d_file,
  d_timestamp,
  d_shallow,
};

enum fb_esp_method
{
  m_put,
  m_put_nocontent,
  m_post,
  m_get,
  m_get_nocontent,
  m_stream,
  m_patch,
  m_patch_nocontent,
  m_delete,
  m_download,
  m_restore,
  m_read_rules,
  m_set_rules,
  m_get_shallow,
  m_get_priority,
  m_set_priority,
};

struct server_response_data_t
{
  int httpCode = -1;
  int payloadLen = -1;
  int contentLen = -1;
  fb_esp_data_type dataType = fb_esp_data_type::d_any;
  int payloadOfs = 0;
  bool boolData = false;
  int intData = 0;
  float floatData = 0.0f;
  double doubleData = 0.0f;
  std::vector<uint8_t> blobData;
  bool isEvent = false;
  bool noEvent = false;
  bool isChunkedEnc = false;
  bool hasEventData = false;
  bool noContent = false;
  bool eventPathChanged = false;
  bool dataChanged = false;
  std::string location = "";
  std::string contentType = "";
  std::string connection = "";
  std::string eventPath = "";
  std::string eventType = "";
  std::string eventData = "";
  std::string etag = "";
  std::string pushName = "";
  std::string fbError = "";
  std::string transferEnc = "";
};

static const char fb_esp_pgm_str_1[] PROGMEM = "/";
static const char fb_esp_pgm_str_2[] PROGMEM = ".json?auth=";
static const char fb_esp_pgm_str_3[] PROGMEM = "\"";
static const char fb_esp_pgm_str_4[] PROGMEM = ".";
static const char fb_esp_pgm_str_5[] PROGMEM = "HTTP/1.1 ";
static const char fb_esp_pgm_str_6[] PROGMEM = " ";
static const char fb_esp_pgm_str_7[] PROGMEM = ":";
static const char fb_esp_pgm_str_8[] PROGMEM = "Content-Type: ";
static const char fb_esp_pgm_str_9[] PROGMEM = "text/event-stream";
static const char fb_esp_pgm_str_10[] PROGMEM = "Connection: ";
static const char fb_esp_pgm_str_11[] PROGMEM = "keep-alive";
static const char fb_esp_pgm_str_12[] PROGMEM = "Content-Length: ";
static const char fb_esp_pgm_str_13[] PROGMEM = "event: ";
static const char fb_esp_pgm_str_14[] PROGMEM = "data: ";
static const char fb_esp_pgm_str_15[] PROGMEM = "put";
static const char fb_esp_pgm_str_16[] PROGMEM = "patch";
static const char fb_esp_pgm_str_17[] PROGMEM = "\"path\":\"";
static const char fb_esp_pgm_str_18[] PROGMEM = "\"data\":";
static const char fb_esp_pgm_str_19[] PROGMEM = "null";
static const char fb_esp_pgm_str_20[] PROGMEM = "{\"name\":\"";
static const char fb_esp_pgm_str_21[] PROGMEM = "\r\n";
static const char fb_esp_pgm_str_22[] PROGMEM = "GET ";
static const char fb_esp_pgm_str_23[] PROGMEM = "PUT";
static const char fb_esp_pgm_str_24[] PROGMEM = "POST";
static const char fb_esp_pgm_str_25[] PROGMEM = "GET";
static const char fb_esp_pgm_str_26[] PROGMEM = "PATCH";
static const char fb_esp_pgm_str_27[] PROGMEM = "DELETE";
static const char fb_esp_pgm_str_28[] PROGMEM = "&download=";
static const char fb_esp_pgm_str_29[] PROGMEM = "&print=silent";
static const char fb_esp_pgm_str_30[] PROGMEM = " HTTP/1.1\r\n";
static const char fb_esp_pgm_str_31[] PROGMEM = "Host: ";
static const char fb_esp_pgm_str_32[] PROGMEM = "User-Agent: ESP\r\n";
static const char fb_esp_pgm_str_33[] PROGMEM = "X-Firebase-Decoding: 1\r\n";
static const char fb_esp_pgm_str_34[] PROGMEM = "Connection: close\r\n";
static const char fb_esp_pgm_str_35[] PROGMEM = "Accept: text/event-stream\r\n";
static const char fb_esp_pgm_str_36[] PROGMEM = "Connection: keep-alive\r\n";
static const char fb_esp_pgm_str_37[] PROGMEM = "Keep-Alive: timeout=30, max=100\r\n";
static const char fb_esp_pgm_str_38[] PROGMEM = "Accept-Encoding: identity;q=1,chunked;q=0.1,*;q=0\r\n";
static const char fb_esp_pgm_str_39[] PROGMEM = "connection refused";
static const char fb_esp_pgm_str_40[] PROGMEM = "send header failed";
static const char fb_esp_pgm_str_41[] PROGMEM = "send payload failed";
static const char fb_esp_pgm_str_42[] PROGMEM = "not connected";
static const char fb_esp_pgm_str_43[] PROGMEM = "connection lost";
static const char fb_esp_pgm_str_44[] PROGMEM = "no HTTP server";
static const char fb_esp_pgm_str_45[] PROGMEM = "bad request, verify the Firebase credentials and the node path";
static const char fb_esp_pgm_str_46[] PROGMEM = "non-authoriative information";
static const char fb_esp_pgm_str_47[] PROGMEM = "no content";
static const char fb_esp_pgm_str_48[] PROGMEM = "moved permanently";
static const char fb_esp_pgm_str_49[] PROGMEM = "use proxy";
static const char fb_esp_pgm_str_50[] PROGMEM = "temporary redirect";
static const char fb_esp_pgm_str_51[] PROGMEM = "permanent redirect";
static const char fb_esp_pgm_str_52[] PROGMEM = "unauthorized";
static const char fb_esp_pgm_str_53[] PROGMEM = "forbidden";
static const char fb_esp_pgm_str_54[] PROGMEM = "not found";
static const char fb_esp_pgm_str_55[] PROGMEM = "method not allow";
static const char fb_esp_pgm_str_56[] PROGMEM = "not acceptable";
static const char fb_esp_pgm_str_57[] PROGMEM = "proxy authentication required";
static const char fb_esp_pgm_str_58[] PROGMEM = "request timeout";
static const char fb_esp_pgm_str_59[] PROGMEM = "length required";
static const char fb_esp_pgm_str_60[] PROGMEM = "too many requests";
static const char fb_esp_pgm_str_61[] PROGMEM = "request header fields too larg";
static const char fb_esp_pgm_str_62[] PROGMEM = "internal server error";
static const char fb_esp_pgm_str_63[] PROGMEM = "bad gateway";
static const char fb_esp_pgm_str_64[] PROGMEM = "service unavailable";
static const char fb_esp_pgm_str_65[] PROGMEM = "gateway timeout";
static const char fb_esp_pgm_str_66[] PROGMEM = "http version not support";
static const char fb_esp_pgm_str_67[] PROGMEM = "network authentication required";
static const char fb_esp_pgm_str_68[] PROGMEM = "data buffer overflow";
static const char fb_esp_pgm_str_69[] PROGMEM = "read Timeout";
static const char fb_esp_pgm_str_70[] PROGMEM = "data type mismatch";
static const char fb_esp_pgm_str_71[] PROGMEM = "path not exist";
static const char fb_esp_pgm_str_72[] PROGMEM = "task";
static const char fb_esp_pgm_str_73[] PROGMEM = "/esp.x";
static const char fb_esp_pgm_str_74[] PROGMEM = "json";
static const char fb_esp_pgm_str_75[] PROGMEM = "string";
static const char fb_esp_pgm_str_76[] PROGMEM = "float";
static const char fb_esp_pgm_str_77[] PROGMEM = "int";
static const char fb_esp_pgm_str_78[] PROGMEM = "null";
static const char fb_esp_pgm_str_79[] PROGMEM = ";";
static const char fb_esp_pgm_str_80[] PROGMEM = "Content-Disposition: ";
static const char fb_esp_pgm_str_81[] PROGMEM = "application/octet-stream";
static const char fb_esp_pgm_str_82[] PROGMEM = "attachment";
static const char fb_esp_pgm_str_83[] PROGMEM = "The backup file is not exist";
static const char fb_esp_pgm_str_84[] PROGMEM = "The SD card is in use";
static const char fb_esp_pgm_str_85[] PROGMEM = "The SD card is not available";
static const char fb_esp_pgm_str_86[] PROGMEM = "Could not read/write the backup file";
static const char fb_esp_pgm_str_87[] PROGMEM = "Transmission error, ";
static const char fb_esp_pgm_str_88[] PROGMEM = "Node path is not exist";
static const char fb_esp_pgm_str_89[] PROGMEM = ".json";
static const char fb_esp_pgm_str_90[] PROGMEM = "/root.json";
static const char fb_esp_pgm_str_91[] PROGMEM = "blob";
static const char fb_esp_pgm_str_92[] PROGMEM = "\"blob,base64,";
static const char fb_esp_pgm_str_93[] PROGMEM = "\"file,base64,";
static const char fb_esp_pgm_str_94[] PROGMEM = "http connection was used by other processes";
static const char fb_esp_pgm_str_95[] PROGMEM = "Location: ";
static const char fb_esp_pgm_str_96[] PROGMEM = "&orderBy=";
static const char fb_esp_pgm_str_97[] PROGMEM = "&limitToFirst=";
static const char fb_esp_pgm_str_98[] PROGMEM = "&limitToLast=";
static const char fb_esp_pgm_str_99[] PROGMEM = "&startAt=";
static const char fb_esp_pgm_str_100[] PROGMEM = "&endAt=";
static const char fb_esp_pgm_str_101[] PROGMEM = "&equalTo=";
static const char fb_esp_pgm_str_102[] PROGMEM = "\"error\" : ";
static const char fb_esp_pgm_str_103[] PROGMEM = "/.settings/rules";
static const char fb_esp_pgm_str_104[] PROGMEM = "{\"status\":\"ok\"}";
static const char fb_esp_pgm_str_105[] PROGMEM = "boolean";
static const char fb_esp_pgm_str_106[] PROGMEM = "false";
static const char fb_esp_pgm_str_107[] PROGMEM = "true";
static const char fb_esp_pgm_str_108[] PROGMEM = "double";
static const char fb_esp_pgm_str_109[] PROGMEM = "cancel";
static const char fb_esp_pgm_str_110[] PROGMEM = "auth_revoked";
static const char fb_esp_pgm_str_111[] PROGMEM = "http://";
static const char fb_esp_pgm_str_112[] PROGMEM = "https://";
static const char fb_esp_pgm_str_113[] PROGMEM = "_stream";
static const char fb_esp_pgm_str_114[] PROGMEM = "_error_queue";
static const char fb_esp_pgm_str_115[] PROGMEM = "get";
static const char fb_esp_pgm_str_116[] PROGMEM = "set";
static const char fb_esp_pgm_str_117[] PROGMEM = "push";
static const char fb_esp_pgm_str_118[] PROGMEM = "update";
static const char fb_esp_pgm_str_119[] PROGMEM = "delete";

static const char fb_esp_pgm_str_120[] PROGMEM = "fcm.googleapis.com";
static const char fb_esp_pgm_str_121[] PROGMEM = "/fcm/send";
static const char fb_esp_pgm_str_122[] PROGMEM = "notification";
static const char fb_esp_pgm_str_123[] PROGMEM = "notification/title";
static const char fb_esp_pgm_str_124[] PROGMEM = "notification/body";
static const char fb_esp_pgm_str_125[] PROGMEM = "notification/icon";
static const char fb_esp_pgm_str_126[] PROGMEM = "notification/click_action";
static const char fb_esp_pgm_str_127[] PROGMEM = "}";
static const char fb_esp_pgm_str_128[] PROGMEM = "to";
static const char fb_esp_pgm_str_129[] PROGMEM = "application/json";
static const char fb_esp_pgm_str_130[] PROGMEM = "registration_ids";
static const char fb_esp_pgm_str_131[] PROGMEM = "Authorization: key=";
static const char fb_esp_pgm_str_132[] PROGMEM = ",";
static const char fb_esp_pgm_str_133[] PROGMEM = "]";
static const char fb_esp_pgm_str_134[] PROGMEM = "/topics/";
static const char fb_esp_pgm_str_135[] PROGMEM = "data";
static const char fb_esp_pgm_str_136[] PROGMEM = "priority";
static const char fb_esp_pgm_str_137[] PROGMEM = "time_to_live";
static const char fb_esp_pgm_str_138[] PROGMEM = "collapse_key";
static const char fb_esp_pgm_str_139[] PROGMEM = "\"multicast_id\":";
static const char fb_esp_pgm_str_140[] PROGMEM = "\"success\":";
static const char fb_esp_pgm_str_141[] PROGMEM = "\"failure\":";
static const char fb_esp_pgm_str_142[] PROGMEM = "\"canonical_ids\":";
static const char fb_esp_pgm_str_143[] PROGMEM = "\"results\":";
static const char fb_esp_pgm_str_144[] PROGMEM = "No topic provided";
static const char fb_esp_pgm_str_145[] PROGMEM = "No device token provided";
static const char fb_esp_pgm_str_146[] PROGMEM = "No server key provided";
static const char fb_esp_pgm_str_147[] PROGMEM = "The index of recipient device registered token not found";
static const char fb_esp_pgm_str_148[] PROGMEM = "X-Firebase-ETag: true\r\n";
static const char fb_esp_pgm_str_149[] PROGMEM = "if-match: ";
static const char fb_esp_pgm_str_150[] PROGMEM = "ETag: ";
static const char fb_esp_pgm_str_151[] PROGMEM = "null_etag";
static const char fb_esp_pgm_str_152[] PROGMEM = "Precondition Failed (ETag is not match)";
static const char fb_esp_pgm_str_153[] PROGMEM = "X-HTTP-Method-Override: ";
static const char fb_esp_pgm_str_154[] PROGMEM = "{\".sv\": \"timestamp\"}";
static const char fb_esp_pgm_str_155[] PROGMEM = "&shallow=true";
static const char fb_esp_pgm_str_156[] PROGMEM = "/.priority";
static const char fb_esp_pgm_str_157[] PROGMEM = ",\".priority\":";
static const char fb_esp_pgm_str_158[] PROGMEM = "&timeout=";
static const char fb_esp_pgm_str_159[] PROGMEM = "ms";
static const char fb_esp_pgm_str_160[] PROGMEM = "&writeSizeLimit=";
static const char fb_esp_pgm_str_161[] PROGMEM = "{\".value\":";
static const char fb_esp_pgm_str_162[] PROGMEM = "&format=export";
static const char fb_esp_pgm_str_163[] PROGMEM = "{";
static const char fb_esp_pgm_str_164[] PROGMEM = "Flash memory was not ready";
static const char fb_esp_pgm_str_165[] PROGMEM = "array";
static const char fb_esp_pgm_str_166[] PROGMEM = "\".sv\"";
static const char fb_esp_pgm_str_167[] PROGMEM = "Transfer-Encoding: ";
static const char fb_esp_pgm_str_168[] PROGMEM = "chunked";
static const char fb_esp_pgm_str_169[] PROGMEM = "Maximum Redirection reached";
static const char fb_esp_pgm_str_170[] PROGMEM = "?auth=";
static const char fb_esp_pgm_str_171[] PROGMEM = "&auth=";
static const char fb_esp_pgm_str_172[] PROGMEM = "&";
static const char fb_esp_pgm_str_173[] PROGMEM = "?";
static const char fb_esp_pgm_str_174[] PROGMEM = ".com";
static const char fb_esp_pgm_str_175[] PROGMEM = ".net";
static const char fb_esp_pgm_str_176[] PROGMEM = ".int";
static const char fb_esp_pgm_str_177[] PROGMEM = ".edu";
static const char fb_esp_pgm_str_178[] PROGMEM = ".gov";
static const char fb_esp_pgm_str_179[] PROGMEM = ".mil";
static const char fb_esp_pgm_str_180[] PROGMEM = "\n";
static const char fb_esp_pgm_str_181[] PROGMEM = "\r\n\r\n";
static const char fb_esp_pgm_str_182[] PROGMEM = "[";
static const char fb_esp_pgm_str_183[] PROGMEM = "file";
static const char fb_esp_pgm_str_184[] PROGMEM = "/fb_bin_0.tmp";
static const char fb_esp_pgm_str_185[] PROGMEM = "The backup data should be the JSON object";
static const char fb_esp_pgm_str_186[] PROGMEM = "object";
static const char fb_esp_pgm_str_187[] PROGMEM = "pool.ntp.org";
static const char fb_esp_pgm_str_188[] PROGMEM = "time.nist.gov";
static const char fb_esp_pgm_str_189[] PROGMEM = "payload too large";
static const char fb_esp_pgm_str_190[] PROGMEM = "cannot config time";
static const char fb_esp_pgm_str_191[] PROGMEM = "SSL client rx buffer size is too small";
static const char fb_esp_pgm_str_192[] PROGMEM = "File I/O error";

static const unsigned char ESP8266_FIREBASE_base64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

class FirebaseData;
class StreamData;
class MultiPathStreamData;
class QueueInfo;
class FirebaseESP8266;
class FCMObject;

typedef std::function<void(void)> callback_function_t;
static std::vector<std::reference_wrapper<FirebaseData>> fbso;
static uint8_t fbdoIdx __attribute__((used)) = 0;

class FCMObject
{
public:
  FCMObject();
  ~FCMObject();

  /** Store Firebase Cloud Messaging's authentication credentials.
   * 
   * @param serverKey Server key found on Console: Project settings > Cloud Messaging
   */
  void begin(const String &serverKey);

  /** Add recipient's device registration token or instant ID token.
   * 
   * @param deviceToken Recipient's device registration token to add that message will be sent to.
   */
  void addDeviceToken(const String &deviceToken);

  /** Remove the recipient's device registration token or instant ID token.
   * 
   * @param index Index (start from zero) of the recipient's device registration token that added to FCM Data Object of Firebase Data object.
   */
  void removeDeviceToken(uint16_t index);

  /** Clear all recipient's device registration tokens.
  */
  void clearDeviceToken();

  /** Set the notify message type information.
   * 
   * @param title The title text of notification message.
   * @param body The body text of notification message.
   */
  void setNotifyMessage(const String &title, const String &body);

  /** Set the notify message type information.
   * 
   * @param title The title text of notification message.
   * @param body The body text of notification message.
   * @param icon The name and/or included URI/URL of the icon to show on notifying message.
   */
  void setNotifyMessage(const String &title, const String &body, const String &icon);

  /** Set the notify message type information.
   * 
   * @param title The title text of notification message.
   * @param body The body text of notification message.
   * @param icon The name and/or included URI/URL of the icon to show on notifying message.
   * @param click_action The URL or intent to accept click event on the notification message.
   */
  void setNotifyMessage(const String &title, const String &body, const String &icon, const String &click_action);

  /** add the custom key/value in the notify message type information.
   * 
   * @param key The key field in notification message.
   * @param value The value field in the notification message.
  */
  void addCustomNotifyMessage(const String &key, const String &value);

  /** Clear all notify message information.
  */
  void clearNotifyMessage();

  /** Set the custom data message type information.
   * 
   * @param jsonString The JSON structured data string.
  */
  void setDataMessage(const String &jsonString);

  /** Set the custom data message type information.
   * 
   * @param json The FirebaseJson object.
  */
  void setDataMessage(FirebaseJson &json);

  /** Clear custom data message type information.
  */
  void clearDataMessage();

  /** Set the priority of the message (notification and custom data).
   * 
   * @param priority The priority string i.e. normal and high.
  */
  void setPriority(const String &priority);

  /** Set the collapse key of the message (notification and custom data).
   * 
   * @param key String of collapse key.
  */
  void setCollapseKey(const String &key);

  /** Set the Time To Live of the message (notification and custom data).
   * 
   * @param seconds Number of seconds from 0 to 2,419,200 (4 weeks).
  */
  void setTimeToLive(uint32_t seconds);

  /** Set the topic of the message will be sent to.
   * 
   * @param topic Topic string.
  */
  void setTopic(const String &topic);

  /** Get the send result.
   * 
   * @return String of payload returned from the server.
  */
  String getSendResult();

private:
  void fcm_begin(FirebaseData &fbdo);

  bool fcm_send(FirebaseData &fbdo, fb_esp_fcm_msg_type messageType);

  void fcm_prepareHeader(std::string &header, size_t payloadSize);

  void fcm_preparePayload(std::string &msg, fb_esp_fcm_msg_type messageType);

  void clear();
  std::string _topic = "";
  std::string _server_key = "";
  std::string _sendResult = "";
  FirebaseJson _fcmPayload;
  int _ttl = -1;
  uint16_t _index = 0;
  uint16_t _port = FIREBASE_PORT;
  std::vector<std::string> _deviceToken;

  friend class FirebaseESP8266;
  friend class FirebaseData;
};

class QueryFilter
{
public:
  QueryFilter();
  ~QueryFilter();
  QueryFilter &orderBy(const String &);
  QueryFilter &limitToFirst(int);
  QueryFilter &limitToLast(int);
  QueryFilter &startAt(float);
  QueryFilter &endAt(float);
  QueryFilter &startAt(const String &);
  QueryFilter &endAt(const String &);
  QueryFilter &equalTo(int);
  QueryFilter &equalTo(const String &);
  QueryFilter &clear();
  friend FirebaseESP8266;
  friend FirebaseData;

protected:
  std::string _orderBy = "";
  std::string _limitToFirst = "";
  std::string _limitToLast = "";
  std::string _startAt = "";
  std::string _endAt = "";
  std::string _equalTo = "";
};

class QueueInfo
{
public:
  QueueInfo();
  ~QueueInfo();
  uint8_t totalQueues();
  uint32_t currentQueueID();
  bool isQueueFull();
  String dataType();
  String firebaseMethod();
  String dataPath();

private:
  void clear();
  uint8_t _totalQueue = 0;
  uint32_t _currentQueueID = 0;
  bool _isQueueFull = false;
  bool _isQueue = false;
  std::string _dataType = "";
  std::string _method = "";
  std::string _path = "";

  friend class FirebaseESP8266;
};

struct QueueItem
{
  fb_esp_data_type dataType = fb_esp_data_type::d_any;
  fb_esp_method method = fb_esp_method::m_put;
  uint32_t qID = 0;
  uint32_t timestamp = 0;
  uint8_t runCount = 0;
  uint8_t runIndex = 0;
  uint8_t storageType = 0;

  std::string path = "";
  std::string payload = "";
  std::vector<uint8_t> blob = std::vector<uint8_t>();
  std::string filename = "";
  QueryFilter queryFilter;
  int *intPtr = nullptr;
  float *floatPtr = nullptr;
  double *doublePtr = nullptr;
  bool *boolPtr = nullptr;
  String *stringPtr = nullptr;
  FirebaseJson *jsonPtr = nullptr;
  FirebaseJsonArray *arrPtr = nullptr;
  std::vector<uint8_t> *blobPtr = nullptr;
};

class QueueManager
{
public:
  QueueManager();
  ~QueueManager();

  bool add(QueueItem q);
  void remove(uint8_t index);

  friend class FirebaseESP8266;
  friend class FirebaseData;

private:
  void clear();
  std::vector<QueueItem> _queueCollection = std::vector<QueueItem>();
  uint8_t _maxQueue = 10;
};

class FirebaseESP8266
{
public:
  friend class StreamData;
  friend class FirebaseData;
  friend class FCMObject;
  friend class QueryFilter;
  friend class MultiPathStreamData;

  typedef void (*StreamEventCallback)(StreamData);
  typedef void (*MultiPathStreamEventCallback)(MultiPathStreamData);

  typedef void (*StreamTimeoutCallback)(bool);
  typedef void (*QueueInfoCallback)(QueueInfo);

  FirebaseESP8266();
  ~FirebaseESP8266();

  /** Store Firebase's authentication credentials.
   * 
   * @param host Your Firebase database project host e.g. Your_ProjectID.firebaseio.com.
   * @param auth Your database secret.
   * @param caCert Root CA certificate base64 string (PEM file).
   * @param caCertFile Root CA certificate DER file (binary).
   * @param StorageType Type of storage, StorageType::SD and StorageType::FLASH.
   * @param GMTOffset GMT time offset in hour is required to set time in order to make BearSSL 
   * data decryption/encryption to work. 
   * 
   * @note This parameter is only required for ESP8266 Core SDK v2.5.x or later. 
   * The Root CA certificate DER file is only supported in Core SDK v2.5.x
  */
  void begin(const String &host, const String &auth);

  void begin(const String &host, const String &auth, const char *caCert, float GMTOffset = 0.0);

  void begin(const String &host, const String &auth, const String &caCertFile, uint8_t storageType, float GMTOffset = 0.0);

  /** Stop Firebase and release all resources.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
  */
  void end(FirebaseData &fbdo);

  /** Reconnect WiFi if lost connection.
   * 
   * @param reconnect The boolean to set/unset WiFi AP reconnection.
  */
  void reconnectWiFi(bool reconnect);

  /** Set the decimal places for float value to be stored in database.
   * 
   * @param digits The decimal places. 
  */
  void setFloatDigits(uint8_t digits);

  /** Set the decimal places for double value to be stored in database.
   * 
   * @param digits The decimal places. 
  */
  void setDoubleDigits(uint8_t digits);

  /** Set the timeout of Firebase.get functions.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param millisec The milliseconds to limit the request (0 900,000 ms or 15 min).
  */
  void setReadTimeout(FirebaseData &fbdo, int millisec);

  /** Set the size limit of payload data that will write to the database for each request.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param size The size identified string e.g. tiny, small, medium, large and unlimited. 
   * 
   * @note Size string and its write timeout in seconds e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  */
  void setwriteSizeLimit(FirebaseData &fbdo, const String &size);

  /** Read the database rules.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @return Boolean type status indicates the success of the operation. 
   * 
   * @note Call [FirebaseData object].jsonData will return the JSON string value of 
   * database rules returned from the server.
  */
  bool getRules(FirebaseData &fbdo);

  /** Write the database rules.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param rules Database rules in JSON String format.
   * @return Boolean type status indicates the success of the operation.
  */
  bool setRules(FirebaseData &fbdo, const String &rules);

  /** Determine whether the defined database path exists or not.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path to be checked.
   * @return Boolean type result indicates whether the defined path existed or not.
   */
  bool pathExist(FirebaseData &fbdo, const String &path);

  /** Determine the unique identifier (ETag) of current data at the defined database path.
   * 
   * @return String of unique identifier.
  */
  String getETag(FirebaseData &fbdo, const String &path);

  /** Get the shallowed data at a defined node path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path being read the data.
   * @return Boolean type status indicates the success of the operation. 
   * 
   * @note Return the child data with its value or JSON object (its values will be truncated to true). 
   * Call [FirebaseData object].stringData() to get shallowed string data (number, string and JSON object).
  */
  bool getShallowData(FirebaseData &fbdo, const String &path);

  /** Enable the library to use only classic HTTP GET and POST methods.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param flag Boolean value to enable.
   * 
   * @note This option used to escape the Firewall restriction (if the device is connected through 
   * Firewall) that allows only HTTP GET and POST
   * HTTP PATCH request was sent as PATCH which not affected by this option.
  */
  void enableClassicRequest(FirebaseData &fbdo, bool flag);

  /** Set the virtual child node ".priority" to the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which to set the priority value.
   * @param priority The priority value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note This allows us to set priority to any node other than a priority that set through setJSON, 
   * pushJSON, updateNode, and updateNodeSilent functions. 
   * The returned priority value from server can read from function [FirebaseData object].priority().
   */
  bool setPriority(FirebaseData &fbdo, const String &path, float priority);

  /** Read the virtual child node ".priority" value at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which to set the priority value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The priority value from server can read from function [FirebaseData object].priority().
   */
  bool getPriority(FirebaseData &fbdo, const String &path);

  /** Append new integer value to the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which integer value will be appended.
   * @param intValue The appended value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The new appended node's key will be stored in Firebase Data object, 
   * which its value can be accessed via function [FirebaseData object].pushName().
  */
  bool pushInt(FirebaseData &fbdo, const String &path, int intValue);

  bool push(FirebaseData &fbdo, const String &path, int intValue);

  /** Append new integer value and the virtual child ".priority" to the defined database path.
  */
  bool pushInt(FirebaseData &fbdo, const String &path, int intValue, float priority);

  bool push(FirebaseData &fbdo, const String &path, int intValue, float priority);

  /** Append new float value to the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path in which float value will be appended.
   * @param floatValue The appended value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The new appended node's key will be stored in Firebase Data object, 
   * which its value can be accessed via function [FirebaseData object].pushName().
  */
  bool pushFloat(FirebaseData &fbdo, const String &path, float floatValue);

  bool push(FirebaseData &fbdo, const String &path, float floatValue);

  /** Append new float value and the virtual child ".priority" to the defined database path.
  */
  bool pushFloat(FirebaseData &fbdo, const String &path, float floatValue, float priority);

  bool push(FirebaseData &fbdo, const String &path, float floatValue, float priority);

  /** Append new double value (8 bytes) to the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path in which float value will be appended.
   * @param doubleValue The appended value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The new appended node's key will be stored in Firebase Data object, 
   * which its value can be accessed via function [FirebaseData object].pushName().
  */
  bool pushDouble(FirebaseData &fbdo, const String &path, double doubleValue);

  bool push(FirebaseData &fbdo, const String &path, double doubleValue);

  /** Append new double value (8 bytes) and the virtual child ".priority" to the defined database path.
  */
  bool pushDouble(FirebaseData &fbdo, const String &path, double doubleValue, float priority);

  bool push(FirebaseData &fbdo, const String &path, double doubleValue, float priority);

  /** Append new Boolean value to the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which Boolean value will be appended.
   * @param boolValue The appended value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The new appended node's key will be stored in Firebase Data object, 
   * which its value can be accessed via function [FirebaseData object].pushName().

  */
  bool pushBool(FirebaseData &fbdo, const String &path, bool boolValue);

  bool push(FirebaseData &fbdo, const String &path, bool boolValue);

  /** Append the new Boolean value and the virtual child ".priority" to the defined database path.
  */
  bool pushBool(FirebaseData &fbdo, const String &path, bool boolValue, float priority);

  bool push(FirebaseData &fbdo, const String &path, bool boolValue, float priority);

  /** Append a new string (text) to the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which string will be appended.
   * @param StringValue The appended value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The new appended node's key stored in Firebase Data object, 
   * which can be accessed via function [FirebaseData object].pushName().
  */
  bool pushString(FirebaseData &fbdo, const String &path, const String &stringValue);

  bool push(FirebaseData &fbdo, const String &path, const char *stringValue);

  bool push(FirebaseData &fbdo, const String &path, const String &stringValue);

  bool push(FirebaseData &fbdo, const String &path, const StringSumHelper &stringValue);

  /** Append new string (text) and the virtual child ".priority" to the defined database path.
  */
  bool pushString(FirebaseData &fbdo, const String &path, const String &stringValue, float priority);

  bool push(FirebaseData &fbdo, const String &path, const char *stringValue, float priority);

  bool push(FirebaseData &fbdo, const String &path, const String &stringValue, float priority);

  bool push(FirebaseData &fbdo, const String &path, const StringSumHelper &stringValue, float priority);

  /** Append new child nodes key and value (using FirebaseJson object) to the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which key and value in FirebaseJson object will be appended.
   * @param json The appended FirebaseJson object.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The new appended node key will be stored in Firebase Data object, 
   * which its value can be accessed via function [FirebaseData object].pushName().
  */
  bool pushJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json);

  bool push(FirebaseData &fbdo, const String &path, FirebaseJson &json);

  /** Append new child node key and value (FirebaseJson object) and the virtual child ".priority" to the defined database path.
  */
  bool pushJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority);

  bool push(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority);

  /** Append child node array (using FirebaseJsonArray object) to the defined database path. 
   * This will replace any child nodes inside the defined path with array defined in FirebaseJsonArray object.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which key and value in FirebaseJsonArray object will be appended.
   * @param arr The appended FirebaseJsonArray object.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The new appended node's key will be stored in Firebase Data object, 
   * which its value can be accessed via function [FirebaseData object].pushName().
  */
  bool pushArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr);

  bool push(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr);

  /** Append FirebaseJsonArray object and virtual child ".priority" at the defined database path.
  */

  bool pushArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority);

  bool push(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority);

  /** Append new blob (binary data) to the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which binary data will be appended.
   * @param blob Byte array of data.
   * @param size Size of the byte array.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The new appended node's key will be stored in Firebase Data object, 
   * which its value can be accessed via function [FirebaseData object].pushName().
  */
  bool pushBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size);

  bool push(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size);

  /** Append new blob (binary data) and the virtual child ".priority" to the defined database path.
  */
  bool pushBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority);

  bool push(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority);

  /** Append new binary data from the file stores on SD card/Flash memory to the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param storageType Type of storage to read file data, StorageType::SPIFS or StorageType::SD.
   * @param path Target database path in which binary data from the file will be appended.
   * @param fileName File name included its path in SD card/Flash memory.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The new appended node's key will be stored in Firebase Data object, 
   * which its value can be accessed via function [FirebaseData object].pushName().
  */
  bool pushFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName);

  bool push(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName);

  /** Append new binary data from the file stores on SD card/Flash memory and the virtual child ".priority" 
   * to the defined database path.
  */
  bool pushFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority);

  bool push(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority);

  /** Append the new Firebase server's timestamp to the defined database path.*
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which timestamp will be appended.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The new appended node's key will be stored in Firebase Data object, 
   * which its value can be accessed via function [FirebaseData object].pushName().
   */
  bool pushTimestamp(FirebaseData &fbdo, const String &path);

  /** Set integer data at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which integer data will be set.
   * @param intValue Integer value to set.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data that successfully stores in the database. 
   * Call [FirebaseData object].intData will return the integer value of 
   * payload returned from server.
  */
  bool setInt(FirebaseData &fbdo, const String &path, int intValue);

  bool set(FirebaseData &fbdo, const String &path, int intValue);

  /** Set integer data and virtual child ".priority" at the defined database path.
  */
  bool setInt(FirebaseData &fbdo, const String &path, int intValue, float priority);

  bool set(FirebaseData &fbdo, const String &path, int intValue, float priority);

  /** Set integer data at the defined database path if defined database path's ETag matched the ETag value.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which integer data will be set.
   * @param intValue Integer value to set.
   * @param ETag Known unique identifier string (ETag) of defined database path.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * If ETag at the defined database path does not match the provided ETag parameter,
   * the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched). 
   * 
   * If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value. 
   * Also call [FirebaseData object].intData to get the current integer value.
  */
  bool setInt(FirebaseData &fbdo, const String &path, int intValue, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, int intValue, const String &ETag);

  /** Set integer data and the virtual child ".priority" if defined ETag matches at the defined database path 
  */
  bool setInt(FirebaseData &fbdo, const String &path, int intValue, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, int intValue, float priority, const String &ETag);

  /** Set float data at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path in which float data will be set.
   * @param floatValue Float value to set.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * Call [FirebaseData object].floatData will return the float value of 
   * payload returned from server.
  */
  bool setFloat(FirebaseData &fbdo, const String &path, float floatValue);

  bool set(FirebaseData &fbdo, const String &path, float floatValue);

  /** Set float data and virtual child ".priority" at the defined database path.
  */
  bool setFloat(FirebaseData &fbdo, const String &path, float floatValue, float priority);

  bool set(FirebaseData &fbdo, const String &path, float floatValue, float priority);

  /** Set float data at the defined database path if defined database path's ETag matched the ETag value.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path in which float data will be set.
   * @param floatValue Float value to set.
   * @param ETag Known unique identifier string (ETag) of defined database path.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * Call [FirebaseData object].floatData will return the float value of payload returned from server. 
   * 
   * If ETag at the defined database path does not match the provided ETag parameter, 
   * the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).
   * 
   * If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value. 
   * Also call [FirebaseData object].floatData to get the current float value.
   */
  bool setFloat(FirebaseData &fbdo, const String &path, float floatValue, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, float floatValue, const String &ETag);

  /** Set float data and the virtual child ".priority" if defined ETag matches at the defined database path 
  */
  bool setFloat(FirebaseData &fbdo, const String &path, float floatValue, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, float floatValue, float priority, const String &ETag);

  /** Set double data at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path in which float data will be set.
   * @param doubleValue Double value to set.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * Call [FirebaseData object].doubleData will return the double value of the payload returned from the server. 
   * 
   * Due to bugs in Serial.print in Arduino, to print large double value with zero decimal place, 
   * use printf("%.9lf\n", firebaseData.doubleData()); for print the returned double value up to 9 decimal places.
  */
  bool setDouble(FirebaseData &fbdo, const String &path, double doubleValue);

  bool set(FirebaseData &fbdo, const String &path, double doubleValue);

  /** Set double data and virtual child ".priority" at the defined database path.
  */
  bool setDouble(FirebaseData &fbdo, const String &path, double doubleValue, float priority);

  bool set(FirebaseData &fbdo, const String &path, double doubleValue, float priority);

  /** Set double data at the defined database path if defined database path's ETag matched the ETag value.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path in which float data will be set.
   * @param doubleValue Double value to set.
   * @param ETag Known unique identifier string (ETag) of defined database path.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * Call [FirebaseData object].doubleData will return the double value of the payload returned from the server. 
   * 
   * If ETag at the defined database path does not match the provided ETag parameter, 
   * the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched). 
   * 
   * If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value. 
   * Also call [FirebaseData object].doubeData to get the current double value.
  */
  bool setDouble(FirebaseData &fbdo, const String &path, double doubleValue, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, double doubleValue, const String &ETag);

  /** Set double data and the virtual child ".priority" if defined ETag matches at the defined database path 
  */
  bool setDouble(FirebaseData &fbdo, const String &path, double doubleValue, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, double doubleValue, float priority, const String &ETag);

  /** Set Boolean data at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which Boolean data will be set.
   * @param boolValue Boolean value to set.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * Call [FirebaseData object].boolData will return the Boolean value of 
   * the payload returned from the server.
  */
  bool setBool(FirebaseData &fbdo, const String &path, bool boolValue);

  bool set(FirebaseData &fbdo, const String &path, bool boolValue);

  /** Set boolean data and virtual child ".priority" at the defined database path.
  */
  bool setBool(FirebaseData &fbdo, const String &path, bool boolValue, float priority);

  bool set(FirebaseData &fbdo, const String &path, bool boolValue, float priority);

  /** Set Boolean data at the defined database path if defined database path's ETag matched the ETag value.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which Boolean data will be set.
   * @param boolValue Boolean value to set.
   * @param ETag Known unique identifier string (ETag) of defined database path.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * Call [FirebaseData object].boolData will return the Boolean value of 
   * the payload returned from the server. 
   * 
   * If ETag at the defined database path does not match the provided ETag parameter, 
   * the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched). 
   * 
   * If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value. 
   * Also call [FirebaseData object].doubeData to get the current boolean value.
  */
  bool setBool(FirebaseData &fbdo, const String &path, bool boolValue, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, bool boolValue, const String &ETag);

  /** Set boolean data and the virtual child ".priority" if defined ETag matches at the defined database path 
  */
  bool setBool(FirebaseData &fbdo, const String &path, bool boolValue, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, bool boolValue, float priority, const String &ETag);

  /** Set string (text) at the defined database path. 
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path in which string data will be set.
   * @param stringValue String or text to set.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data that successfully 
   * stores in the database.
   * 
   * Call [FirebaseData object].stringData will return the string value of 
   * the payload returned from the server.
  */
  bool setString(FirebaseData &fbdo, const String &path, const String &stringValue);

  bool set(FirebaseData &fbdo, const String &path, const char *stringValue);

  bool set(FirebaseData &fbdo, const String &path, const String &stringValue);

  bool set(FirebaseData &fbdo, const String &path, const StringSumHelper &stringValue);

  /** Set string data and virtual child ".priority" at the defined database path.
  */
  bool setString(FirebaseData &fbdo, const String &path, const String &stringValue, float priority);

  bool set(FirebaseData &fbdo, const String &path, const char *stringValue, float priority);

  bool set(FirebaseData &fbdo, const String &path, const String &stringValue, float priority);

  bool set(FirebaseData &fbdo, const String &path, const StringSumHelper &stringValue, float priority);

  /** Set string (text) at the defined database path if defined database path's ETag matched the ETag value.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path in which string data will be set.
   * @param stringValue String or text to set.
   * @param ETag Known unique identifier string (ETag) of defined database path.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * 
   * Call [FirebaseData object].stringData will return the string value of 
   * the payload returned from the server. 
   * 
   * If ETag at the defined database path does not match the provided ETag parameter, 
   * the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched). 
   * 
   * If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value.
   * Also, call [FirebaseData object].stringData to get the current string value.
   */
  bool setString(FirebaseData &fbdo, const String &path, const String &stringValue, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, const char *stringValue, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, const String &stringValue, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, const StringSumHelper &stringValue, const String &ETag);

  /** Set string data and the virtual child ".priority" if defined ETag matches at the defined database path 
  */
  bool setString(FirebaseData &fbdo, const String &path, const String &stringValue, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, const char *stringValue, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, const String &stringValue, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, const StringSumHelper &stringValue, float priority, const String &ETag);

  /** Set the child node key and value (using FirebaseJson object) to the defined database path. 
   * This will replace any child nodes inside the defined path with node' s key 
   * and value defined in FirebaseJson object.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which key and value in FirebaseJson object will be replaced or set.
   * @param json The FirebaseJson object.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * 
   * Call [FirebaseData object].jsonData will return the JSON string value of payload returned from server.
  */
  bool setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json);

  bool set(FirebaseData &fbdo, const String &path, FirebaseJson &json);

  /** Set JSON data or FirebaseJson object and virtual child ".priority" at the defined database path.
  */

  bool setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority);

  bool set(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority);

  /** Set child node key and value (using JSON data or FirebaseJson object) to the defined database path 
   * if defined database path's ETag matched the ETag value. 
   * This will replace any child nodes inside the defined path with node' s key 
   * and value defined in JSON data or FirebaseJson object.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which key and value in JSON data will be replaced or set.
   * @param json The FirebaseJson object.
   * @param ETag Known unique identifier string (ETag) of defined database path.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * 
   * Call [FirebaseData object].jsonData will return the JSON string value of 
   * payload returned from server. 
   * 
   * If ETag at the defined database path does not match the provided ETag parameter, 
   * the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched). 
   * If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value. 
   * Also call [FirebaseData object].jsonData to get the current JSON string value.
  */
  bool setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, FirebaseJson &json, const String &ETag);

  /** Set JSON data or FirebaseJson object and the virtual child ".priority" if defined ETag matches at the defined database path 
  */

  bool setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority, const String &ETag);

  /** Set child node array (using FirebaseJsonArray object) to the defined database path. 
   * This will replace any child nodes inside the defined path with array defined in FirebaseJsonArray object.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which key and value in FirebaseJsonArray object will be replaced or set.
   * @param arr The FirebaseJsonArray object.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data that successfully stores in the database. 
   * 
   * Call [FirebaseData object].jsonArray will return pointer to FirebaseJsonArray object contains array 
   * payload returned from server, get the array payload using FirebaseJsonArray *arr = firebaseData.jsonArray();
  */
  bool setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr);

  bool set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr);

  /** Set FirebaseJsonArray object and virtual child ".priority" at the defined database path.
  */
  bool setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority);

  bool set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority);

  /** Set array (using JSON data or FirebaseJson object) to the defined database path if defined database path's ETag matched the ETag value. 
   * This will replace any child nodes inside the defined path with array defined in FirebaseJsonArray object.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which key and value in JSON data will be replaced or set.
   * @param arr The FirebaseJsonArray object.
   * @param ETag Known unique identifier string (ETag) of defined database path.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * 
   * Call [FirebaseData object].jsonArray will return pointer to FirebaseJsonArray object contains array 
   * payload returned from server, get the array payload using FirebaseJsonArray *arr = firebaseData.jsonArray(); 
   * 
   * If ETag at the defined database path does not match the provided ETag parameter, 
   * the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched). 
   * 
   * If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value. 
   * Also call [FirebaseData object].jsonArray to get the pointer to FirebaseJsonArray object of current array value.
  */
  bool setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, const String &ETag);

  /** Set FirebaseJsonArray object and the virtual child ".priority" if defined ETag matches at the defined database path 
  */
  bool setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority, const String &ETag);

  /** Set blob (binary data) at the defined database path. 
   * This will replace any child nodes inside the defined path with a blob or binary data.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which binary data will be set.
   * @param blob Byte array of data.
   * @param size Size of the byte array.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note No payload returned from the server.
  */
  bool setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size);

  bool set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size);

  /** Set blob data and virtual child ".priority" at the defined database path.
  */
  bool setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority);

  bool set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority);

  /** Set blob (binary data) at the defined database path if defined database path's ETag matched the ETag value. 
   * This will replace any child nodes inside the defined path with a blob or binary data.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which binary data will be set.
   * @param blob Byte array of data.
   * @param size Size of the byte array.
   * @param ETag Known unique identifier string (ETag) of defined database path.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note No payload returned from the server. 
   * If ETag at the defined database path does not match the provided ETag parameter, 
   * the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).
  */
  bool setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, const String &ETag);

  /** Set blob data and the virtual child ".priority" if defined ETag matches at the defined database path 
  */
  bool setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority, const String &ETag);

  /** Set binary data from the file store on SD card/Flash memory to the defined database path. 
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param storageType Type of storage to read file data, StorageType::SPIFS or StorageType::SD.
   * @param path Target database path in which binary data from the file will be set.
   * @param fileName File name included its path in SD card/Flash memory
   * @return Boolean type status indicates the success of the operation. 
   * 
   * @note No payload returned from the server.
  */
  bool setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName);

  bool set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName);

  /** Set binary data from the file and virtual child ".priority" at the defined database path.
  */
  bool setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority);

  bool set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority);

  /** Set binary data from file stored on SD card/Flash memory to the defined database path if defined database path's ETag matched the ETag value.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param storageType Type of storage to read file data, StorageType::SPIFS or StorageType::SD.
   * @param path Target database path in which binary data from the file will be set.
   * @param fileName File name included its path in SD card/Flash memory.
   * @param ETag Known unique identifier string (ETag) of defined database path.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note No payload returned from the server. 
   * 
   * If ETag at the defined database path does not match the provided ETag parameter, 
   * the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).
  */
  bool setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, const String &ETag);

  bool set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, const String &ETag);

  /** Set binary data from the file and the virtual child ".priority" if defined ETag matches at the defined database path 
  */
  bool setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority, const String &ETag);

  /** Set Firebase server's timestamp to the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which timestamp will be set.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].intData will return the integer value of timestamp in seconds 
   * or [FirebaseData object].doubleData to get millisecond timestamp. 
   * 
   * Due to bugs in Serial.print in Arduino, to print large double value with zero decimal place, 
   * use printf("%.0lf\n", firebaseData.doubleData());.
  */
  bool setTimestamp(FirebaseData &fbdo, const String &path);

  /** Update the child node key or existing key's value (using FirebaseJson object) under the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which key and value in FirebaseJson object will be updated.
   * @param json The FirebaseJson object used for the update.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * 
   * Call [FirebaseData object].jsonData will return the json string value of payload returned from server. 
   * To reduce network data usage, use updateNodeSilent instead.
  */
  bool updateNode(FirebaseData &fbdo, const String path, FirebaseJson &json);

  /** Update child node key or existing key's value and virtual child ".priority" (using JSON data or FirebaseJson object) under the defined database path.
  */
  bool updateNode(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority);

  /** Update the child node key or existing key's value (using FirebaseJson object) under the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which key and value in FirebaseJson object will be updated.
   * @param json The FirebaseJson object used for the update.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Owing to the objective of this function to reduce network data usage, 
   * no payload will be returned from the server.
  */
  bool updateNodeSilent(FirebaseData &fbdo, const String &path, FirebaseJson &json);

  /** Update child node key or existing key's value and virtual child ".priority" (using JSON data or FirebaseJson object) under the defined database path.
  */

  bool updateNodeSilent(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority);

  /** Read any type of value at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the float value is being read.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * 
   * Call [FirebaseData object].intData, [FirebaseData object].floatData, [FirebaseData object].doubleData, 
   * [FirebaseData object].boolData, [FirebaseData object].stringData, [FirebaseData object].jsonObject (pointer), 
   * [FirebaseData object].jsonArray (pointer) and [FirebaseData object].blobData corresponded to 
   * its type from [FirebaseData object].dataType.
  */
  bool get(FirebaseData &fbdo, const String &path);

  /** Read the integer value at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the float value is being read.

    @return Boolean type status indicates the success of the operation.

    Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
    
    Call [FirebaseData object].intData will return the integer value of
    payload returned from server.

    If the type of payload returned from server is not integer, float and double, 
    the function [FirebaseData object].intData will return zero (0).

  */
  bool getInt(FirebaseData &fbdo, const String &path);

  /** Read the integer value at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the float value is being read.
   * @param target The integer type variable to store value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note If the type of payload returned from the server is not an integer, float and double, 
   * the target variable's value will be zero (0).
  */
  bool getInt(FirebaseData &fbdo, const String &path, int &target);

  /** Read the float value at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the float value is being read.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database.
   * 
   * Call [FirebaseData object].floatData will return the float value of 
   * payload returned from server. 
   * 
   * If the payload returned from server is not integer, float and double, 
   * the function [FirebaseData object].floatData will return zero (0).
  */
  bool getFloat(FirebaseData &fbdo, const String &path);

  /** Read the float value at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the float value is being read.
   * @param target The float type variable to store value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note If the type of payload returned from the server is not an integer, float and double, 
   * the target variable's value will be zero (0).
  */
  bool getFloat(FirebaseData &fbdo, const String &path, float &target);

  /** Read the double value at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the float value is being read.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * 
   * Call [FirebaseData object].doubleData will return the double value of
   * the payload returned from the server.
   * 
   * If the payload returned from server is not integer, float and double, 
   * the function [FirebaseData object].doubleData will return zero (0).
   * 
   * Due to bugs in Serial.print in Arduino, to print large double value with zero decimal place, 
   * use printf("%.9lf\n", firebaseData.doubleData()); for print value up to 9 decimal places.
  */
  bool getDouble(FirebaseData &fbdo, const String &path);

  /** Read the float value at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the float value is being read.
   * @param target The double type variable to store value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note If the type of payload returned from the server is not an integer, float and double, 
   * the target variable's value will be zero (0).
  */
  bool getDouble(FirebaseData &fbdo, const String &path, double &target);

  /** Read the Boolean value at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the Boolean value is being read.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * 
   * Call [FirebaseData object].boolData will return the Boolean value of
   * the payload returned from the server.
   * 
   * If the type of payload returned from the server is not Boolean, 
   * the function [FirebaseData object].boolData will return false.
  */
  bool getBool(FirebaseData &fbdo, const String &path);

  /** Read the Boolean value at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the Boolean value is being read.
   * @param target The boolean type variable to store value.
   *  @return Boolean type status indicates the success of the operation.
   * 
   * @note If the type of payload returned from the server is not Boolean, 
   * the target variable's value will be false.
  */
  bool getBool(FirebaseData &fbdo, const String &path, bool &target);

  /** Read the string at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the string value is being read.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data that successfully
   * stores in the database.
   * 
   * Call [FirebaseData object].stringData will return the string value of
   * the payload returned from the server.
   * 
   * If the type of payload returned from the server is not a string,
   * the function [FirebaseData object].stringData will return empty string (String object).
  */
  bool getString(FirebaseData &fbdo, const String &path);

  /** Read the string at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the string value is being read.
   * @param target The String object to store value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note If the type of payload returned from the server is not a string,
   * the target String object's value will be empty.
  */
  bool getString(FirebaseData &fbdo, const String &path, String &target);

  /** Read the JSON string at the defined database path. 
   * The returned payload JSON string represents the child nodes and their value.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the JSON string value is being read.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data that successfully stores in the database.
   * 
   * Call [FirebaseData object].jsonObject will return the pointer to FirebaseJson object contains the value of
   * the payload returned from the server.
   * 
   * If the type of payload returned from server is not json,
   * the function [FirebaseData object].jsonObject will contain empty object.
  */
  bool getJSON(FirebaseData &fbdo, const String &path);

  /** Read the JSON string at the defined database path. 
   * The returned the pointer to FirebaseJson that contains JSON payload represents the child nodes and their value.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the JSON string value is being read.
   * @param target The FirebaseJson object pointer to get JSON data.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note If the type of payload returned from the server is not JSON,
   * the target FirebaseJson object will contain an empty object.
  */
  bool getJSON(FirebaseData &fbdo, const String &path, FirebaseJson *target);

  /** Read the JSON string at the defined database path. 
   * The returned the pointer to FirebaseJson that contains JSON payload represents the child nodes and their value.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the JSON string value is being read.
   * @param query QueryFilter class to set query parameters to filter data.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Available query parameters for filtering the data are the following.
   * 
   * QueryFilter.orderBy       Required parameter to specify which data used for data filtering included child key, key, and value.
   *                           Use "$key" for filtering data by keys of all nodes at the defined database path.
   *                           Use "$value" for filtering data by value of all nodes at the defined database path.
   *                           Use "$priority" for filtering data by "virtual child" named .priority of all nodes.
   *                           Use any child key to filter by that key.
   * 
   * QueryFilter.limitToFirst  The total children (number) to filter from the first child.
   * QueryFilter.limitToLast   The total last children (number) to filter.
   * QueryFilter.startAt       Starting value of range (number or string) of query upon orderBy param.
   * QueryFilter.endAt         Ending value of range (number or string) of query upon orderBy param.
   * QueryFilter.equalTo       Value (number or string) matches the orderBy param
   * 
   * Call [FirebaseData object].dataType to determine what type of data successfully stores in the database.
   * 
   * Call [FirebaseData object].jsonObject will return the pointer to FirebaseJson object contains the value of
   * the payload returned from the server.
   * 
   * If the type of payload returned from server is not json,
   * the function [FirebaseData object].jsonObject will contain empty object.
  */
  bool getJSON(FirebaseData &fbdo, const String &path, QueryFilter &query);

  /** Read the JSON string at the defined database path as above
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the JSON string value is being read.
   * @param target The FirebaseJson object pointer to get JSON data.
   * @return Boolean type status indicates the success of the operation.
   * 
   * If the type of payload returned from the server is not JSON,
   * the target FirebaseJson object will contain an empty object.
  */
  bool getJSON(FirebaseData &fbdo, const String &path, QueryFilter &query, FirebaseJson *target);

  /** Read the array data at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the array is being read.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data that successfully
   * stores in the database.
   * 
   * Call [FirebaseData object].jsonArray will return the pointer to FirebaseJsonArray object contains array value of
   * payload returned from server.
   * 
   * If the type of payload returned from the server is not an array,
   * the array element in [FirebaseData object].jsonArray will be empty.
  */
  bool getArray(FirebaseData &fbdo, const String &path);

  /** Read the array data at the defined database path, and assign data to the target.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the array is being read.
   * @param target The FirebaseJsonArray object pointer to get array value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note If the type of payload returned from the server is not an array, 
   * the target FirebaseJsonArray object will contain an empty array.
  */
  bool getArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray *target);

  /** Read the array data at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the array is being read.
   * @param query QueryFilter class to set query parameters to filter data.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Available query parameters for filtering the data are the following.
   * 
   * QueryFilter.orderBy       Required parameter to specify which data used for data filtering included child key, key, and value.
   *                           Use "$key" for filtering data by keys of all nodes at the defined database path.
   *                           Use "$value" for filtering data by value of all nodes at the defined database path.
   *                           Use "$priority" for filtering data by "virtual child" named .priority of all nodes.
   *                           Use any child key to filter by that key.
   *                     
   * QueryFilter.limitToFirst  The total children (number) to filter from the first child.
   * QueryFilter.limitToLast   The total last children (number) to filter.
   * QueryFilter.startAt       Starting value of range (number or string) of query upon orderBy param.
   * QueryFilter.endAt         Ending value of range (number or string) of query upon orderBy param.
   * QueryFilter.equalTo       Value (number or string) matches the orderBy param
   * 
   * Call [FirebaseData object].dataType to determine what type of data that successfully 
   * stores in the database.
   * 
   * Call [FirebaseData object].jsonArray will return the pointer to FirebaseJsonArray object contains array of 
   * payload returned from server.
   * 
   * If the type of payload returned from the server is not an array,
   * the function [FirebaseData object].jsonArray will contain empty array.
  */
  bool getArray(FirebaseData &fbdo, const String &path, QueryFilter &query);

  /** Read the array data at the defined database path as above
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the array is being read.
   * @param target The FirebaseJsonArray object to get array value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note If the type of payload returned from the server is not an array,
   * the target FirebaseJsonArray object will contain an empty array.
  */
  bool getArray(FirebaseData &fbdo, const String &path, QueryFilter &query, FirebaseJsonArray *target);

  /** Read the blob (binary data) at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the binary data is being read.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database.
   * 
   * Call [FirebaseData object].blobData will return the dynamic array of unsigned 8-bit data (i.e. std::vector<uint8_t>) of
   * payload returned from server.
   * 
   * If the type of payload returned from the server is not a blob,
   * the function [FirebaseData object].blobData will return empty array.
  */
  bool getBlob(FirebaseData &fbdo, const String &path);

  /** Read the blob (binary data) at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the binary data is being read.
   * @param target Dynamic array of unsigned 8-bit data (i.e. std::vector<uint8_t>) to store value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note If the type of payload returned from the server is not a blob, 
   * the target variable value will be an empty array.
  */
  bool getBlob(FirebaseData &fbdo, const String &path, std::vector<uint8_t> &target);

  /** Download file data in a database at the defined database path and save it to SD card/Flash memory. 
   * The downloaded data will be decoded to binary and save to SD card/Flash memory, then please make sure that data at the defined database path is the file type.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param storageType Type of storage to write file data, StorageType::SPIFS or StorageType::SD.
   * @param nodePath Database path that file data will be downloaded.
   * @param fileName File name included its path in SD card/Flash memory to save in SD card/Flash memory.
   * @return Boolean type status indicates the success of the operation.
  */
  bool getFile(FirebaseData &fbdo, uint8_t storageType, const String &nodePath, const String &fileName);

  /** Delete all child nodes at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path to be deleted.
   * @return Boolean type status indicates the success of the operation.
  */
  bool deleteNode(FirebaseData &fbdo, const String &path);

  /** Delete all child nodes at the defined database path if defined database path's ETag matched the ETag value.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path to be deleted.
   * @param ETag Known unique identifier string (ETag) of defined database path.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note If ETag at the defined database path does not match the provided ETag parameter,
   * he operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).
  */
  bool deleteNode(FirebaseData &fbdo, const String &path, const String &ETag);

  /** Start subscribe to the value changes at the defined path and its children.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path to subscribe.
   * @return Boolean type status indicates the success of the operation.
  */
  bool beginStream(FirebaseData &fbdo, const String &path);

  /** Start subscribe to the value changes at the defined parent node path with multiple nodes paths parsing.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param parentPath Database parent node path to subscribe.
   * @param childPath The string array of child nodes paths for parsing.
   * @param size The size of string array of child nodes paths for parsing.
   * @return Boolean type status indicates the success of the operation.
  */
  bool beginMultiPathStream(FirebaseData &fbdo, const String &parentPath, const String *childPath, size_t size);

  /** Read the stream event data at the defined database path. 
   * Once beginStream was called e.g. in setup(), the readStream function 
   * should call inside the loop function.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Using the same Firebase Data object for stream read/monitoring associated 
   * with getXXX, setXXX, pushXXX, updateNode and deleteNode will break or quit
   * the current stream connection.
   * 
   * he stream will be resumed or reconnected automatically when calling readStream.
  */
  bool readStream(FirebaseData &fbdo);

  /** End the stream connection at a defined path. 
   * It can be restart again by calling beginStream.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @return Boolean type status indicates the success of the operation.
  */
  bool endStream(FirebaseData &fbdo);

  /** Set the stream callback functions.
   * setStreamCallback should be called before Firebase.beginStream.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param dataAvailablecallback a Callback function that accepts streamData parameter.
   * @param timeoutCallback Callback function will be called when the stream connection was timed out (optional).
   * 
   * @note dataAvailablecallback will be called When data in the defined path changed or the stream path changed or stream connection 
   * was resumed from getXXX, setXXX, pushXXX, updateNode, deleteNode.
   * 
   * The payload returned from the server will be one of these integer, float, string, JSON and blob types.
   * 
   * Call [streamData object].dataType to determine what type of data successfully stores in the database. 
   * 
   * Call [streamData object].xxxData will return the appropriate data type of 
   * the payload returned from the server.
  */
  void setStreamCallback(FirebaseData &fbdo, StreamEventCallback dataAvailablecallback, StreamTimeoutCallback timeoutCallback = NULL);

  /** Set the multiple paths stream callback functions. 
   * setMultiPathStreamCallback should be called before Firebase.beginMultiPathStream.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param multiPathDataCallback a Callback function that accepts MultiPathStreamData parameter.
   * @param timeoutCallback a Callback function will be called when the stream connection was timed out (optional).
   * 
   * @note multiPathDataCallback will be called When data in the defined path changed or the stream path changed or stream connection 
   * was resumed from getXXX, setXXX, pushXXX, updateNode, deleteNode.
   * 
   * The payload returned from the server will be one of these integer, float, string and JSON.
   * 
   * Call [MultiPathStreamData object].get to get the child node value, type and data path. 
   * 
   * The properties [MultiPathStreamData object].value, [MultiPathStreamData object].dataPath, and [MultiPathStreamData object].type will return the value, path of data, and type of data respectively.
   * 
   * These properties will store the result from calling the function [MultiPathStreamData object].get.
  */
  void setMultiPathStreamCallback(FirebaseData &fbdo, MultiPathStreamEventCallback multiPathDataCallback, StreamTimeoutCallback timeoutCallback = NULL);

  /** Remove stream callback functions.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
  */
  void removeStreamCallback(FirebaseData &fbdo);

  /** Remove multiple paths stream callback functions.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
  */
  void removeMultiPathStreamCallback(FirebaseData &fbdo);

  /** Backup (download) database at the defined database path to SD card/Flash memory.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param storageType Type of storage to save file, StorageType::SPIFS or StorageType::SD.
   * @param nodePath Database path to be backuped.
   * @param fileName File name to save.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Only 8.3 DOS format (max. 8 bytes file name and 3 bytes file extension) can be saved to SD card/Flash memory.
  */
  bool backup(FirebaseData &fbdo, uint8_t storageType, const String &nodePath, const String &fileName);

  /** Restore database at a defined path using backup file saved on SD card/Flash memory.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param storageType Type of storage to read file, StorageType::SPIFS or StorageType::SD.
   * @param nodePath Database path to  be restored.
   * @param fileName File name to read
   * @return Boolean type status indicates the success of the operation.
  */
  bool restore(FirebaseData &fbdo, uint8_t storageType, const String &nodePath, const String &fileName);

  /** Set maximum Firebase read/store retry operation (0 255) in case of network problems and buffer overflow.
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param num The maximum retry.
  */
  void setMaxRetry(FirebaseData &fbdo, uint8_t num);

  /** Set the maximum Firebase Error Queues in the collection (0 255). 
   * Firebase read/store operation causes by network problems and buffer overflow will be added to Firebase Error Queues collection.
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param num The maximum Firebase Error Queues.
  */
  void setMaxErrorQueue(FirebaseData &fbdo, uint8_t num);

  /** Save Firebase Error Queues as SPIFFS file (save only database store queues). 
   * Firebase read (get) operation will not be saved.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param filename Filename to be saved.
   * @param storageType Type of storage to save file, StorageType::SPIFS or StorageType::SD.
  */
  bool saveErrorQueue(FirebaseData &fbdo, const String &filename, uint8_t storageType);

  /** Delete file in Flash (SPIFFS) or SD card.
   * 
   * @param filename File name to delete.
   * @param storageType Type of storage to save file, StorageType::SPIFS or StorageType::SD.
  */
  bool deleteStorageFile(const String &filename, uint8_t storageType);

  /** estore Firebase Error Queues from the SPIFFS file.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param filename Filename to be read and restore queues.
   * @param storageType Type of storage to read file, StorageType::SPIFS or StorageType::SD.
  */
  bool restoreErrorQueue(FirebaseData &fbdo, const String &filename, uint8_t storageType);

  /** Determine the number of Firebase Error Queues stored in a defined SPIFFS file.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param filename Filename to be read and count for queues.
   * @param storageType Type of storage to read file, StorageType::SPIFS or StorageType::SD.
   * @return Number (0-255) of queues store in defined SPIFFS file.
  */
  uint8_t errorQueueCount(FirebaseData &fbdo, const String &filename, uint8_t storageType);

  /** Determine number of queues in Firebase Data object Firebase Error Queues collection.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @return Number (0-255) of queues in Firebase Data object queue collection.
  */
  uint8_t errorQueueCount(FirebaseData &fbdo);

  /** Determine whether the  Firebase Error Queues collection was full or not.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @return Boolean type status indicates whether the  Firebase Error Queues collection was full or not.
  */
  bool isErrorQueueFull(FirebaseData &fbdo);

  /** Process all failed Firebase operation queue items when the network is available.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param callback a Callback function that accepts QueueInfo parameter.
  */
  void processErrorQueue(FirebaseData &fbdo, QueueInfoCallback callback = NULL);

  /** Return Firebase Error Queue ID of last Firebase Error. 
   * Return 0 if there is no Firebase Error from the last operation.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @return Number of Queue ID.
  */
  uint32_t getErrorQueueID(FirebaseData &fbdo);

  /** Determine whether the Firebase Error Queue currently exists is Error Queue collection or not.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param errorQueueID The Firebase Error Queue ID get from getErrorQueueID.
   * @return Boolean type status indicates the queue existence.
  */
  bool isErrorQueueExisted(FirebaseData &fbdo, uint32_t errorQueueID);

  /** Start the Firebase Error Queues Auto Run Process.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param callback a Callback function that accepts QueueInfo Object as a parameter, optional.
   * 
   * @note The following functions are available from QueueInfo Object accepted by callback.
   * 
   * queueInfo.totalQueues(), get the total Error Queues in Error Queue Collection.
   * 
   * queueInfo.currentQueueID(), get current Error Queue ID that being process.
   * 
   * queueInfo.isQueueFull(), determine whether Error Queue Collection is full or not.
   * 
   * queueInfo.dataType(), get a string of the Firebase call data type that being process of current Error Queue.
   * 
   * queueInfo.method(), get a string of the Firebase call method that being process of current Error Queue.
   * 
   * queueInfo.path(), get a string of the Firebase call path that being process of current Error Queue.
  */
  void beginAutoRunErrorQueue(FirebaseData &fbdo, QueueInfoCallback callback = NULL);

  /** Stop the Firebase Error Queues Auto Run Process.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
  */
  void endAutoRunErrorQueue(FirebaseData &fbdo);

  /** Clear all Firbase Error Queues in Error Queue collection.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
  */
  void clearErrorQueue(FirebaseData &fbdo);

  /** Send Firebase Cloud Messaging to the device with the first registration token which added by firebaseData.fcm.addDeviceToken.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param index The index (starts from 0) of recipient device token which added by firebaseData.fcm.addDeviceToken
   * @return Boolean type status indicates the success of the operation.
  */
  bool sendMessage(FirebaseData &fbdo, uint16_t index);

  /** Send Firebase Cloud Messaging to all devices (multicast) which added by firebaseData.fcm.addDeviceToken.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @return Boolean type status indicates the success of the operation.
  */
  bool broadcastMessage(FirebaseData &fbdo);

  /** Send Firebase Cloud Messaging to devices that subscribed to the topic.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @return Boolean type status indicates the success of the operation.
  */
  bool sendTopic(FirebaseData &fbdo);

  void errorToString(int httpCode, std::string &buff);

  template <typename T>
  bool set(FirebaseData &fbdo, const String &path, T value);

  template <typename T>
  bool set(FirebaseData &fbdo, const String &path, T value, size_t size);

  template <typename T>
  bool set(FirebaseData &fbdo, const String &path, T value, float priority);

  template <typename T>
  bool set(FirebaseData &fbdo, const String &path, T value, size_t size, float priority);

  template <typename T>
  bool set(FirebaseData &fbdo, const String &path, T value, const String &ETag);

  template <typename T>
  bool set(FirebaseData &fbdo, const String &path, T value, size_t size, const String &ETag);

  template <typename T>
  bool set(FirebaseData &fbdo, const String &path, T value, float priority, const String &ETag);

  template <typename T>
  bool set(FirebaseData &fbdo, const String &path, T value, size_t size, float priority, const String &ETag);

  template <typename T>
  bool push(FirebaseData &fbdo, const String &path, T value);

  template <typename T>
  bool push(FirebaseData &fbdo, const String &path, T value, size_t size);

  template <typename T>
  bool push(FirebaseData &fbdo, const String &path, T value, float priority);

  template <typename T>
  bool push(FirebaseData &fbdo, const String &path, T value, size_t size, float priority);

private:
  callback_function_t _callback_function = nullptr;

  bool pushInt(FirebaseData &fbdo, const std::string &path, int intValue, bool queue, const std::string &priority);
  bool pushFloat(FirebaseData &fbdo, const std::string &path, float floatValue, bool queue, const std::string &priority);
  bool pushDouble(FirebaseData &fbdo, const std::string &path, double doubleValue, bool queue, const std::string &priority);
  bool pushBool(FirebaseData &fbdo, const std::string &path, bool boolValue, bool queue, const std::string &priority);
  bool pushBlob(FirebaseData &fbdo, const std::string &path, uint8_t *blob, size_t size, bool queue, const std::string &priority);
  bool setInt(FirebaseData &fbdo, const std::string &path, int intValue, bool queue, const std::string &priority, const std::string &etag);
  bool setFloat(FirebaseData &fbdo, const std::string &path, float floatValue, bool queue, const std::string &priority, const std::string &etag);
  bool setDouble(FirebaseData &fbdo, const std::string &path, double doubleValue, bool queue, const std::string &priority, const std::string &etag);
  bool setBool(FirebaseData &fbdo, const std::string &path, bool boolValue, bool queue, const std::string &priority, const std::string &etag);
  bool setBlob(FirebaseData &fbdo, const std::string &path, uint8_t *blob, size_t size, bool queue, const std::string &priority, const std::string &etag);

  void getUrlInfo(const std::string url, std::string &host, std::string &uri, std::string &auth);
  bool processRequest(FirebaseData &fbdo, fb_esp_method method, fb_esp_data_type dataType, const std::string &path, const char *buff, bool queue, const std::string &priority, const std::string &etag = "");
  bool processRequestFile(FirebaseData &fbdo, uint8_t storageType, fb_esp_method method, const std::string &path, const std::string &fileName, bool queue, const std::string &priority, const std::string &etag = "");
  bool handleRequest(FirebaseData &fbdo, uint8_t storageType, const std::string &path, fb_esp_method method, fb_esp_data_type dataType, const std::string &payload, const std::string &priority, const std::string &etag);
  bool handleStreamRequest(FirebaseData &fbdo, const std::string &path);
  bool handleStreamRead(FirebaseData &fbdo);
  void parseRespHeader(const char *buf, struct server_response_data_t &response);
  void parseRespPayload(const char *buf, struct server_response_data_t &response, bool getOfs);
  void setNumDataType(const char *buf, int ofs, struct server_response_data_t &response, bool dec);
  char *getHeader(const char *buf, PGM_P beginH, PGM_P endH, int &beginPos, int endPos);
  bool stringCompare(const char *buf, int ofs, PGM_P beginH);
  int readLine(WiFiClient *stream, char *buf, int bufLen);
  int readChunkedData(WiFiClient *stream, char *out, int &chunkState, int &chunkedSize, int &dataLen, int bufLen);
  bool waitResponse(FirebaseData &fbdo);
  void checkOvf(FirebaseData &fbdo, size_t len, struct server_response_data_t &resp);
  bool handleResponse(FirebaseData &fbdo);
  void closeFileHandle(FirebaseData &fbdo);
  void handlePayload(FirebaseData &fbdo, struct server_response_data_t &response, char *payload);
  bool reconnect(FirebaseData &fbdo, unsigned long dataTime = 0);
  void delS(char *p);
  char *newS(size_t len);
  char *newS(char *p, size_t len);
  char *newS(char *p, size_t len, char *d);

  void runStreamTask();
  void runErrorQueueTask();

  void preparePayload(fb_esp_method method, fb_esp_data_type dataType, const std::string &priority, const std::string &payload, std::string &buf);
  void prepareHeader(FirebaseData &fbdo, const std::string &host, fb_esp_method method, fb_esp_data_type dataType, const std::string &path, const std::string &auth, int payloadLength, std::string &header, bool sv);
  void clearDataStatus(FirebaseData &fbdo);
  void closeHTTP(FirebaseData &fbdo);
  int sendRequest(FirebaseData &fbdo, const std::string &path, fb_esp_method method, fb_esp_data_type dataType, const std::string &payload, const std::string &priority);
  void setSecure(FirebaseData &fbdo);
  bool connectionError(FirebaseData &fbdo);
  uint8_t openErrorQueue(FirebaseData &fbdo, const String &filename, uint8_t storageType, uint8_t mode);
  std::vector<std::string> splitString(int size, const char *str, const char delim);

  char *getPGMString(PGM_P pgm);
  char *getFloatString(float value);
  char *getDoubleString(double value);
  char *getIntString(int value);
  char *getBoolString(bool value);
  void pgm_appendStr(std::string &buf, PGM_P p, bool empty = false);
  void trimDigits(char *buf);
  void strcat_c(char *str, char c);
  int strpos(const char *haystack, const char *needle, int offset);
  char *rstrstr(const char *haystack, const char *needle);
  int rstrpos(const char *haystack, const char *needle, int offset);
  bool sdTest();
  void createDirs(std::string dirs, uint8_t storageType);
  bool replace(std::string &str, const std::string &from, const std::string &to);
  std::string base64_encode_string(const unsigned char *src, size_t len);
  void send_base64_encoded_stream(WiFiClient *client, const std::string &filePath, uint8_t storageType);
  bool base64_decode_string(const std::string src, std::vector<uint8_t> &out);
  bool base64_decode_file(File &file, const char *src, size_t len);
  bool base64_decode_flash(fs::File &file, const char *src, size_t len);
  uint32_t hex2int(const char *hex);

  bool handleFCMRequest(FirebaseData &fbdo, fb_esp_fcm_msg_type messageType);
  void setClock(float offset);

  void set_scheduled_callback(callback_function_t callback)
  {
    _callback_function = std::move([callback]() { schedule_function(callback); });
    _callback_function();
  }

  std::string _host = "";
  std::string _auth = "";
  std::shared_ptr<const char> _caCert = nullptr;
  int _certType = -1;
  std::string _caCertFile = "";
  uint8_t _caCertFileStoreageType = StorageType::FLASH;

  uint16_t _port = FIREBASE_PORT;
  uint8_t _sdPin = SD_CS_PIN;
  bool _reconnectWiFi = false;
  bool _sdOk = false;
  bool _sdInUse = false;
  bool _clockReady = false;
  unsigned long _lastReconnectMillis = 0;
  uint16_t _reconnectTimeout = WIFI_RECONNECT_TIMEOUT;
  File file;
  fs::File _file;
  float _gmtOffset = 0.0;
  uint8_t _floatDigits = 5;
  uint8_t _doubleDigits = 9;
};

class FirebaseData
{

public:
  FirebaseData();
  ~FirebaseData();

  /** Set the receive and transmit buffer memory size for secured mode BearSSL WiFi client.
   * 
   * @param rx The number of bytes for receive buffer memory for secured mode BearSSL (512 is minimum, 16384 is maximum).
   * @param tx The number of bytes for transmit buffer memory for secured mode BearSSL (512 is minimum, 16384 is maximum).
   * 
   * @note Set this option to false to support get large Blob and File operations.
  */
  void setBSSLBufferSize(uint16_t rx, uint16_t tx);

  /** Set the HTTP response size limit.
   * 
   * @param len The server response buffer size limit. 
  */
  void setResponseSize(uint16_t len);

  /** Pause/Unpause WiFiClient from all Firebase operations.
   * 
   * @param pause The boolean to set/unset pause operation.
   * @return Boolean type status indicates the success of operation.
  */
  bool pauseFirebase(bool pause);

  /** Get a WiFi client instance.
   * 
   * @return WiFi client instance.
  */
  SSL_CLIENT *getWiFiClient();

  /** Close the keep-alive connection of the internal WiFi client.
   * 
   * @note This will release the memory used by internal WiFi client.
  */
  void stopWiFiClient();

  /** Determine the data type of payload returned from the server.
   * 
   * @return The one of these data type e.g. integer, float, double, boolean, string, JSON and blob.
  */
  String dataType();

  /** Determine the event type of stream.
   * 
   * @return The one of these event type e.g. put, patch, cancel, and auth_revoked.
   * 
   * @note The event type "put" indicated that data at an event path relative to the stream path was completely changed. The event path can be determined by dataPath().
   * 
   * The event type "patch" indicated that data at the event path relative to stream path was updated. The event path can be determined by dataPath().
   * 
   * The event type "cancel" indicated something wrong and cancel by the server.
   * 
   * The event type "auth_revoked" indicated the provided Firebase Authentication Data (Database secret) is no longer valid.
  */
  String eventType();

  /** Determine the unique identifier (ETag) of current data.
   * 
   * @return String of unique identifier.
  */
  String ETag();

  /** Determine the current stream path.
   * 
   * @return The database streaming path.
  */
  String streamPath();

  /** Determine the current data path.
   * 
   * @return The database path which belongs to the server's returned payload.
   * 
   * @note The database path returned from this function in case of stream, also changed upon the child or parent's stream 
   * value changes.
  */
  String dataPath();

  /** Determine the error reason String from the process.
   * 
   * @return The error description string (String object).
  */
  String errorReason();

  /** Return the integer data of server returned payload.
   * 
   * @return integer value.
  */
  int intData();

  /** Return the float data of server returned payload.
   * 
   * @return Float value.
  */
  float floatData();

  /** Return the double data of server returned payload.
   * 
   * @return double value.
  */
  double doubleData();

  /** Return the Boolean data of server returned payload.
   * 
   * @return Boolean value.
  */
  bool boolData();

  /** Return the String data of server returned payload.
   * 
   * @return String (String object).
  */
  String stringData();

  /** Return the JSON String data of server returned payload.
   * 
   * @return String (String object).
  */
  String jsonString();

  /** Return the Firebase JSON object of server returned payload.
   * 
   * @return FirebaseJson object.
  */
  FirebaseJson &jsonObject();

  /** Return the Firebase JSON object pointer of server returned payload.
   * 
   * @return FirebaseJson object pointer.
  */
  FirebaseJson *jsonObjectPtr();

  /** Return the Firebase JSON Array object of server returned payload.
   * 
   * @return FirebaseJsonArray object.
  */
  FirebaseJsonArray &jsonArray();

  /** Return the Firebase JSON Array object pointer of server returned payload.
   * 
   * @return FirebaseJsonArray object pointer.
  */
  FirebaseJsonArray *jsonArrayPtr();

  /** Return the Firebase JSON Data object that keeps the get(parse) result.
   * 
   * @return FirebaseJsonData object.
  */
  FirebaseJsonData &jsonData();

  /** Return the Firebase JSON Data object pointer that keeps the get(parse) result.
   * 
   * @return FirebaseJsonData object pointer.
  */
  FirebaseJsonData *jsonDataPtr();

  /** Return the blob data (uint8_t) array of server returned payload.
   * 
   * @return Dynamic array of 8-bit unsigned integer i.e. std::vector<uint8_t>.
  */
  std::vector<uint8_t> blobData();

  /** Return the file stream of server returned payload.
   * 
   * @return the file stream.
  */
  fs::File fileStream();

  /** Return the new appended node's name or key of server returned payload when calling pushXXX function.
   * 
   * @return String (String object).
  */
  String pushName();

  /** Determine the stream connection status.
   * 
   * @return Boolean type status indicates whether the Firebase Data object is working with a stream or not.
  */
  bool isStream();

  /** Determine the server connection status.
   * 
   * @return Boolean type status indicates whether the Firebase Data object is connected to the server or not.
  */
  bool httpConnected();

  /** Determine the timeout event of the server's stream (30 sec is the default). 
   * Nothing to do when stream connection timeout, the stream connection will be automatically resumed.
   * 
   * @return Boolean type status indicates whether the stream was a timeout or not.
  */
  bool streamTimeout();

  /** Determine the availability of data or payload returned from the server.
   * 
   * @return Boolean type status indicates whether the server return the new payload or not.
  */
  bool dataAvailable();

  /** Determine the availability of stream event-data payload returned from the server.
   * 
   * @return Boolean type status indicates whether the server returns the stream event-data 
   * payload or not.
  */
  bool streamAvailable();

  /** Determine the matching between data type that intends to get from/store to database and the server's return payload data type.
   * 
   * @return Boolean type status indicates whether the type of data being get from/store to database 
   * and the server's returned payload is matched or not.
  */
  bool mismatchDataType();

  /** Determine the HTTP status code return from the server.
   * 
   * @return integer number of HTTP status.
  */
  int httpCode();

  /** Check the overflow of the returned payload data buffer.
   * 
   * @return The overflow status. 
   * 
   * @note Total default HTTP response buffer size is 400 bytes which can be set through FirebaseData.setResponseSize.
  */
  bool bufferOverflow();

  /** Determine the name (full path) of the backup file in SD card/Flash memory.
   * 
   * @return String (String object) of the file name that stores on SD card/Flash memory after backup operation.
  */
  String getBackupFilename();

  /** Determine the size of the backup file.
   * 
   * @return Size of backup file in byte after backup operation.
  */
  size_t getBackupFileSize();

  /** Clear or empty data in Firebase Data object.
  */
  void clear();

  /** Determine the error description for file transferring (push file, set file, backup and restore).
   * 
   * @return Error description string (String object).
  */
  String fileTransferError();

  /** Return the server's payload data.
   * 
   * @return Payload string (String object).
   * 
   * @note The returned String will be empty when the response data is File, BLOB, JSON and JSON Array objects.
   * 
   * For File data type, call fileStream to get the file stream.
   * 
   * For BLOB data type, call blobData to get the dynamic array of unsigned 8-bit data.
   * 
   * For JSON object data type, call jsonObject and jsonObjectPtr to get the object and its pointer.
   * 
   * For JSON Array data type, call jsonArray and jsonArrayPtr to get the object and its pointer.
  */
  String payload();

  FCMObject fcm;

  FirebaseESP8266HTTPClient httpClient;

  QueryFilter queryFilter;

private:
  FirebaseESP8266::StreamEventCallback _dataAvailableCallback = NULL;
  FirebaseESP8266::MultiPathStreamEventCallback _multiPathDataCallback = NULL;
  FirebaseESP8266::StreamTimeoutCallback _timeoutCallback = NULL;
  FirebaseESP8266::QueueInfoCallback _queueInfoCallback = NULL;

  int _QIdx = -1;
  int _idx = -1;
  uint8_t _dataTypeNum = 0;

  bool _isDataTimeout = false;
  bool _isStream = false;
  bool _isFCM = false;
  bool _isRTDB = false;
  bool _streamStop = false;
  bool _reqNoContent = false;

  bool _bufOvf = false;
  bool _streamDataChanged = false;
  bool _streamPathChanged = false;
  bool _dataAvailable = false;
  bool _keepAlive = false;
  bool _isChunkedEnc = false;
  bool _httpConnected = false;
  bool _mismatchDataType = false;
  bool _pathNotExist = false;
  bool _pause = false;
  bool _classicRequest = false;
  fb_esp_data_type resp_dataType = fb_esp_data_type::d_any;
  uint8_t _connectionStatus = 0;
  uint32_t _qID = 0;
  QueueManager _qMan;
  uint8_t _maxAttempt = 0;

  fb_esp_method _req_method = fb_esp_method::m_put;
  fb_esp_data_type _req_dataType = fb_esp_data_type::d_any;

  std::string _path = "";
  std::string _data = "";
  std::string _data2 = "";
  std::string _streamPath = "";
  std::string _pushName = "";
  std::string _file_transfer_error = "";
  std::string _fileName = "";
  std::string _redirectURL = "";
  std::string _fbError = "";
  std::string _eventType = "";
  std::string _resp_etag = "";
  std::string _req_etag = "";
  std::string _priority = "";
  uint8_t _storageType = 0;
  uint8_t _redirectCount = 0;
  int _redirect = 0;
  FirebaseJson _json;
  FirebaseJsonArray _jsonArr;
  FirebaseJsonData _jsonData;

  uint16_t _maxBlobSize = MAX_BLOB_PAYLOAD_SIZE;
  uint16_t _bsslRxSize = 512;
  uint16_t _bsslTxSize = 512;
  uint16_t _responseBufSize = 1024;

  unsigned long _streamTimeoutMillis = 0;
  unsigned long _streamReconnectMillis = 0;

  std::vector<uint8_t> _blob = std::vector<uint8_t>();
  std::vector<std::string> _childNodeList = std::vector<std::string>();

  int _httpCode = -1000;
  int _contentLength = 0;

  bool _priority_val_flag = false;
  bool _priority_json_flag = false;
  bool _shallow_flag = false;
  int _readTimeout = -1;
  std::string _writeLimit = "";

  unsigned long _dataMillis = 0;
  std::string _backupNodePath = "";
  std::string _backupDir = "";
  std::string _backupFilename = "";
  size_t _backupzFileSize = 0;

  bool handleStreamRead();
  bool handleResponse();
  std::string getDataType(uint8_t type);
  std::string getMethod(uint8_t method);

  void addQueue(fb_esp_method method,
                uint8_t storageType,
                fb_esp_data_type dataType,
                const std::string path,
                const std::string filename,
                const std::string payload,
                bool isQuery,
                int *intTarget,
                float *floatTarget,
                double *doubleTarget,
                bool *boolTarget,
                String *stringTarget,
                std::vector<uint8_t> *blobTarget,
                FirebaseJson *jsonTarget,
                FirebaseJsonArray *arrTarget);

  void clearQueueItem(QueueItem &item);

  void setQuery(QueryFilter &query);

  void clearNodeList();

  void addNodeList(const String *childPath, size_t size);

  friend class FirebaseESP8266;
  friend class QueueManager;
  friend class StreamData;
  friend class FCMObject;
};

class StreamData
{
public:
  StreamData();
  ~StreamData();
  String dataPath();
  String streamPath();
  int intData();
  float floatData();
  double doubleData();
  bool boolData();
  String stringData();
  String jsonString();
  FirebaseJson *jsonObjectPtr();
  FirebaseJson &jsonObject();
  FirebaseJsonArray *jsonArrayPtr();
  FirebaseJsonArray &jsonArray();
  FirebaseJsonData *jsonDataPtr();
  FirebaseJsonData &jsonData();
  std::vector<uint8_t> blobData();
  File fileStream();
  String dataType();
  String eventType();
  void empty();

  FirebaseJson *_json = nullptr;
  FirebaseJsonArray *_jsonArr = nullptr;
  FirebaseJsonData *_jsonData = nullptr;

private:
  std::string _streamPath = "";
  std::string _path = "";
  std::string _data = "";
  std::vector<uint8_t> _blob = std::vector<uint8_t>();
  std::string _dataTypeStr = "";
  std::string _eventTypeStr = "";
  uint8_t _dataType = 0;
  int _idx = -1;

  friend class FirebaseESP8266;
};

class MultiPathStreamData
{
  friend class FirebaseESP8266;

public:
  MultiPathStreamData();
  ~MultiPathStreamData();
  bool get(const String &path);
  String dataPath;
  String value;
  String type;

private:
  uint8_t _type = 0;
  std::string _data = "";
  std::string _path = "";
  std::string _typeStr = "";
  FirebaseJson *_json = nullptr;

  void empty();
};

extern FirebaseESP8266 Firebase;

#endif /* ESP8266 */

#endif /* FirebaseESP8266_H */