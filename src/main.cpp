#include <Arduino.h>
#include "WiFi.h"
#include "SipMachine.h"
//#include "Debug.h"
#include "SD.h"

const char *ssid = "Yer A Wifi Harry";               //  your network SSID (name)
const char *passphrase = "Pannekoekenplantdealer1996!"; // your network password (use for WPA, or use as key for WEP)


//Setup SIP
String user = "999";
String pwd  = "97a42b335ffd33c668aebae251f14579";
String telNr = "999";
String telNrTo = "102";
String serverAgent = "10.13.37.7";
String serverIp = "10.13.37.7";
String sipPort = "5060";

SipMachine sipMachine = SipMachine(user, pwd, telNr, serverAgent, serverIp); //esp


fs::File f;
fs::File fo;

void connectToNetwork()
{
    WiFi.begin(ssid, passphrase);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        debug_println(String(" Establishing connection to WiFi..."));
    }
    debug_println(String(" Connected to network"));
}

void setup()
{
    Serial.begin(9600);
     WiFi.begin(ssid, passphrase);
     delay(5000);
    debug_println(String(" Mac Address ") + WiFi.macAddress());
    debug_println(String(" IP Address  ") + WiFi.localIP().toString());
    debug_println(String(" Gateway IP  ") + WiFi.gatewayIP().toString());
    sipMachine.setup(WiFi.localIP().toString(), serverIp);
    SipMachine sipMachine = SipMachine(user, pwd, telNr, WiFi.gatewayIP().toString(), serverIp); //esp

    if (!SD.begin(4))
    {
        Serial.println("Card Mount Failed");
        return;
    }
    else {}
}

bool b = true;
unsigned long t = millis();
unsigned long t2 = millis();
unsigned long x = 0;
unsigned long t3 = millis();
unsigned long t4 = micros();
unsigned long t5 = millis();

SipMachine::Status status;
int16_t pcmOut=0;
int16_t pcmIn;


int16_t minP=0;
int16_t maxP=0;

void loop()
{

    pcmIn = sipMachine.loop(pcmOut);
    pcmOut=0;
    status=sipMachine.getStatus();

    switch (status)
    {
    case SipMachine::ringIn:
        // debug_println(String("Ringing Call Nr. ") + sipMachine.getTelNr());
        if (sipMachine.getTelNrIncomingCall().toInt() < 300)
        {
            debug_println(String("Accept incoming Call ") + sipMachine.getTelNrIncomingCall());
            sipMachine.acceptIncomingCall();
        }
        break;
    case SipMachine::idle:
        if ((t2 + 5000 < millis()) & (t2 + 6000 > millis()))
            if (!telNr.equals("999"))
            {
                sipMachine.makeCall(telNrTo);
            }
        if (f)
        {
            f.close();
        }
        if (fo)
        {
            fo.close();
        }
        break;
    case SipMachine::call:
        if ((t5+20000)<millis())
        {
            t5+=20000;
            sipMachine.bye();
        }
        if (t4<micros())
        {
            t4+=125;
            if (!f)
            {
                f=SD.open("/sd/Data.pcm",FILE_WRITE);

            }
            if (!fo)
            {
                fo=SD.open("/sd/wilk.pcm");
            }
            else
            {
                if (pcmIn==(688))
                {
                    pcmIn=0;
                }
                if (pcmIn>maxP)
                {
                    maxP=pcmIn;
                }
                if (pcmIn<minP)
                {
                    minP=pcmIn;
                }
                f.write((uint8_t*)&pcmIn,2);
            }

            if(fo.available()>1)
            {
                fo.read((uint8_t*)&pcmOut,2);
            }
            else
            {
                fo.close();
            }
        }
        break;
    default:
        break;
    }
    x++;

    if (millis() % 10000 == 0 && t != millis())
    {
        t = millis();

        debugL1_print(x);
        x = 0;
        debugL1_print(" min ");
        debugL1_print(String(minP));
        debugL1_print(" max ");
        debugL1_print(String(maxP));
        debugL1_print(" ");
        debugL1_print(millis());
        debugL1_print(" dtmf: ");
        debugL1_println(sipMachine.getKeyPressedLast20());
    }
}

//#ifdef __ESP_ARDU

int main()
{
    setup();
    while (true==true)
    {
        loop();
    }
}

//#endif