// Set Serial terminal to 115200 bps, 8N1, DTR; RTS

#include <Arduino.h>
#include <ETH.h>
#include <WifiServer.h>
#include <ArduinoOTA.h>

static bool eth_connected = false;

// WARNING: WiFiEvent is called from a separate FreeRTOS task (thread)!
void WiFiEvent(WiFiEvent_t event)
{
    switch (event)
    {
    case ARDUINO_EVENT_ETH_START:
        Serial.println("ETH Started");
        // The hostname must be set after the interface is started, but needs
        // to be set before DHCP, so set it from the event handler thread.
        ETH.setHostname("esp32-ethernet");
        break;
    case ARDUINO_EVENT_ETH_CONNECTED:
        Serial.println("ETH Connected");
        break;
    case ARDUINO_EVENT_ETH_GOT_IP:
        Serial.println("ETH Got IP");
        ETH.printInfo(Serial);
        eth_connected = true;
        break;
    case ARDUINO_EVENT_ETH_LOST_IP:
        Serial.println("ETH Lost IP");
        eth_connected = false;
        break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
        Serial.println("ETH Disconnected");
        eth_connected = false;
        break;
    case ARDUINO_EVENT_ETH_STOP:
        Serial.println("ETH Stopped");
        eth_connected = false;
        break;
    default:
        break;
    }
}

WiFiServer tcpServer;

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
    }
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
    while (!Serial2)
    {
    }

    WiFi.onEvent(WiFiEvent); // Will call WiFiEvent() from another thread.

    ETH.begin();

    // Port defaults to 3232
    // ArduinoOTA.setPort(3232);

    // Hostname defaults to esp3232-[MAC]
    // ArduinoOTA.setHostname("myesp32");

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    // ArduinoOTA
    //     .onStart([]()
    //              {
    //   String type;
    //   if (ArduinoOTA.getCommand() == U_FLASH)
    //     type = "sketch";
    //   else // U_SPIFFS
    //     type = "filesystem";

    //   // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    //   Serial.println("Start updating " + type); })
    //     .onEnd([]()
    //            { Serial.println("\nEnd"); })
    //     .onProgress([](unsigned int progress, unsigned int total)
    //                 { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
    //     .onError([](ota_error_t error)
    //              {
    //   Serial.printf("Error[%u]: ", error);
    //   if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    //   else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    //   else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    //   else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    //   else if (error == OTA_END_ERROR) Serial.println("End Failed"); });

    // ArduinoOTA.begin();

    tcpServer.begin(8888);
}

WiFiClient client;

void loop()
{
    // ArduinoOTA.handle();

    if (!client)
    {
        client = tcpServer.accept();
    }
    struct serialdata
    {
        static void move(Stream& in, Stream& out)
        {
            int n;
            while ((n = in.available()) > 0)
            {
                if (n > 128) n = 128;
                uint8_t buf[n];
                in.readBytes(buf, n);
                out.write(buf, n);
            }
        }
    };
    if (client)
    {
        serialdata::move(client, Serial2);
        serialdata::move(Serial2, client);
    }
    else
    {
        serialdata::move(Serial, Serial2);
        serialdata::move(Serial2, Serial);
    }
}
