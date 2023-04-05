#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>

#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define I2C_DEV "/dev/i2c-2"
#define AW2028_I2C_ADDR 0x65

int fd = -1;

static uint8_t aw2028_init(void)
{
	fd = open(I2C_DEV, O_RDWR);

	if (fd < 0) {
		perror("Can't open " I2C_DEV " \n");
		exit(1);
	}

	printf("open " I2C_DEV " success !\n");

	if (ioctl(fd, I2C_SLAVE, AW2028_I2C_ADDR) < 0) {
		perror("fail to set i2c device slave address!\n");
		close(fd);
		return -1;
	}

	return 0;
}

static uint8_t i2c_write(uint8_t reg, uint8_t val)
{
	int retries;
	uint8_t data[2];

	data[0] = reg;
	data[1] = val;

	for (retries = 5; retries; retries--) {
		if (write(fd, data, 2) == 2) {
			return 0;
		}
		usleep(1000 * 10);
	}

	return -1;
}

static uint8_t i2c_read(uint8_t reg, uint8_t *val)
{
	int retries;

	for (retries = 5; retries; retries--) {
		if (write(fd, &reg, 1) == 1) {
			if (read(fd, val, 1) == 1) {
				return 0;
			}
		}
	}

	return -1;
}

unsigned char ms2timer(unsigned int time)
{
	unsigned char i = 0;
	unsigned int ref[16] = {4,	  128,	256,  384,	512,  762,	1024, 1524,
							2048, 2560, 3072, 4096, 5120, 6144, 7168, 8192};

	for (i = 0; i < 15; i++) {
		if (time <= ref[0]) {
			return 0;
		} else if (time > ref[15]) {
			return 15;
		} else if ((time > ref[i]) && (time <= ref[i + 1])) {
			if ((time - ref[i]) <= (ref[i + 1] - time)) {
				return i;
			} else {
				return (i + 1);
			}
		}
	}
	return 0;
}

unsigned char AW2028_LED_ON(unsigned char r, unsigned char g, unsigned char b)
{
	i2c_write(0x00, 0x55);	// software reset

	i2c_write(0x01, 0x01);	// GCR
	i2c_write(0x03, 0x01);	// IMAX
	i2c_write(0x04, 0x00);	// LCFG1
	i2c_write(0x05, 0x00);	// LCFG2
	i2c_write(0x06, 0x00);	// LCFG3
	i2c_write(0x07, 0x07);	// LEDEN

	i2c_write(0x10, r);		// ILED1
	i2c_write(0x11, g);		// ILED2
	i2c_write(0x12, b);		// ILED3
	i2c_write(0x1C, 0xFF);	// PWM1
	i2c_write(0x1D, 0xFF);	// PWM2
	i2c_write(0x1E, 0xFF);	// PWM3

	return 0;
}

unsigned char AW2028_LED_OFF(void)
{
	i2c_write(0x00, 0x55);	// software reset
	return 0;
}

unsigned char AW2028_LED_Blink(unsigned char r, unsigned char g,
							   unsigned char b, unsigned int trise_ms,
							   unsigned int ton_ms, unsigned int tfall_ms,
							   unsigned int toff_ms)
{
	unsigned char trise, ton, tfall, toff;

	trise = ms2timer(trise_ms);
	ton = ms2timer(ton_ms);
	tfall = ms2timer(tfall_ms);
	toff = ms2timer(toff_ms);

	i2c_write(0x00, 0x55);	// software reset

	i2c_write(0x01, 0x01);	// GCR
	i2c_write(0x03, 0x01);	// IMAX
	i2c_write(0x04, 0x01);	// LCFG1
	i2c_write(0x05, 0x01);	// LCFG2
	i2c_write(0x06, 0x01);	// LCFG3
	i2c_write(0x07, 0x07);	// LEDEN
	i2c_write(0x08, 0x08);	// LEDCTR

	i2c_write(0x10, r);		// ILED1
	i2c_write(0x11, g);		// ILED2
	i2c_write(0x12, b);		// ILED3
	i2c_write(0x1C, 0xFF);	// PWM1
	i2c_write(0x1D, 0xFF);	// PWM2
	i2c_write(0x1E, 0xFF);	// PWM3

	i2c_write(0x30, (trise << 4) | ton);   // PAT_T1               Trise & Ton
	i2c_write(0x31, (tfall << 4) | toff);  // PAT_T2               Tfall & Toff
	i2c_write(0x32, 0x00);	// PAT_T3                               Tdelay
	i2c_write(0x33, 0x00);	// PAT_T4         PAT_CTR & Color
	i2c_write(0x34, 0x00);	// PAT_T5                   Timer

	i2c_write(0x09, 0x07);	// PAT_RIN
}

unsigned char AW2028_Audio_Corss_Zero(void)
{
	i2c_write(0x00, 0x55);	// software reset

	i2c_write(0x01, 0x01);	// GCR
	i2c_write(0x03, 0x01);	// IMAX
	i2c_write(0x07, 0x07);	// LEDEN
	i2c_write(0x10, 0xFF);	// ILED1
	i2c_write(0x11, 0xFF);	// ILED2
	i2c_write(0x12, 0xFF);	// ILED3
	i2c_write(0x1C, 0xFF);	// PWM1
	i2c_write(0x1D, 0xFF);	// PWM2
	i2c_write(0x1E, 0xFF);	// PWM3

	i2c_write(0x40, 0x11);	// AUDIO_CTR
	i2c_write(0x41, 0x07);	// AUDIO_LEDEN
	i2c_write(0x42, 0x00);	// AUDIO_FLT
	i2c_write(0x43, 0x1A);	// AGC_GAIN
	i2c_write(0x44, 0x1F);	// GAIN_MAX
	i2c_write(0x45, 0x3D);	// AGC_CFG
	i2c_write(0x46, 0x14);	// ATTH
	i2c_write(0x47, 0x0A);	// RLTH
	i2c_write(0x48, 0x00);	// NOISE
	i2c_write(0x49, 0x02);	// TIMER
	i2c_write(0x40, 0x13);	// AUDIO_CTR

	return 0;
}

unsigned char AW2028_Audio_Timer(void)
{
	i2c_write(0x00, 0x55);	// software reset

	i2c_write(0x01, 0x01);	// GCR
	i2c_write(0x03, 0x01);	// IMAX
	i2c_write(0x07, 0x07);	// LEDEN
	i2c_write(0x10, 0xFF);	// ILED1
	i2c_write(0x11, 0xFF);	// ILED2
	i2c_write(0x12, 0xFF);	// ILED3
	i2c_write(0x1C, 0xFF);	// PWM1
	i2c_write(0x1D, 0xFF);	// PWM2
	i2c_write(0x1E, 0xFF);	// PWM3

	i2c_write(0x40, 0x11);	// AUDIO_CTR
	i2c_write(0x41, 0x07);	// AUDIO_LEDEN
	i2c_write(0x42, 0x00);	// AUDIO_FLT
	i2c_write(0x43, 0x1A);	// AGC_GAIN
	i2c_write(0x44, 0x1F);	// GAIN_MAX
	i2c_write(0x45, 0x3D);	// AGC_CFG
	i2c_write(0x46, 0x14);	// ATTH
	i2c_write(0x47, 0x0A);	// RLTH
	i2c_write(0x48, 0x00);	// NOISE
	i2c_write(0x49, 0x00);	// TIMER
	i2c_write(0x40, 0x0B);	// AUDIO_CTR

	return 0;
}

unsigned char AW2028_Audio(unsigned char mode)
{
	if (mode > 5) {
		mode = 0;
	}
	i2c_write(0x00, 0x55);	// software reset

	i2c_write(0x01, 0x01);	// GCR
	i2c_write(0x03, 0x01);	// IMAX
	i2c_write(0x07, 0x07);	// LEDEN
	i2c_write(0x10, 0xFF);	// ILED1
	i2c_write(0x11, 0xFF);	// ILED2
	i2c_write(0x12, 0xFF);	// ILED3
	i2c_write(0x1C, 0xFF);	// PWM1
	i2c_write(0x1D, 0xFF);	// PWM2
	i2c_write(0x1E, 0xFF);	// PWM3

	i2c_write(0x40, (mode << 3) | 0x01);  // AUDIO_CTR
	i2c_write(0x41, 0x07);				  // AUDIO_LEDEN
	i2c_write(0x42, 0x00);				  // AUDIO_FLT
	i2c_write(0x43, 0x1A);				  // AGC_GAIN
	i2c_write(0x44, 0x1F);				  // GAIN_MAX
	i2c_write(0x45, 0x3D);				  // AGC_CFG
	i2c_write(0x46, 0x14);				  // ATTH
	i2c_write(0x47, 0x0A);				  // RLTH
	i2c_write(0x48, 0x00);				  // NOISE
	i2c_write(0x49, 0x00);				  // TIMER
	i2c_write(0x40, (mode << 3) | 0x03);  // AUDIO_CTR

	return 0;
}

int main(int argc, char **argv)
{
	if (aw2028_init() != 0) {
		exit(-1);
	}
	AW2028_LED_OFF();
	// AW2028_LED_ON(255, 255, 255);
	// AW2028_Audio(1);
	// AW2028_Audio_Corss_Zero();
	// AW2028_Audio_Timer();
	AW2028_Audio(3);
	// AW2028_LED_ON(255, 255, 255);
	// AW2028_LED_Blink(255,255,255, 10, 7, 10, 2);
	while (1) {
		AW2028_LED_Blink(255, 255, 255, 1000, 0, 1000, 1000);
		sleep(2);
		AW2028_LED_Blink(0, 255, 255, 1000, 0, 1000, 1000);
		sleep(2);
		AW2028_LED_Blink(255, 0, 255, 1000, 0, 1000, 1000);
		sleep(2);
		AW2028_LED_Blink(255, 255, 0, 1000, 0, 1000, 1000);
		sleep(2);
		AW2028_LED_Blink(0, 0, 0, 1000, 0, 1000, 1000);
		sleep(2);
	}
	return 0;
}