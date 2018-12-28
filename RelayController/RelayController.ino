#include <EtherCard.h>
#include <Bounce2.h>

const static byte MAC_ADDRESS[] = { 0x74,0x69,0x70,0x2D,0x30,0x32 };
byte Ethernet::buffer[700];
Stash stash;
const byte HTTT_SERVER_IP[] = { 10,0,1,10 };

const int PIN_BUTTON = 2;
const int PIN_DOOR = 3;
const int PIN_RELAY = 4;

const char DOOR_CLOSED[] = "50ad294f-29eb-4950-bd7c-9008b5a62ea9";
const char DOOR_OPENED[] = "7ba0c15f-f718-4eb4-ad98-ec75676bf392";

const int BUTTON_DEBOUNCE_MILLIS = 50;
const int DOOR_DEBOUNCE_MILLIS = 500;

Bounce button = Bounce();
Bounce door = Bounce();

void activateRelay(){
  digitalWrite(PIN_RELAY,LOW);
}

void deactivateRelay(){
  digitalWrite(PIN_RELAY,HIGH);
}

void setupPorts() {
  Serial.begin(57600);
  Serial.println(F("[Arduino Door Bell Relay Controller]"));

  button.attach(PIN_BUTTON,INPUT_PULLUP);
  button.interval(BUTTON_DEBOUNCE_MILLIS);

  door.attach(PIN_DOOR,INPUT_PULLUP);
  door.interval(DOOR_DEBOUNCE_MILLIS);

  pinMode(PIN_RELAY, OUTPUT);
  deactivateRelay();
}

void setupEtherCard() {
  if (ether.begin(sizeof Ethernet::buffer, MAC_ADDRESS, SS) == 0)
    Serial.println(F("Failed to access Ethernet controller"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);
  ether.copyIp(ether.hisip, HTTT_SERVER_IP);
  ether.printIp("SRV: ", ether.hisip);
  ether.hisport = 8123;
  Serial.print("SRV_PORT: "); Serial.println(ether.hisport);
}

void setup () {
  setupPorts();
  setupEtherCard();
}

static void httpResponseCallback (byte status, word off, word len) {
  Serial.println(">>>");
  Ethernet::buffer[off+300] = 0;
  Serial.print((const char*) Ethernet::buffer + off);
  Serial.println("...");
}

void postHttpRequestButtonPress() {
  Serial.println();
  Serial.print("<<< REQ button press");

  char body[] = "";
  Stash::prepare(PSTR("POST /api/webhook/8e687740-7ecc-472f-84a1-bec5b1c74284 HTTP/1.1" "\r\n"
      "Host: klingel.freudl" "\r\n"
      "Content-Type: text/plain" "\r\n"
      "Content-Length: $D" "\r\n"
      "\r\n"
      "$S"),
      strlen(body), body); 
  ether.tcpSend();
}

void postHttpRequestDoorClosed() {
  Serial.println();
  Serial.print("<<< REQ door closed "); 

  char body[] = "";
  Stash::prepare(PSTR("POST /api/webhook/50ad294f-29eb-4950-bd7c-9008b5a62ea9 HTTP/1.1" "\r\n"
      "Host: klingel.freudl" "\r\n"
      "Content-Type: text/plain" "\r\n"
      "Content-Length: $D" "\r\n"
      "\r\n"
      "$S"),
      strlen(body), body); 
  ether.tcpSend();
}

void postHttpRequestDoorOpened() {
  Serial.println();
  Serial.print("<<< REQ door opened "); 

  char body[] = "";
  Stash::prepare(PSTR("POST /api/webhook/7ba0c15f-f718-4eb4-ad98-ec75676bf392 HTTP/1.1" "\r\n"
      "Host: klingel.freudl" "\r\n"
      "Content-Type: text/plain" "\r\n"
      "Content-Length: $D" "\r\n"
      "\r\n"
      "$S"),
      strlen(body), body); 
  ether.tcpSend();
}

void loop () {
  button.update();  
  door.update();

  ether.packetLoop(ether.packetReceive());

  if (button.fell()) {
    activateRelay();
    postHttpRequestButtonPress();
  }
  if (button.rose()) {
    deactivateRelay();
  }

  if (door.fell()){
    postHttpRequestDoorClosed();
  }
  if (door.rose()){
    postHttpRequestDoorOpened();
  }
}
