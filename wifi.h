#ifndef __WIFI_CONNECT__H__
#define __WIFI_CONNECT__H__

class WifiConnect {
  public:
    WifiConnect();
    bool startWPSPBC();
    bool connectWithCred(char* ssid, char* password);
    void disconnect();

};

#endif
