
// API documentation: https://www.aelius.com/njh/ethercard/
// Library home page: https://github.com/njh/EtherCard

#include <EtherCard.h>

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74,0x69,0x70,0x2D,0x30,0x32 };

byte Ethernet::buffer[700];
static uint32_t timer;

const char website[] PROGMEM = "smarthome.freudl";

static void response_callback (byte status, word off, word len) {
  Serial.println(">>>");
  Ethernet::buffer[off+300] = 0;
  Serial.print((const char*) Ethernet::buffer + off);
  Serial.println("...");
}

void setup () {
  Serial.begin(57600);
  Serial.println(F("\n[webClient]"));

  // Change 'SS' to your Slave Select pin, if you arn't using the default pin
  if (ether.begin(sizeof Ethernet::buffer, mymac, SS) == 0)
    Serial.println(F("Failed to access Ethernet controller"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);

  byte hisip[] = { 10,0,1,10 };
  ether.copyIp(ether.hisip, hisip);

  ether.printIp("SRV: ", ether.hisip);
}

void loop () {
  ether.packetLoop(ether.packetReceive());

  if (millis() > timer) {
    timer = millis() + 5000;
    Serial.println();
    Serial.print("<<< REQ ");
    ether.browseUrl(PSTR("/api/webhook/8e687740-7ecc-472f-84a1-bec5b1c74284"), "bar", website, response_callback);
  }
}
