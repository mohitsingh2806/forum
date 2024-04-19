#include "misc.h"
#include <Arduino.h>
int main(void)
{

  init_all_peripherals(); // initialisation related tasks

  // ERASE EEPROM
  //  for (int i = EEPROM_LOC_START; i <= EEPROM_LOC_END; i++)
  //  {
  //    EEPROM.update(i, 0xFF);
  //  }

  while (true)
  {
    // Protocol:
    // 1. Maximum two simultaneous connections.
    // 2. In the two max connections 1 will be local, other will be hub
    // 3. If both are connected, priority will be given to local connection.
    // 4. Priority means that in this case Hub can only listen to the data, hub cannot give any commands. Hub cnx will be half duplex in this case
    // 5. For this system will have two modes. LOCAL and REMOTE. System will periodically send the MODE information to all connected clients
    // checkCableDisconnect();
    wdt_reset();    // reset the watchdog timer, this is to make sure the code doesn't get stuck anywhere
    checkTCPdata(); // check data received on TCP. this sho
    checkSerial();
    configServerTasks(); // configuration change task
  }
  return 0;
}
