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
//#--------------------------------------------------------------------------------------------#//
//#--------------------------------------------------------------------------------------------#//
//# hm2_7143:
// .c:
static int hm2_7i43_setup(void) {
   rtapi_snprintf(board[i].llio.name, sizeof(board[i].llio.name), "%s.%d", HM2_LLIO_NAME, i);
}
int rtapi_app_main(void) {
    int r = 0;

    comp_id = hal_init(HM2_LLIO_NAME);
}
// .h:
#define HM2_LLIO_NAME "hm2_7i43"

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

//#--------------------------------------------------------------------------------------------#//
//# hm2_eth:
// .c
int rtapi_app_main(void) {
    int ret;
    LL_PRINT("loading Mesa AnyIO HostMot2 ethernet driver version " HM2_ETH_VERSION "\n");
    ret = hal_init(HM2_LLIO_NAME);
}
// .h:
#define HM2_LLIO_NAME "hm2_eth"

//#--------------------------------------------------------------------------------------------#//
//# hm2_pci:
// .c:
static struct pci_driver hm2_pci_driver = {
    .name = HM2_LLIO_NAME,
}
int rtapi_app_main(void) {
    int r = 0;    
    LL_PRINT("loading Mesa AnyIO HostMot2 driver version " HM2_PCI_VERSION "\n");
    comp_id = hal_init(HM2_LLIO_NAME);
}
// .h:
#define HM2_LLIO_NAME "hm2_pci"

//#--------------------------------------------------------------------------------------------#//
//# hm2_test:
// .c:
int rtapi_app_main(void) {
    hm2_test_t *me;
    hm2_lowlevel_io_t *this;
    int r = 0;
    LL_PRINT("loading HostMot2 test driver with test pattern %d\n", test_pattern);
    comp_id = hal_init(HM2_LLIO_NAME);
}
// .h:
#define HM2_LLIO_NAME "hm2_test"


//#--------------------------------------------------------------------------------------------#//
// hostmot2-lowlevel.h

#define LL_PRINT(fmt, args...)    rtapi_print(HM2_LLIO_NAME ": " fmt, ## args);
#define THIS_PRINT(fmt, args...)  rtapi_print("%s: " fmt, this->name, ## args);

#define LL_PRINT_IF(enable, fmt, args...)  if (enable) { rtapi_print(HM2_LLIO_NAME ": " fmt, ## args); }

#define LL_ERR(fmt, args...)   rtapi_print_msg(RTAPI_MSG_ERR,  HM2_LLIO_NAME ": " fmt, ## args);
#define LL_WARN(fmt, args...)  rtapi_print_msg(RTAPI_MSG_WARN, HM2_LLIO_NAME ": " fmt, ## args);
#define LL_INFO(fmt, args...)  rtapi_print_msg(RTAPI_MSG_INFO, HM2_LLIO_NAME ": " fmt, ## args);
#define LL_DBG(fmt, args...)   rtapi_print_msg(RTAPI_MSG_DBG,  HM2_LLIO_NAME ": " fmt, ## args);

