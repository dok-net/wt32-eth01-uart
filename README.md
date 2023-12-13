This is an Arduino Core for ESP32 sketch to build your own firmware for the WT32-ETH01 boards.

In order to support full HW reset of the PHY, a hardware modification of WT32-ETH01 at least up to version 1.4 can be performed:

Connect IO16_OSC_EN, hence EN of OSC50MHZ, to nRST of the RTL8720 PHY.

Steps:
- Completely remove R43
- From the lead between C18 and R43 a connection to the lead between R50 and OSC50MHZ EN gets soldered

The finished modification should be like this ![photo of the result](wt32-eth01-phy-rst.jpg "modded WT32-ETH01 V1.4")
