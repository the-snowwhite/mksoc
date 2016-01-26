//#--------------------------------------------------------------------------------#//
// hostmot2-lowlevel.h
// 
// this struct holds a HostMot2 instance
//

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

//#--------------------------------------------------------------------------------------------#//
//
// test template includefile
//

#define HM2_LLIO_NAME "hm2_test"

#define HM2_TEST_MAX_BOARDS (2)

typedef struct {
    union {
        u8 tp8[64 * 1024];
        u32 tp32[16 * 1024];
    } test_pattern;
    
    hm2_lowlevel_io_t llio;
} hm2_test_t;

//
// The Mesa 7i90 /7i43 struct
//

typedef struct {
    hal_parport_t port;
    int epp_wide;
    
    hm2_lowlevel_io_t llio;
} hm2_7i90_t;

// or
// The Mesa pci-cards struct
//

typedef struct {
    struct pci_dev *dev;
    void __iomem *base;
    int len;
    unsigned long ctrl_base_addr;
    unsigned long data_base_addr;
    hm2_lowlevel_io_t llio;
} hm2_pci_t;


//#--------------------------------------------------------------------------------------------#//

//hm2_7i90.c:
// http://store.mesanet.com/index.php?route=product/product&filter_name=7i90&product_id=291
// 7I90HD Parallel/SPI Anything I/O card
// The MESA 7I90HD is a very low cost, general purpose, FPGA based programmable I/O card with a EPP ..
// these are the low-level i/o functions exported to the hostmot2 driver
//

int hm2_7i90_read(hm2_lowlevel_io_t *this, u32 addr, void *buffer, int size) {
    int bytes_remaining = size;
    hm2_7i90_t *board = this->private;
    
    hm2_7i90_epp_addr16(addr | HM2_7I90_ADDR_AUTOINCREMENT, board);
    
    for (; bytes_remaining > 3; bytes_remaining -= 4) {
        *((u32*)buffer) = hm2_7i90_epp_read32(board);
        buffer += 4;
    }
    
    for ( ; bytes_remaining > 0; bytes_remaining --) {
        *((u8*)buffer) = hm2_7i90_epp_read(board);
        buffer ++;
    }
    
    if (hm2_7i90_epp_check_for_timeout(board)) {
        THIS_PRINT("EPP timeout on data cycle of read(addr=0x%04x, size=%d)\n", addr, size);
        (*this->io_error) = 1;
        this->needs_reset = 1;
        hm2_7i90_epp_clear_timeout(board);
        return 0;  // fail
    }
    
    return 1;  // success
}


int hm2_7i90_write(hm2_lowlevel_io_t *this, u32 addr, void *buffer, int size) {
    int bytes_remaining = size;
    hm2_7i90_t *board = this->private;
    
    hm2_7i90_epp_addr16(addr | HM2_7I90_ADDR_AUTOINCREMENT, board);
    
    for (; bytes_remaining > 3; bytes_remaining -= 4) {
        hm2_7i90_epp_write32(*((u32*)buffer), board);
        buffer += 4;
    }
    
    for ( ; bytes_remaining > 0; bytes_remaining --) {
        hm2_7i90_epp_write(*((u8*)buffer), board);
        buffer ++;
    }
    
    if (hm2_7i90_epp_check_for_timeout(board)) {
        THIS_PRINT("EPP timeout on data cycle of write(addr=0x%04x, size=%d)\n", addr, size);
        (*this->io_error) = 1;
        this->needs_reset = 1;
        hm2_7i90_epp_clear_timeout(board);
        return 0;
    }
    
    return 1;
}

//#--------------------------------------------------------------------------------#//
//# Functions exported;
//#--------------------------------------------------------------------------------#//

static void hm2_7i90_cleanup(void) {
    int i;
    
    // NOTE: hal_malloc() doesnt have a matching free
    
    for (i = 0; i < num_boards; i ++) {
        hm2_lowlevel_io_t *this = &board[i].llio;
        THIS_PRINT("releasing board\n");
        hm2_unregister(this);
        hal_parport_release(&board[i].port);
    }
}

static int hm2_7i90_setup(void) {
    int i;
    
    LL_PRINT("loading HostMot2 Mesa 7i90 driver version %s\n", HM2_7I90_VERSION);
    
    // zero the board structs
    memset(board, 0, HM2_7I90_MAX_BOARDS * sizeof(hm2_7i90_t));
    num_boards = 0;
    
    for (i = 0; i < HM2_7I90_MAX_BOARDS; i ++) {
        if(!config[i] || !*config[i]) break;
        
        hm2_lowlevel_io_t *this;
        int r;
        
        board[i].epp_wide = epp_wide[i];
        
        //
        // claim the I/O regions for the parport
        //
        
        r = hal_parport_get(comp_id, &board[i].port,
                            ioaddr[i], ioaddr_hi[i], PARPORT_MODE_EPP);
        if(r < 0)
            return r;
        
        // set up the parport for EPP
        if(board[i].port.base_hi) {
            outb(0x94, board[i].port.base_hi + HM2_7I90_ECP_CONTROL_HIGH_OFFSET); // select EPP mode in ECR
        }
        
        //
        // when we get here, the parallel port is in epp mode and ready to go
        //
        
        // select the device and tell it to make itself ready for io
        hm2_7i90_epp_write_control(0x04, &board[i]);  // set control lines and input mode
        hm2_7i90_epp_clear_timeout(&board[i]);
        
        rtapi_snprintf(board[i].llio.name, sizeof(board[i].llio.name), "%s.%d", HM2_LLIO_NAME, i);
        board[i].llio.comp_id = comp_id;
        
        board[i].llio.read = hm2_7i90_read;
        board[i].llio.write = hm2_7i90_write;
        
        board[i].llio.num_ioport_connectors = 3;
        board[i].llio.pins_per_connector = 24;
        board[i].llio.ioport_connector_name[0] = "P1";
        board[i].llio.ioport_connector_name[1] = "P2";
        board[i].llio.ioport_connector_name[2] = "P3";
        board[i].llio.num_leds = 2;
        board[i].llio.private = &board[i];
        
        this = &board[i].llio;
        
        
        
        
        //  select CPLD data register
        hm2_7i90_epp_addr8(0, &board[i]);
        
        if (hm2_7i90_epp_read(&board[i]) & 0x01) {
            board[i].llio.fpga_part_number = "3s400tq144";
        } else {
            board[i].llio.fpga_part_number = "3s200tq144";
        }
        THIS_DBG("detected FPGA '%s'\n", board[i].llio.fpga_part_number);
        
        
        r = hm2_register(&board[i].llio, config[i]);
        if (r != 0) {
            THIS_ERR(
                "board at (ioaddr=0x%04X, ioaddr_hi=0x%04X, epp_wide %s) not found!\n",
                     board[i].port.base,
                     board[i].port.base_hi,
                     (board[i].epp_wide ? "ON" : "OFF")
            );
            
            hal_parport_release(&board[i].port);
            return r;
        }
        
        THIS_PRINT(
            "board at (ioaddr=0x%04X, ioaddr_hi=0x%04X, epp_wide %s) found\n",
                   board[i].port.base,
                   board[i].port.base_hi,
                   (board[i].epp_wide ? "ON" : "OFF")
        );
        
        num_boards ++;
    }
    
    return 0;
}

int rtapi_app_main(void) {
    int r = 0;
    
    comp_id = hal_init(HM2_LLIO_NAME);
    if (comp_id < 0) return comp_id;
    
    r = hm2_7i90_setup();
    if (r) {
        hm2_7i90_cleanup();
        hal_exit(comp_id);
    } else {
        hal_ready(comp_id);
    }
    
    return r;
}

void rtapi_app_exit(void) {
    hm2_7i90_cleanup();
    hal_exit(comp_id);
    LL_PRINT("driver unloaded\n");
}
//#--------------------------------------------------------------------------------------------#//
//#  llio names:
//#--------------------------------------------------------------------------------------------#//
//# hm2_7190:
// .c:
static int hm2_7i90_setup(void) {
    rtapi_snprintf(board[i].llio.name, sizeof(board[i].llio.name), "%s.%d", HM2_LLIO_NAME, i);
}
int rtapi_app_main(void) {
    int r = 0;
    
    comp_id = hal_init(HM2_LLIO_NAME);
}
// .h:
#define HM2_LLIO_NAME "hm2_7i90"
