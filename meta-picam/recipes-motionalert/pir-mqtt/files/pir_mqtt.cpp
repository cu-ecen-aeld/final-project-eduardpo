// pir_mqtt.cpp without TLS support

#include <iostream>
#include <fstream>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <getopt.h>
#include <mosquitto.h>
#include <gpiod.h>
#include <chrono>
#include <ctime>
#include <iomanip> // for std::put_time

#define MQTT_PAYLOAD_MOTION_DETECTED "motion_detected"

static volatile bool running = true;

std::string mqtt_host = "10.100.102.4";
int mqtt_port = 1883;
std::string mqtt_topic = "pir/motion";
int gpio_line = 12;
std::string log_file = "/var/log/pir_mqtt.log";


void log_event(const std::string& msg) {
    std::ofstream log(log_file, std::ios_base::app);
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    log << std::put_time(std::localtime(&now), "%c") << ": " << msg << std::endl;
}

void handle_signal(int sig) {
    log_event("Exiting on signal " + std::to_string(sig));
    running = false;
}

void mqtt_reconnect_loop(struct mosquitto *mosq) {
    while (mosquitto_connect(mosq, mqtt_host.c_str(), mqtt_port, 60) != MOSQ_ERR_SUCCESS) {
        log_event("MQTT reconnect to host '" + mqtt_host + "' failed, retrying in 3s...");
        sleep(3);
    }
    log_event("Connected to MQTT broker at " + mqtt_host + ":" + std::to_string(mqtt_port));
}

void parse_config(const std::string &path) {
    std::ifstream file(path);
    std::string line;
    while (getline(file, line)) {
        if (line.find("gpio=") == 0) gpio_line = std::stoi(line.substr(5));
        else if (line.find("host=") == 0) mqtt_host = line.substr(5);
        else if (line.find("port=") == 0) mqtt_port = std::stoi(line.substr(5));
        else if (line.find("topic=") == 0) mqtt_topic = line.substr(6);
        else if (line.find("logfile=") == 0) log_file = line.substr(8);
    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, handle_signal);

    std::string config_path;
    int opt;
    while ((opt = getopt(argc, argv, "g:h:p:t:c:l:")) != -1) {
        switch (opt) {
            case 'g': gpio_line = std::stoi(optarg); break;
            case 'h': mqtt_host = optarg; break;
            case 'p': mqtt_port = std::stoi(optarg); break;
            case 't': mqtt_topic = optarg; break;
            case 'c': config_path = optarg; break;
            case 'l': log_file = optarg; break;
            default:
                std::cerr << "Usage: " << argv[0] << " [-c config] [-g gpio] [-h host] [-p port] [-t topic] [-l logfile]\n";
                return 1;
        }
    }

    if (!config_path.empty()) parse_config(config_path);

    mosquitto_lib_init();
    struct mosquitto *mosq = mosquitto_new(nullptr, true, nullptr);
    if (!mosq) {
        log_event("Failed to create mosquitto client");
        return 1;
    }

    int protocol_version = MQTT_PROTOCOL_V311;
    mosquitto_opts_set(mosq, MOSQ_OPT_PROTOCOL_VERSION, &protocol_version);
    mosquitto_reconnect_delay_set(mosq, 2, 10, false);

    mqtt_reconnect_loop(mosq);

    struct gpiod_chip *chip = gpiod_chip_open("/dev/gpiochip0");
    struct gpiod_line *line = gpiod_chip_get_line(chip, gpio_line);
    gpiod_line_request_both_edges_events(line, "pir_mqtt");

    struct gpiod_line_event event;
    int motion_count = 0;
    log_event("Monitoring started on GPIO" + std::to_string(gpio_line));

    auto last_ping = std::chrono::steady_clock::now();

    while (running) {
        struct timespec timeout = {0, 100000000}; // 100 ms
        int ret = gpiod_line_event_wait(line, &timeout);
        if (ret == 1 && gpiod_line_event_read(line, &event) == 0 &&
            event.event_type == GPIOD_LINE_EVENT_RISING_EDGE) {

            motion_count++;
            log_event("Motion detected (#" + std::to_string(motion_count) + ")");
            int rc = mosquitto_publish(mosq, nullptr, mqtt_topic.c_str(),
                                       strlen(MQTT_PAYLOAD_MOTION_DETECTED), MQTT_PAYLOAD_MOTION_DETECTED, 0, false);
            if (rc != MOSQ_ERR_SUCCESS)
                log_event("MQTT publish failed: " + std::string(mosquitto_strerror(rc)));
        }

        // Maintain MQTT connection
        mosquitto_loop_misc(mosq);
        mosquitto_loop_write(mosq, 1);
        mosquitto_loop_read(mosq, 1);
    }

    mosquitto_disconnect(mosq);
    gpiod_line_release(line);
    gpiod_chip_close(chip);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    log_event("Shutdown complete. Total motions detected: " + std::to_string(motion_count));
    return 0;
}
