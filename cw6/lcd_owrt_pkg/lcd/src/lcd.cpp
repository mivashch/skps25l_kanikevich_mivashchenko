#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <cstring>
#include <cstdio>   

#define MCP3424_ADDR 0x68
#define LCD_ADDR     0x27

int open_i2c_device(const char* device, int addr) {
    int file = open(device, O_RDWR);
    if (file < 0) {
        perror("Failed to open I2C bus");
        return -1;
    }
    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        perror("Failed to connect to device");
        close(file);
        return -1;
    }
    return file;
}

int16_t read_mcp3424_channel(int file, uint8_t channel) {
    uint8_t config = 0x90 | ((channel & 0x03) << 5); // 0b10010000 | CH<<5
    write(file, &config, 1);
    usleep(5000); // wait for conversion

    uint8_t buffer[3] = {0};
    read(file, buffer, 3);

    // Combine and sign-extend 16-bit two's complement value
    int16_t value = static_cast<int16_t>((buffer[0] << 8) | buffer[1]);
    return value;
}



void lcd_send_byte(int file, uint8_t data, bool is_data) {
    uint8_t mode = is_data ? 0x01 : 0x00;
    uint8_t backlight = 0x08;
    auto write_nibble = [&](uint8_t nibble) {
        uint8_t out = (nibble & 0xF0) | mode | backlight;
        write(file, &out, 1);
        out |= 0x04; // EN=1
        write(file, &out, 1);
        out &= ~0x04; // EN=0
        write(file, &out, 1);
    };
    write_nibble(data & 0xF0);
    write_nibble((data << 4) & 0xF0);
}

void lcd_init(int file) {
    usleep(50000);
    lcd_send_byte(file, 0x33, false);
    lcd_send_byte(file, 0x32, false);
    lcd_send_byte(file, 0x28, false);
    lcd_send_byte(file, 0x0C, false);
    lcd_send_byte(file, 0x06, false);
    lcd_send_byte(file, 0x01, false);
    usleep(2000);
}

void lcd_set_cursor(int file, int line) {
    lcd_send_byte(file, 0x80 | (line == 1 ? 0x40 : 0x00), false);
}

void lcd_print(int file, const std::string& text) {
    for (char c : text) lcd_send_byte(file, c, true);
}

int main() {
    int adc = open_i2c_device("/dev/i2c-1", MCP3424_ADDR);
    int lcd = open_i2c_device("/dev/i2c-1", LCD_ADDR);

    if (adc < 0 || lcd < 0) return 1;

    lcd_init(lcd);

    while (true) {
        int16_t val1 = read_mcp3424_channel(adc, 0);
        int16_t val2 = read_mcp3424_channel(adc, 1);

        char buf1[17], buf2[17];
        snprintf(buf1, sizeof(buf1), "CH1: %5d", val1);
        snprintf(buf2, sizeof(buf2), "CH2: %5d", val2);

        lcd_set_cursor(lcd, 0);
        lcd_print(lcd, buf1);
        lcd_set_cursor(lcd, 1);
        lcd_print(lcd, buf2);

    }

    close(adc);
    close(lcd);
    return 0;
}
