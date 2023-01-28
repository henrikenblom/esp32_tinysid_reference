#include <Arduino.h>
#include "sid_data.h"
#include "sid_info.h"
#include "driver/i2s.h"

#define SAMPLE_RATE 44100
#define SAMPLES_PER_BUFFER      (16 * 1024)

extern "C" bool sid_load_from_memory(void *data, size_t size, struct sid_info *info);
extern "C" void sid_synth_render(int16_t *buffer, size_t len);
extern "C" void cpuJSR(unsigned short, unsigned char);
extern "C" void sidPoke(int reg, unsigned char val);
extern "C" void c64Init(int nSampleRate);
extern "C" unsigned char memory[];

static const i2s_port_t i2s_num = I2S_NUM_0;
static const i2s_config_t i2s_config = {
        .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = (i2s_comm_format_t) (I2S_COMM_FORMAT_STAND_I2S | I2S_COMM_FORMAT_STAND_MSB),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll=false,
        .tx_desc_auto_clear= true,
        .fixed_mclk=-1
};
static const i2s_pin_config_t pin_config = {
        .bck_io_num = 27,
        .ws_io_num = 26,
        .data_out_num = 25,
        .data_in_num = I2S_PIN_NO_CHANGE
};

int16_t intermediate_buffer[SAMPLES_PER_BUFFER];
sid_info sid;

static void generate_samples() {
    int samples_rendered = 0;
    int samples_to_render = 0;

    while (samples_rendered < SAMPLES_PER_BUFFER) {
        if (samples_to_render == 0) {
            cpuJSR(sid.play_addr, 0);

            int n_refresh_cia = (int) (20000 * (memory[0xdc04] | (memory[0xdc05] << 8)) / 0x4c00);
            if ((n_refresh_cia == 0) || (sid.speed == 0))
                n_refresh_cia = 20000;

            samples_to_render = SAMPLE_RATE * n_refresh_cia / 1000000;
        }
        if (samples_rendered + samples_to_render > SAMPLES_PER_BUFFER) {
            sid_synth_render(intermediate_buffer + samples_rendered, SAMPLES_PER_BUFFER - samples_rendered);
            samples_to_render -= SAMPLES_PER_BUFFER - samples_rendered;
            samples_rendered = SAMPLES_PER_BUFFER;
        } else {
            sid_synth_render(intermediate_buffer + samples_rendered, samples_to_render);
            samples_rendered += samples_to_render;
            samples_to_render = 0;
        }
    }
}

void setup() {
    log_i("Begin setup");
    psramInit();
    i2s_driver_install(i2s_num, &i2s_config, 0, nullptr);
    i2s_set_pin(i2s_num, &pin_config);
    c64Init(SAMPLE_RATE);
    sid_load_from_memory((char *) SID_DATA, SID_DATA_SIZE, &sid);
    sidPoke(24, 15);
    cpuJSR(sid.init_addr, sid.start_song);
    log_i("End setup");
}

void loop() {
    generate_samples();
    size_t bytesWritten;
    int16_t sample[2];
    for (int16_t x: intermediate_buffer) {
        sample[0] = x;
        sample[1] = x;
        i2s_write(i2s_num, &sample, 4, &bytesWritten, 1000);
    }
}