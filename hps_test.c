#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>  

#define PIO_SIZE    0x0000000F
#define LED_0_BASE 0xFF203000 
#define GPIO_0_BASE 0xFF206000 
#define SWTCH_BASE 0xFF204000 

int main()
{
    int filedes;
    int i;
    // Open memory
    if( ( filedes = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return( 1 );
	}

    void *gpio0;
    void *led;
    void *swtch;
    // Map memory
    //void * mmap (void *address, size_t length, int protect, int flags, int filedes,off_t offset)
    gpio0 = mmap( NULL, PIO_SIZE, ( PROT_READ | PROT_WRITE ), MAP_SHARED, filedes, GPIO_0_BASE );
    led = mmap( NULL, PIO_SIZE, ( PROT_READ | PROT_WRITE ), MAP_SHARED, filedes, LED_0_BASE );
    swtch = mmap( NULL, PIO_SIZE, ( PROT_READ | PROT_WRITE ), MAP_SHARED, filedes, SWTCH_BASE );
    int switchvalue;
    for(i=0;i<32;i++)
    {
        *(uint32_t *)gpio0 = 0xFF;
        *(uint32_t *)led = 0xFF;
        usleep(500000);
        *(uint32_t *)gpio0 = 0x00;
        *(uint32_t *)led = 0x00;
        usleep(500000);
        switchvalue = *(uint32_t *)swtch;
        printf("Switch value: %d\n",switchvalue);
        printf("%d\n",i);
    }

    // un map memory
    if( munmap( gpio0, PIO_SIZE ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( filedes );
		return( 1 );
	}

    if( munmap( led, PIO_SIZE ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( filedes );
		return( 1 );
	}

    if( munmap( swtch, PIO_SIZE ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( filedes );
		return( 1 );
	}
    
    // Close memory
    close( filedes );
    printf( "Done\n" );
    return 0;
}