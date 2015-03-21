
// Example of initializing and using the random number generator.

#include <Crypto.h>
#include <RNG.h>
#include <TransistorNoiseSource.h>
#include <RingOscillatorNoiseSource.h>

// Change "MyApp 1.0" to some other tag for your application
// so that different applications will generate different results
// even if the input noise or seed data is otherwise identical.
#define RNG_APP_TAG "MyApp 1.0"

// EEPROM address to save the random number seed at.
#define RNG_EEPROM_ADDRESS 500

// Noise source to seed the random number generator.
TransistorNoiseSource noise(A1);
//RingOscillatorNoiseSource noise;

bool calibrating = false;
byte data[32];
unsigned long startTime;

void setup() {
    Serial.begin(9600);
    Serial.println("start");

    // Initialize the random number generator.
    RNG.begin(RNG_APP_TAG, RNG_EEPROM_ADDRESS);

    startTime = millis();
}

void printHex(const byte *data, unsigned len)
{
    static char const hexchars[] = "0123456789ABCDEF";
    unsigned long time = millis() - startTime;
    Serial.print(time / 1000);
    Serial.print('.');
    Serial.print((time / 100) % 10);
    Serial.print(": ");
    while (len > 0) {
        int b = *data++;
        Serial.print(hexchars[(b >> 4) & 0x0F]);
        Serial.print(hexchars[b & 0x0F]);
        --len;
    }
    Serial.println();
}

void loop() {
    // Track changes to the calibration state on the noise source.
    bool newCalibrating = noise.calibrating();
    if (newCalibrating != calibrating) {
        calibrating = newCalibrating;
        if (calibrating)
            Serial.println("calibrating");
    }

    // If the noise source has accumulated new entropy, then stir it in.
    RNG.stir(noise);

    // Perform regular housekeeping on the random number generator.
    RNG.loop();

    // Generate output whenever 32 bytes of entropy have been accumulated.
    if (RNG.available(sizeof(data))) {
        RNG.rand(data, sizeof(data));
        printHex(data, sizeof(data));
    }
}