// Set Serial terminal to 115200 bps, 8N1, DTR; RTS

#include <Arduino.h>
#include <ETH.h>
#include <WifiServer.h>

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
    WiFi.onEvent(WiFiEvent); // Will call WiFiEvent() from another thread.
    ETH.begin();
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
    while (!Serial2)
    {
    }
    tcpServer.begin(8888);
}

WiFiClient client;

void loop()
{
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
