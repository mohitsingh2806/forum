#ifndef MISC_H
#define MISC_H

// Include statements
#include <EEPROM.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <avr/wdt.h>

// Constants for IP configuration
#define IP_ADDRESS 192,168,1,177
#define GATEWAY 0,0,0,0
#define SUBNET 255,255,255,0
#define MAC_ADDR {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC}
#define BROADCAST_IP 255,255,255,255

// Port configuration
#define PORT_FOR_LOCAL_CNX 6262
#define PORT_FOR_REMOTE_CNX 7277
#define PORT_FOR_UDP_CNX 4321


#define CONF_PORT_TELNET 23

// Communication configuration
#define DEFAULT_BAUD_RATE 115200
#define UDP_TX_PACKET_MAX 2000
// #define SERIAL_BUF_SIZE 1000
#define END_MARKER '\r'  // CARRIAGE RETURN
#define MAXPACKETSIZE_CONFIG 200

// EEPROM locations
#define EEPROM_LOC_START 80
#define EEPROM_LOC_END 150

#define EEPROM_LOC_IP_VALID_FLAG 80
#define EEPROM_LOC_IP_OCT1 81
#define EEPROM_LOC_IP_OCT2 82
#define EEPROM_LOC_IP_OCT3 83
#define EEPROM_LOC_IP_OCT4 84

#define EEPROM_LOC_GATEWAY_VALID_FLAG 85
#define EEPROM_LOC_GATEWAY_OCT1 86
#define EEPROM_LOC_GATEWAY_OCT2 87
#define EEPROM_LOC_GATEWAY_OCT3 88
#define EEPROM_LOC_GATEWAY_OCT4 89

#define EEPROM_LOC_SUBNET_VALID_FLAG 90
#define EEPROM_LOC_SUBNET_OCT1 91
#define EEPROM_LOC_SUBNET_OCT2 92
#define EEPROM_LOC_SUBNET_OCT3 93
#define EEPROM_LOC_SUBNET_OCT4 94

#define EEPROM_LOC_MAC_VALID_FLAG 95
#define EEPROM_LOC_MAC_OCT1 96
#define EEPROM_LOC_MAC_OCT2 97
#define EEPROM_LOC_MAC_OCT3 98
#define EEPROM_LOC_MAC_OCT4 99
#define EEPROM_LOC_MAC_OCT5 100
#define EEPROM_LOC_MAC_OCT6 101

#define EEPROM_LOC_REMOTE_IP_VALID_FLAG 102
#define EEPROM_LOC_REMOTE_IP_OCT1 103
#define EEPROM_LOC_REMOTE_IP_OCT2 104
#define EEPROM_LOC_REMOTE_IP_OCT3 105
#define EEPROM_LOC_REMOTE_IP_OCT4 106

#define EEPROM_LOC_REMOTE_PORT_VALID_FLAG 107
#define EEPROM_LOC_REMOTE_PORT 108  // 4 bytes

#define EEPROM_LOC_LOCAL_PORT_VALID_FLAG 112
#define EEPROM_LOC_LOCAL_PORT 113  // 4 bytes

#define EEPROM_LOC_BAUD_RATE_VALID_FLAG 117
#define EEPROM_LOC_BAUD_RATE 118  // 4 bytes



// Function prototypes
extern IPAddress g_broadcastIP;
extern EthernetUDP Udp;
extern void checkSerial();
extern void checkCableDisconnect();
extern void configServerTasks();
extern void init_all_peripherals(void);
extern void checkTCPdata();
extern unsigned char hex2byte(const char *hex);
extern void ipAddressToString(const IPAddress &ip, char *buffer, size_t bufferSize);

// Flags
#define EEPROM_VALID_VALUE_FLAG 1
#define NONE 0
#define ONLY_LOCAL 1 
#define ONLY_REMOTE 2
#define BOTH_LOCAL_AND_REMOTE 3



#define FALSE 0
#define TRUE 1

#endif
