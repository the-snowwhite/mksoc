#ifndef FPGA_H
#define FPGA_H

#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>

class FPGAFS
{
public:
    FPGAFS();
    ~FPGAFS();
    int HmregSize( void );
    int HmregReadall( char** );
    char HmregGet( unsigned int addr );
    bool HmregSet(unsigned int , int);


protected:
    bool m_bInitSuccess;
    int fd;
//    bool m_bIsVideoEnabled;
//    u_int8_t *s_synthreg_base;
    bool Init();

};

#endif // FPGA_H
