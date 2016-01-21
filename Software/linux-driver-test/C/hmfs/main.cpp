#include <iostream>
#include <sys/types.h>
#include <stdlib.h>
#include "fpga.h"


using namespace std;


int main(int argc, char *argv[])
{
    std::cout << "Input "<< argc << " Arguments::--> ";//  << " arguments:" << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i] << " ";
    }
    std::cout << std::endl;
    
    int ret = EXIT_FAILURE;
    unsigned char  value,option;
    unsigned int offset;
    int index;

    
    
    if (argc == 2) {
        option = *argv[1];
        if ( option != 'a') {
            std::cout << "Invalid option for 1 argument \n" <<
            " a as argument displays all address values \n" << 
            " r (read) + an address location displays a single location \n" << 
            " w (write) + and arddress location + writes a single location \n";
            exit(EXIT_FAILURE);
        }
    }
    
    
    if (argc >= 2) {
        option = *argv[1];
        if (option != 'w' && option != 'r' && option != 'a') {
            std::cout << "Invalid option setting. " <<
            "Option must be either r (read) or w (write)\n";
            exit(EXIT_FAILURE);
        }
    }
        
    if (argc >= 3) {
        // check the bounds of the address //
        offset = atol(argv[2]);
        if (offset > 4095) {
            std::cout  << "Invalid offset input. \n" <<
            "Address must be between 0 and 4095, inclusive.\n";
            exit(EXIT_FAILURE);
        }
        std::cout << "offset = " << offset << "\n";
        
    }
    
    if (argc == 4) {
        // check the bounds of the value being set //
        value = atoi(argv[3]);
        if (value > 255) {
            std::cout  << "Invalid number setting. " <<
            "Value must be between 0 and 255, inclusive.\n";
            exit(EXIT_FAILURE);
        }
    }

    FPGAFS fpga;
    
    if (option == 'a'){
        char* buf = NULL;
        int buffsize = fpga.HmregReadall(& buf);
        cout << "Buff Size = " << buffsize << " \n";
        int val;
        for(int j=0;j<128;j++){
            cout <<"\n" << std::dec << j*8 << "\t";
            for(int i=0;i<8;i++){
                val = buf[i+(j*8)];
                if (i==4) cout <<"\t";
                cout << i <<": " << std::dec << val  << "\t";
                cout << std::dec ;
            }
        }
        
        if(buf!=NULL)
        {
            delete[] buf;
        }
    }
    else if (option == 'r') {
        cout << "r option running \n";
        char datavalue = fpga.HmregGet(offset);
        int pval = datavalue;
        std::cout  << "Register " << offset << " Value = " << pval << " \n";
    }
    
    
//    cout << "Hmmm = \n";
    
    /*    printf("Hello\n");
    int fd, ret = EXIT_FAILURE;
    unsigned char  value,option,datavalue;
    uint16_t  offset;
    int index;
    off_t hmreg_base = (LWHPS2FPGA_BRIDGE_BASE);// + HMREG_OFFSET);
    printf("Welcome \n");

    // open the memory device file //
    fd = open("/dev/mem", O_RDWR|O_SYNC);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    // map the LWHPS2FPGA bridge into process memory //
    bridge_map = mmap(NULL, PAGE_SIZE, PROT_WRITE, MAP_SHARED,
                      fd, hmreg_base);
    if (bridge_map == MAP_FAILED) {
        perror("mmap");
        goto cleanup;
    }
    
    // get the registers peripheral's base offset //
    hmreg_mem = (unsigned char *) (bridge_map + HMREG_OFFSET);
    
    else if (option == 'w' && argc == 4){
        *(hmreg_mem + offset) = value;
        printf("Value %d written to 0x%x\n",value,offset);
    }
    else if (option == 'a'){
        for(index=0;index<256;index++) {
            datavalue = *(hmreg_mem + index);
            printf("Register 0x%x Value = 0x%x \n",index,datavalue);
        }
    }
    
    if (munmap(bridge_map, PAGE_SIZE) < 0) {
        perror("munmap");
        goto cleanup;
    }
    
    ret = 0;
    
    cleanup:
    close(fd);
    return ret;
*/
}