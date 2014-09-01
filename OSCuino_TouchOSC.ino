//DHCP-based OSC server test code
//for use with IDE 1.0.5
//for use with W5100 or W5200 based ethernet shields

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <OSCBundle.h>

// you can find this written on the board of some Arduino Ethernets or shields
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 

// NOTE: Alternatively, you can assign a fixed IP to configure your
//       Ethernet shield.
//       byte ip[] = { 192, 168, 0, 154 };


int serverPort  = 8000; //TouchOSC (incoming port)
int destPort = 9000;    //TouchOSC (outgoing port)
int ledPin =  13;       //pin 13 on Arduino Uno. Pin 6 on a Teensy++2
int ledState = LOW;

//Create UDP message object
EthernetUDP Udp;

void setup(){
  Serial.begin(9600); //9600 for a "normal" Arduino board (Uno for example). 115200 for a Teensy ++2 
  Serial.println("OSC test");

  // start the Ethernet connection:
  // NOTE: Alternatively, you can assign a fixed IP to configure your
  //       Ethernet shield.
  //       Ethernet.begin(mac, ip);   
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    while(true);
  }
  // print your local IP address:
  Serial.print("Arduino IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }

  Udp.begin(serverPort);
}

void loop(){
  //process received messages
  OSCMsgReceive();
} 

void OSCMsgReceive(){
  OSCMessage msgIN;
  int size;
  if((size = Udp.parsePacket())>0){
    while(size--)
      msgIN.fill(Udp.read());
    if(!msgIN.hasError()){
      msgIN.route("/OnOff/toggle1",toggleOnOff);
      msgIN.route("/Fader/Value",funcValue);
    }
  }
}

void toggleOnOff(OSCMessage &msg, int addrOffset){
  ledState = (boolean) msg.getFloat(0);
  OSCMessage msgOUT("/OnOff/toggle1");

  digitalWrite(ledPin, ledState);

  msgOUT.add(ledState);
  if (ledState) {
    Serial.println("LED on");
  }
  else {
    Serial.println("LED off");
  }

  ledState = !ledState;		 // toggle the state from HIGH to LOW to HIGH to LOW ...

  //send osc message back to controll object in TouchOSC
  //Local feedback is turned off in the TouchOSC interface.
  //The button is turned on in TouchOSC interface whe the conrol receives this message.
  Udp.beginPacket(Udp.remoteIP(), destPort);
  msgOUT.send(Udp); // send the bytes
  Udp.endPacket(); // mark the end of the OSC Packet
  msgOUT.empty(); // free space occupied by message
}

void funcValue(OSCMessage &msg, int addrOffset ){

  int value = msg.getFloat(0);
  OSCMessage msgOUT("/Fader/Value");

  Serial.print("Value = : ");
  Serial.println(value);

  msgOUT.add(value);

  Udp.beginPacket(Udp.remoteIP(), destPort);
  msgOUT.send(Udp); // send the bytes
  Udp.endPacket(); // mark the end of the OSC Packet
  msgOUT.empty(); // free space occupied by message
}








