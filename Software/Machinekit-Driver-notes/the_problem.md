1. all needed info is passed thru the device tree. (Size).

2. the-snowwhite-git/machinekit-iotest/src/rtapi/socfpgadrv/hm2reg_uio-module.c
or:
the-snowwhite-git/mksoc-repo/mksoc/Software/hm2reg_uio-module/hm2reg_uio-module.c

kernel driver module that when compiled and included, after
insmod hm2reg_uio-module.ko
will create /dev/uio0 entity.

3.
Following driver example will provide memory access to the ipcore:

User Space Driver Example:

requirement(s):

    #define UIO_SIZE "/sys/class/uio/uio0/maps/map0/size"

setup:

    int main( int argc, char **argv ) {
    int uio_fd;
    unsigned int uio_size;
    FILE *size_fp;
    void *base_address;
    uio_fd = open( “/dev/uio0”, O_RDWR);
    size_fp = fopen(UIO_SIZE, O_RDONLY);
    fscanf(size_fp, “0x%08X”, &uio_size);
    base_address = mmap(NULL, uio_size,
    PROT_READ | PROT_WRITE,
    MAP_SHARED, uio_fd, 0);

    // Access to the hardware can now occur....

cleanup:

    munmap(base_address, uio_size);

    }

----

the machinekit part I need help to implement:


llio data structure:
(the info requested here also resides in the hostmot2 rom so why can't it just be autoprobed ?')

    typedef struct {
        hm2_lowlevel_io_t *llio;
    
        struct {
            int num_encoders;
            int num_absencs;
            struct list_head absenc_formats;
            int num_resolvers;
            int num_pwmgens;
            int num_tp_pwmgens;
            int num_stepgens;
            int stepgen_width;
            int num_leds;
            int num_sserials;
            int num_bspis;
            int num_uarts;
            int num_dplls;
            char sserial_modes[4][8];
            int enable_raw;
            char *firmware;
        } config;
    
        char config_name[HM2_CONFIGNAME_LENGTH + 1];
        u16 idrom_offset;
    
        hm2_idrom_t idrom;
    
        hm2_module_descriptor_t md[HM2_MAX_MODULE_DESCRIPTORS];
        int num_mds;
    
        int dpll_module_present;
        int use_serial_numbers;
    
        hm2_pin_t *pin;
        int num_pins;
    
        // this keeps track of all the tram entries
        struct list_head tram_read_entries;
        u32 *tram_read_buffer;
        u16 tram_read_size;
    
        struct list_head tram_write_entries;
        u32 *tram_write_buffer;
        u16 tram_write_size;
    
        // the hostmot2 "Functions"
        hm2_encoder_t encoder;
        hm2_absenc_t absenc;
        hm2_resolver_t resolver;
        hm2_pwmgen_t pwmgen;
        hm2_tp_pwmgen_t tp_pwmgen;
        hm2_stepgen_t stepgen;
        hm2_sserial_t sserial;
        hm2_bspi_t bspi;
        hm2_uart_t uart;
        hm2_ioport_t ioport;
        hm2_watchdog_t watchdog;
        hm2_dpll_t dpll;
        hm2_led_t led;
    
        hm2_raw_t *raw;
    
        struct list_head list;
    } hostmot2_t;


4. Machinekit setup:

        int rtapi_app_main(void) {
        int r = 0;
    
        comp_id = hal_init(HM2_LLIO_NAME);
        if (comp_id < 0) return comp_id;
    
        r = hm2_cvsocfpga_setup();
        if (r) {
            hm2_cvsocfpga_cleanup();
            hal_exit(comp_id);
        } else {
            hal_ready(comp_id);
        }
    
        return r;
    }

    static int hm2_cvsocfpga_setup(void) {
        int i;
    
        LL_PRINT("loading HostMot2 Mesa 5i25 driver version %s\n", HM2_CVSOCFPGA_VERSION);
    
        // zero the ipcore structs
        memset(ipcore, 0, HM2_CVSOCFPGA_MAX_IPCORES * sizeof(hm2_cvsocfpga_t));
        num_ipcores = 0;

        uio_fd = open( “/dev/uio0”, O_RDWR);
        size_fp = fopen(UIO_SIZE, O_RDONLY);
        fscanf(size_fp, “0x%08X”, &uio_size);
        base_address = mmap(NULL, uio_size,
                        
            
        hm2_lowlevel_io_t *this;
        int r;
        
        rtapi_snprintf(ipcore[i].llio.name, sizeof(ipcore[i].llio.name), "%s.%d", HM2_LLIO_NAME, i);
        ipcore[i].llio.comp_id = comp_id;
        
        ipcore[i].llio.read = hm2_cvsocfpga_read;
        ipcore[i].llio.write = hm2_cvsocfpga_write;
  
        // all the info here can be loaded from the hm2 rom instead of written in here.
        // it can also be added to the device tree data if it needs to be provided before accessing the hoastmot2 rom is possible.
  
        ipcore[i].llio.num_ioport_connectors = 2;
        ipcore[i].llio.pins_per_connector = 17;
        ipcore[i].llio.ioport_connector_name[0] = "P3";
        ipcore[i].llio.ioport_connector_name[1] = "P2";
        ipcore[i].llio.fpga_part_number = "6slx9tqg144";
        ipcore[i].llio.num_leds = 2;
        ipcore[i].llio.private = &ipcore[i];
        
        this = &ipcore[i].llio;
        
        r = hm2_register(&ipcore[i].llio, config[i]);
        if (r != 0) {
            THIS_ERR(
                   "ipcore setup error!\n"
                )
            );
        }
        
        THIS_PRINT(
            "ipcore loaded ok\n"
            )
        );
        return 0;
    }
    
    
    void rtapi_app_exit(void) {
        hm2_cvsocfpga_cleanup();
        hal_exit(comp_id);
        LL_PRINT("driver unloaded\n");
    }

suggested dts data structure to replace the pci data struct
(holds all data currently delivered in the device tree)

    static struct dts_device_id hm2_cvsoc_tbl[] = {
        // 5i25
        {   
            .address_width = 14,        //0x0000000E
            .clocks = 2,                //0x00000002 number of clocks ?
            .compatible {
                .vendor = machkt,       //6D 61 63 68 6B 74 2C
                .name = hm2reg-io,      //68 6D 32 72 65 67 2D 69 6F 2D 31 2E 30 00
            } // (machkt,hm2reg-io-1.0.)
        .data_width = 32,           // 0x00000020
        .name = hm2-socfpga,        //68 6D 32 2D 73 6F 63 66 70 67 61 00 (hm2-socfpga.)
        .reg = 0x00000001, 0x00040000, 0x00010000,// ?, address offset from bridge, address span (= max address +1)
        },
        {0,},
    };

