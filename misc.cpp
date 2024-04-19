#include "misc.h"

// Default ethernet configuration
byte g_mac[] = MAC_ADDR;
IPAddress g_ip(IP_ADDRESS);
IPAddress g_gateway(GATEWAY);
IPAddress g_subnet(SUBNET);
IPAddress g_broadcastIP(BROADCAST_IP);

// Ports for connections
uint32_t g_port_for_local_cnx = PORT_FOR_LOCAL_CNX;   // To be used for MPT GUI
uint32_t g_port_for_remote_cnx = PORT_FOR_REMOTE_CNX; // To be used for UNMS at Hub
uint32_t g_port_for_udp_cnx = PORT_FOR_UDP_CNX;

// Ethernet servers for data connections
EthernetServer *g_dataServer_local_cnx;  // To be used for MPT GUI
EthernetServer *g_dataServer_remote_cnx; // To be used for UNMS at Hub

// Configuration server
EthernetServer configurationServer(CONF_PORT_TELNET);

// Configuration related flags and data
boolean g_configurationPktTransferComplete = false;
char g_configurationMessage[50] = {'\0'};
boolean g_configurationDataReady = false;

// Ethernet clients
EthernetClient g_confclient;
EthernetClient g_localDataclient;
EthernetClient g_remoteDataclient;

// UDP connection
EthernetUDP Udp;

// Baud rate configuration
uint32_t g_baud_rate = DEFAULT_BAUD_RATE;

void checkCableDisconnect()
{
  if (Ethernet.hardwareStatus() == EthernetNoHardware || Ethernet.linkStatus() != LinkON)
  {
    Ethernet.begin(0);
    delay(50);
    Ethernet.begin(g_mac, g_ip, g_gateway, g_gateway, g_subnet); // start ethernet so that ping connectivity can be checked before other processes are complete
  }
}
void init_all_peripherals(void)
{

  /************************************* controller initialisation start****************************************/
  init(); // arduino related initialisations

  wdt_enable(WDTO_4S);

  byte o1, o2, o3, o4, o5, o6;
  if (EEPROM.read(EEPROM_LOC_IP_VALID_FLAG) == EEPROM_VALID_VALUE_FLAG) // if eeprom has valid data, then initialise from eeprom, otherwise initialise from decalared value in code.
  {
    o1 = EEPROM.read(EEPROM_LOC_IP_OCT1);
    o2 = EEPROM.read(EEPROM_LOC_IP_OCT2);
    o3 = EEPROM.read(EEPROM_LOC_IP_OCT3);
    o4 = EEPROM.read(EEPROM_LOC_IP_OCT4);
    g_ip = IPAddress(o1, o2, o3, o4); // take IP Address from EEPROM
  }

  if (EEPROM.read(EEPROM_LOC_GATEWAY_VALID_FLAG) == EEPROM_VALID_VALUE_FLAG)
  {
    o1 = EEPROM.read(EEPROM_LOC_GATEWAY_OCT1);
    o2 = EEPROM.read(EEPROM_LOC_GATEWAY_OCT2);
    o3 = EEPROM.read(EEPROM_LOC_GATEWAY_OCT3);
    o4 = EEPROM.read(EEPROM_LOC_GATEWAY_OCT4);
    g_gateway = IPAddress(o1, o2, o3, o4);
  }

  if (EEPROM.read(EEPROM_LOC_SUBNET_VALID_FLAG) == EEPROM_VALID_VALUE_FLAG)
  {
    o1 = EEPROM.read(EEPROM_LOC_SUBNET_OCT1);
    o2 = EEPROM.read(EEPROM_LOC_SUBNET_OCT2);
    o3 = EEPROM.read(EEPROM_LOC_SUBNET_OCT3);
    o4 = EEPROM.read(EEPROM_LOC_SUBNET_OCT4);
    g_subnet = IPAddress(o1, o2, o3, o4);
  }

  if (EEPROM.read(EEPROM_LOC_MAC_VALID_FLAG) == EEPROM_VALID_VALUE_FLAG)
  {
    o1 = EEPROM.read(EEPROM_LOC_MAC_OCT1);
    o2 = EEPROM.read(EEPROM_LOC_MAC_OCT2);
    o3 = EEPROM.read(EEPROM_LOC_MAC_OCT3);
    o4 = EEPROM.read(EEPROM_LOC_MAC_OCT4);
    o5 = EEPROM.read(EEPROM_LOC_MAC_OCT5);
    o6 = EEPROM.read(EEPROM_LOC_MAC_OCT6);

    g_mac[0] = o1;
    g_mac[1] = o2;
    g_mac[2] = o3;
    g_mac[3] = o4;
    g_mac[4] = o5;
    g_mac[5] = o6;
  }

  Ethernet.begin(g_mac, g_ip, g_gateway, g_gateway, g_subnet); // start ethernet so that ping connectivity can be checked before other processes are complete

  if (EEPROM.read(EEPROM_LOC_LOCAL_PORT_VALID_FLAG) == EEPROM_VALID_VALUE_FLAG)
  {
    EEPROM.get(EEPROM_LOC_LOCAL_PORT, g_port_for_local_cnx);
  }

  if (EEPROM.read(EEPROM_LOC_REMOTE_PORT_VALID_FLAG) == EEPROM_VALID_VALUE_FLAG)
  {
    EEPROM.get(EEPROM_LOC_REMOTE_PORT, g_port_for_remote_cnx);
  }

  if (EEPROM.read(EEPROM_LOC_BAUD_RATE_VALID_FLAG) == EEPROM_VALID_VALUE_FLAG)
  {
    EEPROM.get(EEPROM_LOC_BAUD_RATE, g_baud_rate);
  }

  Serial.begin(g_baud_rate);                                           // start serial port
  Udp.begin(g_port_for_udp_cnx);                                       // for own IP broadcast
  g_dataServer_local_cnx = new EthernetServer(g_port_for_local_cnx);   // create instanceof data server  for local cnx(TCP)
  g_dataServer_local_cnx->begin();                                     // start data server
  g_dataServer_remote_cnx = new EthernetServer(g_port_for_remote_cnx); // create instanceof data server  for remote cnx(TCP)
  g_dataServer_remote_cnx->begin();                                    // start data server
  configurationServer.begin();                                         // start configuration server  // it is already instantiated in declaration as its port is fixed
}

// void checkSerial()
// {
//   char c[2000] = {0}; // initialise with all zeros as zero signifies string ending.  // actually it need not be > 64
//   // so no need to end the string explicityle to use string functions
//   // uint32_t i = 0;
//   // returns number of characters available in serial buffer
//   uint32_t serialCharCount = 0;

//         // Serial.end();
//         // Serial.begin(g_baud_rate);


  

//     if (Ethernet.linkStatus() == LinkON && (g_localDataclient.connected() || g_remoteDataclient.connected()))
//     {
//         Serial.begin(g_baud_rate);

//      if (Serial.available() > 0) // read till we have data in serial buffer
//   { 
//       serialCharCount = Serial.available();
     
//       for (uint32_t count = 0; count < serialCharCount; count++)
//       {

//         c[count] = Serial.read();
//       }
//       if (g_localDataclient.connected()) // if there is a client connected on TCP we send the serial data. otherwise do nothing
//       {
//         g_localDataclient.write(c, serialCharCount); // specify the number of bytes to be written. this was done to solve bug that omitted 0s
//       }

//       if (g_remoteDataclient.connected()) // if there is a client connected on TCP we send the serial data. otherwise do nothing
//       {
//         g_remoteDataclient.write(c, serialCharCount); // specify the number of bytes to be written. this was done to solve bug that omitted 0s
//       }
//     }
//     else
//     {
//       // while (Serial.available() > 0)
//       //   Serial.read();
//       Serial.end();
//     }
//   }
// }


void checkSerial() {
  char c[2000] = {0}; // initialize with all zeros as zero signifies string ending
  uint32_t serialCharCount = Serial.available(); // returns number of characters available in serial buffer

  if (serialCharCount > 0) { // read only if there is data in serial buffer
    if (Ethernet.linkStatus() == LinkON) { // Check Ethernet connection status
      if (g_localDataclient.connected() || g_remoteDataclient.connected()) {
        for (uint32_t count = 0; count < serialCharCount; count++) {
          c[count] = Serial.read();
        }

        if (g_localDataclient.connected()) {
          g_localDataclient.write(c, serialCharCount);
        }

        if (g_remoteDataclient.connected()) {
          g_remoteDataclient.write(c, serialCharCount);
        }
      } else {
        // Read and discard the serial data if no clients are connected
        while (Serial.available()) {
          Serial.read();
        }
      }
    } else {
      // If Ethernet connection is not available, read and discard the serial data
      while (Serial.available()) {
        Serial.read();
      }
    }
  }
}


void configServerTasks()
{
  char *ret = NULL;
  static boolean recvInProgress = false;
  static byte ndx = 255;
  char endMarker = END_MARKER;
  char rc = 0;
  char ipAddressStr[16] = {0};

  if (!g_confclient.connected()) // this ensures only one connection on this server
  {
    g_confclient.stop();                         // release the unused socket.
    g_confclient = configurationServer.accept(); // accept incoming connections
  }

  if (g_confclient)
  {

    // Process incoming data
    while (g_confclient.available() > 0 && g_configurationPktTransferComplete == false)
    {
      rc = g_confclient.read();

      if (recvInProgress == true)
      {
        if ((rc == endMarker) or (ndx >= MAXPACKETSIZE_CONFIG - 1)) // either end of packet reached or buffer overflow
        {
          g_configurationMessage[ndx] = 0; // terminate the string
          recvInProgress = false;
          ndx = 255;
          g_configurationPktTransferComplete = true;
        }
        else
        {
          g_configurationMessage[ndx] = rc;
          ndx++;
        }
      }
      else if (ndx == 255)
      {
        recvInProgress = true;
        g_configurationDataReady = false;
        ndx = 0;
        g_configurationMessage[ndx] = rc;
        ndx++;
      }
    }

    if (g_configurationPktTransferComplete == true)
    {

      byte o1, o2, o3, o4;

      ret = strstr(g_configurationMessage, "?");
      if (ret != NULL)
      {

        g_confclient.write("    \r\n"
                           "    FORMAT:\r\n"
                           "    LOCAL_IP=192.168.1.1<CR>\r\n"
                           "    DEFAULT_GATEWAY=192.168.1.1<CR>\r\n"
                           "    SUBNET=255.255.255.0<CR>\r\n"
                           "    MAC_ADDRESS=12:34:56:08:9A:BC<CR>\r\n"
                           "    LOCAL_PORT=8821<CR>\r\n"
                           "    REMOTE_PORT=1111<CR>\r\n"
                           "    BAUD_RATE=9600<CR>\r\n");
      }

      ret = strstr(g_configurationMessage, "HELP");
      if (ret != NULL)
      {

        g_confclient.write("    \r\n"
                           "    FORMAT:\r\n"
                           "    LOCAL_IP=192.168.1.1<CR>\r\n"
                           "    DEFAULT_GATEWAY=192.168.1.1<CR>\r\n"
                           "    SUBNET=255.255.255.0<CR>\r\n"
                           "    MAC_ADDRESS=12:34:56:08:9A:BC<CR>\r\n"
                           "    LOCAL_PORT=8821<CR>\r\n"
                           "    REMOTE_PORT=1111<CR>\r\n"
                           "    BAUD_RATE=9600<CR>\r\n");
      }
      ret = strstr(g_configurationMessage, "LOCAL_IP=");
      if (ret != NULL)
      {
        ret = strtok(ret, "=");
        ret = strtok(NULL, ".");
        o1 = atoi(ret);
        ret = strtok(NULL, ".");
        o2 = atoi(ret);
        ret = strtok(NULL, ".");
        o3 = atoi(ret);
        ret = strtok(NULL, "\0");
        o4 = atoi(ret);

        EEPROM.update(EEPROM_LOC_IP_VALID_FLAG, EEPROM_VALID_VALUE_FLAG);
        EEPROM.update(EEPROM_LOC_IP_OCT1, o1);
        EEPROM.update(EEPROM_LOC_IP_OCT2, o2);
        EEPROM.update(EEPROM_LOC_IP_OCT3, o3);
        EEPROM.update(EEPROM_LOC_IP_OCT4, o4);

        g_ip = IPAddress(o1, o2, o3, o4);

        ipAddressToString(g_ip, ipAddressStr, sizeof(ipAddressStr));

        g_confclient.write("\r\nNew IP Address is : ");
        g_confclient.write(ipAddressStr);

        Ethernet.begin(g_mac, g_ip, g_gateway, g_gateway, g_subnet);
      }

      ret = strstr(g_configurationMessage, "SUBNET=");
      if (ret != NULL)
      {
        ret = strtok(ret, "=");
        ret = strtok(NULL, ".");
        o1 = atoi(ret);
        ret = strtok(NULL, ".");
        o2 = atoi(ret);
        ret = strtok(NULL, ".");
        o3 = atoi(ret);
        ret = strtok(NULL, "\0");
        o4 = atoi(ret);
        EEPROM.update(EEPROM_LOC_SUBNET_VALID_FLAG, EEPROM_VALID_VALUE_FLAG);
        EEPROM.update(EEPROM_LOC_SUBNET_OCT1, o1);
        EEPROM.update(EEPROM_LOC_SUBNET_OCT2, o2);
        EEPROM.update(EEPROM_LOC_SUBNET_OCT3, o3);
        EEPROM.update(EEPROM_LOC_SUBNET_OCT4, o4);
        g_subnet = IPAddress(o1, o2, o3, o4);
        ipAddressToString(g_subnet, ipAddressStr, sizeof(ipAddressStr));

        g_confclient.write("\r\nNew Subnet Mask is : ");
        g_confclient.write(ipAddressStr);

        Ethernet.begin(g_mac, g_ip, g_gateway, g_gateway, g_subnet);
      }

      ret = strstr(g_configurationMessage, "DEFAULT_GATEWAY=");
      if (ret != NULL)
      {
        ret = strtok(ret, "=");
        ret = strtok(NULL, ".");
        o1 = atoi(ret);
        ret = strtok(NULL, ".");
        o2 = atoi(ret);
        ret = strtok(NULL, ".");
        o3 = atoi(ret);
        ret = strtok(NULL, "\0");
        o4 = atoi(ret);

        EEPROM.update(EEPROM_LOC_GATEWAY_VALID_FLAG, EEPROM_VALID_VALUE_FLAG);
        EEPROM.update(EEPROM_LOC_GATEWAY_OCT1, o1);
        EEPROM.update(EEPROM_LOC_GATEWAY_OCT2, o2);
        EEPROM.update(EEPROM_LOC_GATEWAY_OCT3, o3);
        EEPROM.update(EEPROM_LOC_GATEWAY_OCT4, o4);
        g_gateway = IPAddress(o1, o2, o3, o4);
        ipAddressToString(g_gateway, ipAddressStr, sizeof(ipAddressStr));

        g_confclient.write("\r\nNew Default Gateway is : ");
        g_confclient.write(ipAddressStr);

        Ethernet.begin(g_mac, g_ip, g_gateway, g_gateway, g_subnet);
      }

      char *ret = strstr(g_configurationMessage, "MAC_ADDRESS=");
      if (ret != NULL)
      {
        ret += strlen("MAC_ADDRESS="); // Move pointer to the start of MAC address after the equal sign
        for (int i = 0; i < 6; i++)
        {
          g_mac[i] = hex2byte(ret);
          ret += 3; // Jump over two hex chars and one colon
        }

        EEPROM.update(EEPROM_LOC_MAC_VALID_FLAG, EEPROM_VALID_VALUE_FLAG);
        EEPROM.update(EEPROM_LOC_MAC_OCT1, g_mac[0]);
        EEPROM.update(EEPROM_LOC_MAC_OCT2, g_mac[1]);
        EEPROM.update(EEPROM_LOC_MAC_OCT3, g_mac[2]);
        EEPROM.update(EEPROM_LOC_MAC_OCT4, g_mac[3]);
        EEPROM.update(EEPROM_LOC_MAC_OCT5, g_mac[4]);
        EEPROM.update(EEPROM_LOC_MAC_OCT6, g_mac[5]);

        g_confclient.write("\r\nMAC Address Received");
        Ethernet.begin(g_mac, g_ip, g_gateway, g_gateway, g_subnet);
      }

      ret = strstr(g_configurationMessage, "LOCAL_PORT=");
      if (ret != NULL)
      {
        ret = strtok(ret, "=");
        ret = strtok(NULL, "");

        g_port_for_local_cnx = atoi(ret);
        EEPROM.update(EEPROM_LOC_LOCAL_PORT_VALID_FLAG, EEPROM_VALID_VALUE_FLAG);
        EEPROM.update(EEPROM_LOC_LOCAL_PORT, g_port_for_local_cnx);
        g_confclient.write("\r\nPort for local connection Updated to: ");
        g_confclient.print(g_port_for_local_cnx);
        g_confclient.write(". Reboot the system\r\n");

        delay(10000);
      }

      ret = strstr(g_configurationMessage, "REMOTE_PORT=");
      if (ret != NULL)
      {
        ret = strtok(ret, "=");
        ret = strtok(NULL, "");

        g_port_for_remote_cnx = atoi(ret);
        EEPROM.update(EEPROM_LOC_REMOTE_PORT_VALID_FLAG, EEPROM_VALID_VALUE_FLAG);
        EEPROM.update(EEPROM_LOC_REMOTE_PORT, g_port_for_remote_cnx);
        g_confclient.write("\r\nPort for remote connection Updated to: ");
        g_confclient.print(g_port_for_remote_cnx);
        g_confclient.write(". Reboot the system\r\n");

        delay(10000);
      }

      ret = strstr(g_configurationMessage, "BAUD_RATE=");
      if (ret != NULL)
      {
        ret = strtok(ret, "=");
        ret = strtok(NULL, "");
        g_baud_rate = strtol(ret, 0, 10);

        Serial.end();
        Serial.begin(g_baud_rate);
        EEPROM.update(EEPROM_LOC_BAUD_RATE_VALID_FLAG, EEPROM_VALID_VALUE_FLAG);
        EEPROM.update(EEPROM_LOC_BAUD_RATE, g_baud_rate);

        g_confclient.write("\r\nBaud Rate for Serial Connection updated to: ");
        g_confclient.print(g_baud_rate);
      }

      ret = strstr(g_configurationMessage, "ERASE@5433");
      if (ret != NULL)
      {

        g_confclient.write("\r\nDevice Erase started.. \r\n");
        for (int i = EEPROM_LOC_START; i <= EEPROM_LOC_END; i++)
        {
          EEPROM.update(i, 0xFF);
        }
        g_confclient.write("Device Erased. Reboot the system\r\n");
        delay(10000);
      }

      g_configurationDataReady = true;
      g_configurationPktTransferComplete = false;
    }
  }
}

// void checkTCPdata()
// {
//   static uint32_t timeSinceLastBroadcast = millis();
//   static uint32_t timeSinceLastModeSent = millis();
//   uint32_t currentTime = millis();
//   char ipAddressStr[16]; // Adjust size as needed
//   char rc = 0;
//   EthernetClient tempClient;
//   if (!g_localDataclient.connected())
//   {

//     tempClient = g_dataServer_local_cnx->accept(); // accept incoming connections
//     if (tempClient)
//     {
//       g_localDataclient.stop();
//       g_localDataclient = tempClient;
//     }
//   }

//   if (g_localDataclient.connected())
//   {
//     tempClient = g_dataServer_local_cnx->accept(); // accept incoming connections
//     tempClient.stop();

//     while (g_localDataclient.available() > 0) // if any data is received on TCP, write to serial
//     {
//       rc = g_localDataclient.read();
//       Serial.write(rc);
//     }
//   }

//   if (!g_remoteDataclient.connected())
//   {

//     tempClient = g_dataServer_remote_cnx->accept(); // accept incoming connections

//     if (tempClient)
//     {
//       g_remoteDataclient.stop();
//       g_remoteDataclient = tempClient;
//     }
//   }
//   if (g_remoteDataclient.connected())
//   {
//     tempClient = g_dataServer_remote_cnx->accept(); // accept incoming connections
//     tempClient.stop();

//     if (!g_localDataclient.connected())
//     {
//       while (g_remoteDataclient.available() > 0) // if any data is received on TCP, write to serial
//       {
//         rc = g_remoteDataclient.read();
//         Serial.write(rc);
//       }
//     }
//     else
//     {
//       while (g_remoteDataclient.available() > 0) // if any data is received on TCP, write to serial
//       {
//         rc = g_remoteDataclient.read();
//       }
//     }
//   }

//   if ((currentTime - timeSinceLastBroadcast) >= 20000) // if there has not been any client connection broadcast own IP on port 9999
//   {

//     if (!g_localDataclient.connected() && !g_remoteDataclient.connected())
//     {
//       ipAddressToString(g_ip, ipAddressStr, sizeof(ipAddressStr));
//       timeSinceLastBroadcast = millis();
//       Udp.beginPacket(g_broadcastIP, 9999);
//       Udp.write(ipAddressStr);
//       Udp.endPacket();
//     }
//   }

//   if ((currentTime - timeSinceLastModeSent) >= 15000) // if there has not been any client connection broadcast own IP on port 9999
//   {
//     timeSinceLastModeSent = millis();
//     if (g_localDataclient.connected() && g_remoteDataclient.connected())
//     {
//       g_localDataclient.write("MODE:L&R");
//       g_remoteDataclient.write("MODE:L&R");
//     }
//     else if (g_localDataclient.connected() && !g_remoteDataclient.connected())
//     {
//       g_localDataclient.write("MODE:L");
//     }
//     else if (!g_localDataclient.connected() && g_remoteDataclient.connected())
//     {
//       g_remoteDataclient.write("MODE:R");
//     }
//   }
// }



void checkTCPdata() {
  static uint32_t timeSinceLastBroadcast = millis();
  static uint32_t timeSinceLastModeSent = millis();
  uint32_t currentTime = millis();
  char ipAddressStr[16]; // Adjust size as needed
  char rc = 0;
  EthernetClient tempClient;

  // Accept local data client connection
  if (!g_localDataclient.connected()) {
    tempClient = g_dataServer_local_cnx->accept();
    if (tempClient) {
      g_localDataclient.stop();
      g_localDataclient = tempClient;
    }
  }

  // Accept remote data client connection
  if (!g_remoteDataclient.connected()) {
    tempClient = g_dataServer_remote_cnx->accept();
    if (tempClient) {
      g_remoteDataclient.stop();
      g_remoteDataclient = tempClient;
    }
  }

  // Read from local data client and write to Serial
  if (g_localDataclient.connected()) {
    while (g_localDataclient.available() > 0) {
      rc = g_localDataclient.read();
      Serial.write(rc);
    }
  }

  // Read from remote data client and write to Serial if local client is not connected
  if (g_remoteDataclient.connected() && !g_localDataclient.connected()) {
    while (g_remoteDataclient.available() > 0) {
      rc = g_remoteDataclient.read();
      Serial.write(rc);
    }
  }

  // Broadcast own IP if no client is connected
  if ((currentTime - timeSinceLastBroadcast) >= 20000) {
    if (!g_localDataclient.connected() && !g_remoteDataclient.connected()) {
      ipAddressToString(g_ip, ipAddressStr, sizeof(ipAddressStr));
      timeSinceLastBroadcast = millis();
      Udp.beginPacket(g_broadcastIP, 9999);
      Udp.write(ipAddressStr);
      Udp.endPacket();
    }
  }

  // Send mode updates
  if ((currentTime - timeSinceLastModeSent) >= 15000) {
    timeSinceLastModeSent = millis();
    if (g_localDataclient.connected() && g_remoteDataclient.connected()) {
      g_localDataclient.write("MODE:L&R");
      g_remoteDataclient.write("MODE:L&R");
    } else if (g_localDataclient.connected() && !g_remoteDataclient.connected()) {
      g_localDataclient.write("MODE:L");
    } else if (!g_localDataclient.connected() && g_remoteDataclient.connected()) {
      g_remoteDataclient.write("MODE:R");
    }
  }
}


unsigned char hex2byte(const char *hex)
{
  unsigned char val = 0;
  for (int i = 0; i < 2; i++)
  {
    unsigned char byte = hex[i];
    val <<= 4;
    if (byte >= '0' && byte <= '9')
    {
      val |= (byte - '0');
    }
    else if (byte >= 'A' && byte <= 'F')
    {
      val |= (byte - 'A' + 10);
    }
    else if (byte >= 'a' && byte <= 'f')
    {
      val |= (byte - 'a' + 10);
    }
    else
    {
      // Invalid hex character
      return 0;
    }
  }
  return val;
}

void ipAddressToString(const IPAddress &ip, char *buffer, size_t bufferSize)
{
  snprintf(buffer, bufferSize, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}