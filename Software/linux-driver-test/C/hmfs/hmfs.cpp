#include "fpga.h"

//#include <unistd.h>
#include <sys/types.h>
//#include <sys/ioctl.h>
//#include <linux/i2c-dev.h>
//#include <fcntl.h>

#include <ios>
#include <iostream>
#include <fstream>
#include <string>
//#include <sys/mman.h>
//#include "socal/socal.h"
//#include "socal/hps.h"sethmreg
//#include "socal/alt_gpio.h"

//#define SYNTHREG_OFFSET 0x00400
//#define PAGE_SIZE 4096
//volatile unsigned char *synthreg_mem;
// ///////////////////////////////////////// memory map

//#define HW_REGS_BASE ( ALT_STM_OFST )
//#define HW_REGS_SPAN ( 0x04000000 )
//#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )
#define FILE_DEV "/sys/bus/platform/drivers/hmreg/hmreg"

using namespace std;

bool m_bInitSuccess; 

FPGAFS::FPGAFS()// :
//    m_bIsVideoEnabled(false)
{
    m_bInitSuccess = Init();
    if (!m_bInitSuccess )
        cout << "FPGAFS init failed!!!" "\n";
    else
        cout << "FPGAFS init success" " ";
}

FPGAFS::~FPGAFS()
{
    close(fd);
}

bool FPGAFS::Init()
{
    bool bSuccess = false;
    fstream fileB;
    fileB.open(FILE_DEV);
    if (fileB.is_open()){
        bSuccess = true;
        fileB.close();
    }
    return bSuccess;
}

int FPGAFS::HmregSize( void ){
    int size;
    ifstream file( FILE_DEV, ios::binary | ios::ate);
    size = file.tellg(); 
    file.close();
    return size;
}

int FPGAFS::HmregReadall( char** buf){
    int count = 0; int size = HmregSize();
    char* bufpoint = new char[size];
    ifstream in( FILE_DEV, ios::binary  | ios::ate);
//    size = in.tellg();
    in.seekg(0);
    in.get(bufpoint,size);
//    while(count < size){
//        bufpoint[count++] = in.get();
//    }
    *buf = bufpoint;
    in.close();
    return size;
}

char FPGAFS::HmregGet(unsigned int addr ){
    int size = HmregSize();
    char data[size];
    if (m_bInitSuccess){
       ifstream gin( FILE_DEV, ios::binary | ios::ate);
        //        fileB.open(FILE_DEV);//
//        ifstream file;
/*        if (!fi.is_open())
        {
            std::cout << "Failed to open /sys/bus/platform/drivers/hmreg/hmreg for reading\r\n";
            return false;
        }
*/
//      cout << "reading value \n";
//        size = in.tellg(); 
        gin.seekg (0);
        gin.get(data,size);
        cout << "got value ... closing file \n";
        gin.close();
        //        value = alt_read_byte((void *) ( (u_int8_t *)synthreg_mem + ( ( uint32_t )( regaddr + SYNTHREG_OFFSET) & ( uint32_t )( HW_REGS_MASK ) )) );
    }
     return data[addr];
}


bool FPGAFS::HmregSet( unsigned int addr, int value ){
    if (!m_bInitSuccess)
        return false;
    ofstream fileB;
    fileB.open(FILE_DEV);//
    if (!fileB.is_open())
    {
        std::cout << "Failed to open /sys/bus/platform/drivers/hmreg/hmreg for writing\r\n";
        return false;
    }
    fileB.seekp(addr);
    fileB.put(value);
    fileB.close();
    //   alt_write_byte((void *) ( (u_int8_t *)synthreg_mem + ( ( uint32_t )( regadr + SYNTHREG_OFFSET) & ( uint32_t )( HW_REGS_MASK ) )) , value );
    return true;
}


