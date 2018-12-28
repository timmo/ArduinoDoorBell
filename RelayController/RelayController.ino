
// API documentation: https://www.aelius.com/njh/ethercard/
// Library home page: https://github.com/njh/EtherCard

#include <EtherCard.h>
#include <Bounce2.h>

const static byte MAC_ADDRESS[] = { 0x74,0x69,0x70,0x2D,0x30,0x32 };
byte Ethernet::buffer[700];
Stash stash;
const char HTTP_HOST_NAME[] PROGMEM = "klingel.freudl";
const byte HTTT_SERVER_IP[] = { 10,0,1,10 };


const int PIN_BUTTON = 2;
const int PIN_DOOR = 3;
const int PIN_RELAY = 4;

// int lastButtonState = LOW;   // the previous reading from the input pin
// unsigned uint32_t BUTTON_lastDebounceTime = 0;  // the last time the output pin was toggled
const uint32_t BUTTON_DEBOUNCDE_DELAY_MILLIS = 100; // the debounce time; increase if the output flickers

Bounce button = Bounce();
Bounce door = Bounce();

// static uint32_t timer;

void activateRelay(){
  digitalWrite(PIN_RELAY,LOW);
}

void deactivateRelay(){
  digitalWrite(PIN_RELAY,HIGH);
}

void setupPorts() {
  Serial.begin(57600);
  Serial.println(F("[Arduino Door Bell Relay Controller]"));

  // pinMode(PIN_BUTTON, INPUT_PULLUP);
  // pinMode(PIN_DOOR, INPUT_PULLUP);
  button.attach(PIN_BUTTON,INPUT_PULLUP);
  button.interval(50);
  door.attach(PIN_DOOR,INPUT_PULLUP);
  door.interval(50);
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

void postHttpRequest() {
  Serial.println();
  Serial.print("<<< REQ ");

  // ether.browseUrl(PSTR("/api/webhook/8e687740-7ecc-472f-84a1-bec5b1c74284"), "bar", HTTP_HOST_NAME, httpResponseCallback);
  // ether.httpPost(PSTR("/api/webhook/8e687740-7ecc-472f-84a1-bec5b1c74284"), HTTP_HOST_NAME,'\0','\0', httpResponseCallback);

  char body[] = "";
  Stash::prepare(PSTR("POST /api/webhook/8e687740-7ecc-472f-84a1-bec5b1c74284 HTTP/1.1" "\r\n"
      "Host: klingel.freudl" "\r\n"
      "Content-Type: text/plain" "\r\n"
      "Content-Length: $D" "\r\n"
      "\r\n"
      "$S"),
      strlen(body), body); 
  ether.tcpSend(); // send the packet - this also releases all stash buffers once done
}


void loop () {
  button.update();  
  door.update();

  ether.packetLoop(ether.packetReceive());

  if (button.fell()) {
    activateRelay();
    postHttpRequest();
  }
  if (button.rose()) {
    deactivateRelay();
  }
}
