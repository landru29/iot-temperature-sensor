#ifndef __TEMPERATURE__H__
#define __TEMPERATURE__H__


#define TEMPERATURE_PIN D6   // on pin D4 (a 4.7K resistor is necessary)


class Temperature {
  private:
    byte addr[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  public:
    Temperature(byte* address);
    float performMeasure();

    static int count;
    static Temperature** sensor;
    static void searchSensors();

};

#endif
