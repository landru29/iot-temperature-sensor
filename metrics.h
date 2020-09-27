#ifndef __HTTP__H__
#define __HTTP__H__

typedef struct Sensor Sensor;
struct Sensor
{
    float value;
    long timestamp;
};

class Metrics {
  public:
    Metrics(char* sensorName);
    void addValue(float val, long timestamp);
    void flushData();
    void addValueAndFlush(float val, long timestamp);
    int getBufferSize();

  private:
    Sensor* sensor;
    int bufferSize;
    char* sensorName;

    static const char* url;
};

#endif
