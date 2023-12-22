#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#define PWM_GPIO_PIN 17  // Replace with your actual PWM GPIO pin number
#define DIR_GPIO_PIN1 20
#define PWM_GPIO_PIN2 16  // Replace with your actual PWM GPIO pin number
#define DIR_GPIO_PIN3 19
#define DIR_GPIO_PIN4 18


const uint sensor_pin = 26; 

void setup_pwm(uint gpio_pin) {
    gpio_set_function(gpio_pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio_pin);

    // Set PWM range and frequency
    pwm_set_wrap(slice_num, 1000);  // Adjust as needed
    pwm_set_clkdiv(slice_num, 125.0);  // Adjust as needed
    pwm_set_enabled(slice_num, true);
}


void setup() {

    // Initialize ADC for moisture sensor
    adc_init();
    adc_gpio_init(sensor_pin);
    adc_select_input(0); // Select ADC input (adjust if using a different ADC channel)

    // Initialize serial port for debugging
    stdio_init_all();
    setup_pwm(PWM_GPIO_PIN);
    setup_pwm(PWM_GPIO_PIN2);

    gpio_init(DIR_GPIO_PIN1);
    gpio_set_dir(DIR_GPIO_PIN1, GPIO_OUT);
     gpio_init(DIR_GPIO_PIN3);
    gpio_set_dir(DIR_GPIO_PIN3, GPIO_OUT);
     gpio_init(DIR_GPIO_PIN4);
    gpio_set_dir(DIR_GPIO_PIN4, GPIO_OUT);
}


uint normalize(uint value, uint min_input, uint max_input, uint min_output, uint max_output) {
   
    // Make sure the value is within the input range
   
    if (value > max_input) {
        value = max_input;
    }
     // Add max_input to the value if it's less than 310
    else if (value < 310) {
        value = value + max_input;
    }
     else if (value < min_input) {
        value = min_input;
    }


    // Map the input value to the output range
    return (value - min_input) * (max_output - min_output) / (max_input - min_input) + min_output;
}

int main() {
   setup();
    // Replace these with your actual pin numbers
    

    while (1) {
        gpio_put(DIR_GPIO_PIN1, 1);
        gpio_put(DIR_GPIO_PIN3, 1);
        gpio_put(DIR_GPIO_PIN4, 0);

        uint16_t analog_value = adc_read();
          // Invert sensor value to change logic
        uint16_t inverted_sensor_value = 4095 - analog_value;

        // Apply a more nuanced mapping formula for brightness
        // Example: using a quadratic scaling to increase the variation
        uint32_t scaled_value = (inverted_sensor_value * inverted_sensor_value) / 4095;

        printf("Measured Value: %u        ", scaled_value);
        
       uint normalized_analog = normalize(scaled_value, 320, 900, 10, 2000);
        printf("Normalized Value: %u\n", normalized_analog);
        pwm_set_gpio_level(PWM_GPIO_PIN, normalized_analog); 
        pwm_set_gpio_level(PWM_GPIO_PIN2, normalized_analog*1.5); 

        sleep_ms(100);
    }

    return 0;
}
