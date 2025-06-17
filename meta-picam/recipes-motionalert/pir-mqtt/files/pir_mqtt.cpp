#include <iostream>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <mosquitto.h>
#include <gpiod.h>

#define GPIO_CHIP     "/dev/gpiochip0"
#define PIR_LINE_NUM  12
#define MQTT_HOST     "10.100.102.4"
#define MQTT_PORT     1883
#define MQTT_TOPIC    "pir/motion"
#define CLIENT_ID     "pir_sensor"
#define MQTT_PAYLOAD_MOTION_DETECTED "MOTION_DETECTED"
#define MQTT_PAYLOAD_WAITING_FOR_MOTION "waiting_for_motion"

static volatile bool running = true;

void handle_signal(int sig)
{
    std::cout << "\nTerminating on signal " << sig << std::endl;
    running = false;
}

void mqtt_reconnect_loop(struct mosquitto *mosq)
{
    int rc;
    std::cout << "Trying to connected to MQTT broker at " << MQTT_HOST << ":" << MQTT_PORT << "\n";
    while ((rc = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 3600)) != MOSQ_ERR_SUCCESS) {
        std::cerr << "Failed to connect to MQTT broker at " << MQTT_HOST << ": " << mosquitto_strerror(rc)
                  << " — retrying in 3s...\n";
        sleep(3);
    }
    std::cout << "Connected to MQTT broker at " << MQTT_HOST << ":" << MQTT_PORT << "\n";
}

int main()
{
    signal(SIGINT, handle_signal);

    // --- MQTT Setup ---
    mosquitto_lib_init();
    struct mosquitto *mosq = mosquitto_new(CLIENT_ID, true, nullptr);
    if (!mosq) {
        std::cerr << "Failed to create MQTT client.\n";
        return 1;
    }

    mqtt_reconnect_loop(mosq);

    // --- GPIO Setup ---
    struct gpiod_chip *chip = gpiod_chip_open(GPIO_CHIP);
    if (!chip) {
        perror("gpiod_chip_open");
        return 1;
    }

    struct gpiod_line *line = gpiod_chip_get_line(chip, PIR_LINE_NUM);
    if (!line) {
        perror("gpiod_chip_get_line");
        gpiod_chip_close(chip);
        return 1;
    }

    if (gpiod_line_request_both_edges_events(line, "pir_monitor") < 0) {
        perror("gpiod_line_request_both_edges_events");
        gpiod_chip_close(chip);
        return 1;
    }

    std::cout << "Waiting for PIR motion events on GPIO" << PIR_LINE_NUM << "...\n";

    // --- Event loop ---
    struct gpiod_line_event event;

    // Define the timeout duration
    struct timespec gpio_wait_timeout;
    gpio_wait_timeout.tv_sec = 5; // 5 seconds
    gpio_wait_timeout.tv_nsec = 0; // 0 nanoseconds

    while (running) {

        int ret = gpiod_line_event_wait(line, &gpio_wait_timeout);  // Blocking wait with timeout
        if (ret < 0) {
            perror("gpiod_line_event_wait");
            break;
        } else if (ret == 0) {
            continue;  // timeout (shouldn't happen with NULL)
        }

        if (gpiod_line_event_read(line, &event) == 0) {

            if (event.event_type == GPIOD_LINE_EVENT_RISING_EDGE) {
                std::cout << "Motion detected!!! Sending MQTT alert...\n";

                int rc = mosquitto_publish(mosq, nullptr, MQTT_TOPIC,
                                           strlen(MQTT_PAYLOAD_MOTION_DETECTED), MQTT_PAYLOAD_MOTION_DETECTED,
                                           0, false);
                if (rc != MOSQ_ERR_SUCCESS) {
                    std::cerr << "MQTT publish failed: " << mosquitto_strerror(rc) << "\n";
                    if (rc == MOSQ_ERR_NO_CONN) {
                        std::cerr << "Reconnecting...\n";
                        mqtt_reconnect_loop(mosq); 
                    }
                }

            } else if (event.event_type == GPIOD_LINE_EVENT_FALLING_EDGE) {

                std::cout << "Waiting for motion... Sending MQTT alert...\n";

                int rc = mosquitto_publish(mosq, nullptr, MQTT_TOPIC,
                                           strlen(MQTT_PAYLOAD_WAITING_FOR_MOTION), MQTT_PAYLOAD_WAITING_FOR_MOTION,
                                           0, false);
                if (rc != MOSQ_ERR_SUCCESS) {
                    std::cerr << "MQTT publish failed: " << mosquitto_strerror(rc) << "\n";
                    if (rc == MOSQ_ERR_NO_CONN) {
                        std::cerr << "Reconnecting...\n";
                        mqtt_reconnect_loop(mosq);
                    }
                }
            }
        }
    }

    // --- Cleanup ---
    gpiod_line_release(line);
    gpiod_chip_close(chip);
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    std::cout << "Clean shutdown.\n";
    return 0;
}
