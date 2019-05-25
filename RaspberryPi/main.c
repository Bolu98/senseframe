#include "gpio.h"
#include "spi.h"
#include "lora.h"
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

uint8_t buf1[8], buf2[8];

void teste1(void)
{
   memset(buf1, 0x55, sizeof(buf1));

   int fd = spi_init("/dev/spidev0.0");
   assert(fd >= 0);

   for(;;) {
      usleep(2000000);
      spi_transfer(fd, buf1, buf2, 8);
   }

   close(fd);
}

void teste2(void)
{
   int fd = gpio_open(21, 1);
   assert(fd >= 0);

   for(;;) {
      gpio_output(fd, 1);
      usleep(2000000);
      gpio_output(fd, 0);
      usleep(2000000);
   }

   close(fd);
}

void teste3(void)
{
	lora_set_pins(NULL, -1, -1, 27);
	lora_init();
	lora_set_bandwidth(500000);
	lora_set_coding_rate(5);
	lora_set_frequency(868000000);
	lora_set_preamble_length(8);
	lora_set_spreading_factor(7);
	lora_set_sync_word(52);
	lora_enable_crc();
	

   for(;;) {
      usleep(2000000);
      printf("Enviando pacote...");
      lora_send_packet("Hello", 5);
      printf("ok\n");
   }
}

#define SERVER_ADDR		0x00
#define NODE1_ADDR		0x01
#define NODE2_ADDR		0x02

#define MY_ADDR			SERVER_ADDR

void on_rx(){
		printf("Got some PACOTE mate\n");
		lora_idle();
		char buffer[32];
		lora_receive_packet(buffer, sizeof(buffer));
		// Check message destination address
		if ( (int)buffer[0] == MY_ADDR){	
			printf("--> Data: %s\n", buffer+1); // buffer+1 to skip first element
		}
		else{
			printf("--> MESSAGE DROPPED\n");
		}
		printf("\n");
		lora_sleep();
		lora_receive();
}

void teste4(void){
	lora_set_pins(NULL, -1, -1, 27);
	lora_init();
	lora_set_bandwidth(500000);
	lora_set_coding_rate(5);
	lora_set_frequency(868000000);
	lora_set_preamble_length(8);
	lora_set_spreading_factor(7);
	lora_set_sync_word(18);
	lora_enable_crc();
	
	lora_on_receive(&on_rx);
	
	lora_receive();
	int a = 0;
	for(;;a++){
		if (a%1000000000 == 0) printf("Listening for pacotes...%d\n",a);
	}
	
	
}


void teste5(void){
	lora_set_pins(NULL, -1, -1, 27);
	lora_init();
	
	//lora_dump_registers();
	
	lora_set_bandwidth(500000);
	lora_set_coding_rate(5);
	lora_set_frequency(868000000);
	lora_set_preamble_length(8);
	lora_set_spreading_factor(7);
	lora_set_sync_word(18);
	lora_enable_crc();
	lora_explicit_header_mode();
	
	lora_on_receive(&on_rx);
	lora_receive();
	for(;;){}
}


int 
main(int argc, char **argv)
{
   teste5();
   return 0;
}
