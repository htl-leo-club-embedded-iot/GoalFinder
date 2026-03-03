#pragma once

#include <stdint.h>

/**
 * @file SW420.h
 * @brief Simple interface for the SW-420 vibration sensor.
 *
 * This class encapsulates the Arduino-specific logic required to read a
 * vibration sensor attached to a digital input pin. The caller may request a
 * pulse length measurement for a specified timeout and adjust an abstract
 * "sensitivity" parameter (currently only clamped, no hardware effect).
 */
class SW420 {
  public:
    /**
     * @brief Destructor.
     */
    virtual ~SW420();

    /**
     * @brief Initialize the vibration sensor hardware.
     *
     * Configures the default GPIO pin and sets its mode. Must be called
     * before performing any measurements.
     */
    void Init();

    /**
     * @brief Perform a vibration measurement.
     *
     * @param measureTimeUs Maximum time to wait for a pulse, in microseconds.
     * @return long Length of the HIGH pulse in microseconds, or 0 if the
     *         timeout expired.
     */
    long Vibration(uint64_t measureTimeUs);

    /**
     * @brief Set the (software) sensitivity level.
     *
     * The value is clamped to the 0–100 range. At present the sensitivity
     * parameter is not used by the sensor code; it exists for future
     * expansion or for callers that wish to track a desired threshold.
     *
     * @param sensitivity 0 (low) to 100 (high).
     */
    void SetSensitivity(int sensitivity);

  private:
  /** GPIO pin number used for the sensor. */
    int vs; 
};
