 
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "../Include/mkhm2soc/hps_0.h"

#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

//------------------   /dev/mem test ------------------//


int main ( void )
{
    void *virtual_base;
    void *h2p_lw_axi_mem_addr=NULL;
    int fd;
    uint32_t value[4];
    uint32_t index;
    printf("Hello \n");
    
    // Open /dev/mem
    if ( ( fd = open ( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
        printf ( "ERROR: could not open \"/dev/mem\"...\n" );
        return ( 1 );
    }
    printf("/dev/mem opened fine OK \n");
    
    // get virtual addr that maps to physical
    virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, HW_REGS_BASE);

    if ( virtual_base == MAP_FAILED ) {
        printf ( "ERROR: mmap() failed...\n" ); 
        
        close ( fd );
        return ( 1 );
    }
    printf("region mmap'ed  OK \n");
    
    // Get the address that maps to the LEDs
    h2p_lw_axi_mem_addr=virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + HM2REG_IO_0_BASE ) & ( unsigned long)( HW_REGS_MASK ) );

    printf("mem pointer created OK\n");
    
    // print val:
//    value = *(uint32_t *)h2p_lw_axi_mem_addr;
//    printf("value = %x  \n",value );
    int bytes[4][4];
    for(index=0;index<4096;index = index+4) {
        for(int j=0;j<4;j++) {
            value[j] = *((uint32_t *)(h2p_lw_axi_mem_addr + index+j));
            for(int i=0;i<4;i++) {
                bytes[0][i] = (value[j] >> 24) & 0xFF;
                bytes[1][i] = (value[j] >> 16) & 0xFF;
                bytes[2][i] = (value[j] >> 8) & 0xFF;
                bytes[3][i] = value[j] & 0xFF;
            }
 
        }        
        printf("0x%x | %08x  %08x  %08x  %08x  |\t %c%c%c%c %c%c%c%c %c%c%c%c %c%c%c%c |\n",
               index,value[0],value[1],value[2],value[3],
               bytes[0][0],bytes[1][0],bytes[2][0],bytes[3][0],
               bytes[0][1],bytes[1][1],bytes[2][1],bytes[3][1],
               bytes[0][2],bytes[1][2],bytes[2][2],bytes[3][2],
               bytes[0][3],bytes[1][3],bytes[2][3],bytes[3][3]
        );
    }
    
    if ( munmap ( virtual_base, HW_REGS_SPAN ) != 0 ) {
        printf ( "ERROR: munmap() failed...\n" );
        close ( fd );
        return ( 1 );

    }

    close ( fd );

    return 0;
}