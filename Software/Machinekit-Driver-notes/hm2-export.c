// relevant data structures:
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

//#--------------------------------------------------------------------------------#//
// rtapi_pci.h

/***********************************************************************
 *                        PCI DEVICE SUPPORT                            *
 ************************************************************************/
#if defined(BUILD_SYS_USER_DSO)

/*---------------------------------------------------------------------**
 * * Structures and defines that should be pulled in from <linux/pci.h>  **
 ** ...except we're not compiling kernel code.  :(                      **
 **---------------------------------------------------------------------*/

struct pci_device_id {
    __u32 vendor, device;           /* Vendor and device ID or PCI_ANY_ID*/
    __u32 subvendor, subdevice;     /* Subsystem ID's or PCI_ANY_ID */
    __u32 class, class_mask;        /* (class,subclass,prog-if) triplet */
};

struct pci_dev {
    char            dev_name[32];   /* Device name (0000:03:00.0) */
    char            sys_path[256];  /* Path to device (/sys/device/... */
    unsigned short  vendor;
    unsigned short  device;
    unsigned short  subsystem_vendor;
    unsigned short  subsystem_device;
    unsigned int    class;          /* 3 bytes: (base,sub,prog-if) */
    struct pci_resource   
    resource[6];    /* Device BARs */    
    void *driver_data;              /* Data private to the driver */
};

//#--------------------------------------------------------------------------------#//
// hm2_pci.h

typedef struct {
    struct pci_dev *dev;
    void __iomem *base;
    int len;
    unsigned long ctrl_base_addr;
    unsigned long data_base_addr;
    hm2_lowlevel_io_t llio;
} hm2_pci_t;

//#--------------------------------------------------------------------------------#//
//#--------------------------------------------------------------------------------#//
//#--------------------------------------------------------------------------------#//

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
//hm2_7i43.c:
// http://store.mesanet.com/index.php?route=product/product&filter_name=7i43&product_id=67
// 7I43-P FPGA based EPP Anything I/O card
// The MESA 7I43 is a low cost, general purpose programmable I/O card that connects to the host comp..
// 
// these are the low-level i/o functions exported to the hostmot2 driver
//

int hm2_7i43_read(hm2_lowlevel_io_t *this, u32 addr, void *buffer, int size) {
    int bytes_remaining = size;
    hm2_7i43_t *board = this->private;
    
    hm2_7i43_epp_addr16(addr | HM2_7I43_ADDR_AUTOINCREMENT, board);
    
    for (; bytes_remaining > 3; bytes_remaining -= 4) {
        *((u32*)buffer) = hm2_7i43_epp_read32(board);
        buffer += 4;
    }
    
    for ( ; bytes_remaining > 0; bytes_remaining --) {
        *((u8*)buffer) = hm2_7i43_epp_read(board);
        buffer ++;
    }
    
    if (hm2_7i43_epp_check_for_timeout(board)) {
        THIS_PRINT("EPP timeout on data cycle of read(addr=0x%04x, size=%d)\n", addr, size);
        (*this->io_error) = 1;
        this->needs_reset = 1;
        hm2_7i43_epp_clear_timeout(board);
        return 0;  // fail
    }
    
    return 1;  // success
}


int hm2_7i43_write(hm2_lowlevel_io_t *this, u32 addr, void *buffer, int size) {
    int bytes_remaining = size;
    hm2_7i43_t *board = this->private;
    
    hm2_7i43_epp_addr16(addr | HM2_7I43_ADDR_AUTOINCREMENT, board);
    
    for (; bytes_remaining > 3; bytes_remaining -= 4) {
        hm2_7i43_epp_write32(*((u32*)buffer), board);
        buffer += 4;
    }
    
    for ( ; bytes_remaining > 0; bytes_remaining --) {
        hm2_7i43_epp_write(*((u8*)buffer), board);
        buffer ++;
    }
    
    if (hm2_7i43_epp_check_for_timeout(board)) {
        THIS_PRINT("EPP timeout on data cycle of write(addr=0x%04x, size=%d)\n", addr, size);
        (*this->io_error) = 1;
        this->needs_reset = 1;
        hm2_7i43_epp_clear_timeout(board);
        return 0;
    }
    
    return 1;
}


int hm2_7i43_program_fpga(hm2_lowlevel_io_t *this, const bitfile_t *bitfile) {
    int orig_debug_epp = debug_epp;  // we turn off EPP debugging for this part...
    hm2_7i43_t *board = this->private;
    int64_t start_time, end_time;
    int i;
    const u8 *firmware = bitfile->e.data;
    
    
    //
    // send the firmware
    //
    
    debug_epp = 0;
    start_time = rtapi_get_time();
    
    // select the CPLD's data address
    hm2_7i43_epp_addr8(0, board);
    
    for (i = 0; i < bitfile->e.size; i ++, firmware ++) {
        hm2_7i43_epp_write(bitfile_reverse_bits(*firmware), board);
    }
    
    end_time = rtapi_get_time();
    debug_epp = orig_debug_epp;
    
    
    // see if it worked
    if (hm2_7i43_epp_check_for_timeout(board)) {
        THIS_PRINT("EPP Timeout while sending firmware!\n");
        return -EIO;
    }
    
    
    //
    // brag about how fast it was
    //
    
    {
        __u32 duration_ns;
        
        duration_ns = (__u32)(end_time - start_time);
        
        if (duration_ns != 0) {
            THIS_INFO(
                "%d bytes of firmware sent (%u KB/s)\n",
                      bitfile->e.size,
                      (__u32)(((double)bitfile->e.size / ((double)duration_ns / (double)(1000 * 1000 * 1000))) / 1024)
            );
        }
    }
    
    if(board->epp_wide)
    {
        hm2_7i43_epp_clear_timeout(board);
        u32 cookie;
        hm2_7i43_read(this, HM2_ADDR_IOCOOKIE, &cookie, sizeof(cookie));
        if(cookie != HM2_IOCOOKIE) {
            THIS_ERR("Reading cookie with epp_wide failed. (read 0x%08x) Falling back to byte transfers\n", cookie);
            board->epp_wide = 0;
            hm2_7i43_epp_clear_timeout(board);
            hm2_7i43_read(this, HM2_ADDR_IOCOOKIE, &cookie, sizeof(cookie));
            if(cookie == HM2_IOCOOKIE) {
                THIS_ERR("Successfully read cookie after selecting byte transfers\n");
            } else {
                THIS_ERR("Reading cookie still failed without epp_wide. (read 0x%08x)\n", cookie);
            }
        }
    }
    
    return 0;
}

// return 0 if the board has been reset, -errno if not
int hm2_7i43_reset(hm2_lowlevel_io_t *this) {
    hm2_7i43_t *board = this->private;
    __u8 byte;
    
    
    //
    // this resets the FPGA *only* if it's currently configured with the
    // HostMot2 or GPIO firmware
    //
    
    hm2_7i43_epp_addr16(0x7F7F, board);
    hm2_7i43_epp_write(0x5A, board);
    hm2_7i43_epp_addr16(0x7F7F, board);
    hm2_7i43_epp_write(0x5A, board);
    
    
    // 
    // this code resets the FPGA *only* if the CPLD is in charge of the
    // parallel port
    //
    
    // select the control register
    hm2_7i43_epp_addr8(1, board);
    
    // bring the Spartan3's PROG_B line low for 1 us (the specs require 300-500 ns or longer)
    hm2_7i43_epp_write(0x00, board);
    hm2_7i43_nanosleep(1000);
    
    // bring the Spartan3's PROG_B line high and wait for 2 ms before sending firmware (required by spec)
    hm2_7i43_epp_write(0x01, board);
    hm2_7i43_nanosleep(2 * 1000 * 1000);
    
    // make sure the FPGA is not asserting its /DONE bit
    byte = hm2_7i43_epp_read(board);
    if ((byte & 0x01) != 0) {
        LL_PRINT("/DONE is not low after CPLD reset!\n");
        return -EIO;
    }
    
    return 0;
}

//#--------------------------------------------------------------------------------#//
//hm2_test.c:
//
//
// 
// these are the "low-level I/O" functions exported up
//


static int hm2_test_read(hm2_lowlevel_io_t *this, u32 addr, void *buffer, int size) {
    hm2_test_t *me = this->private;
    memcpy(buffer, &me->test_pattern.tp8[addr], size);
    return 1;  // success
}


static int hm2_test_write(hm2_lowlevel_io_t *this, u32 addr, void *buffer, int size) {
    return 1;  // success
}


static int hm2_test_program_fpga(hm2_lowlevel_io_t *this, const bitfile_t *bitfile) {
    return 0;
}


static int hm2_test_reset(hm2_lowlevel_io_t *this) {
    return 0;
}

//#--------------------------------------------------------------------------------#//
//hm2_eth.c:
// http://store.mesanet.com/index.php?route=product/product&filter_name=7I80&product_id=59
// 7I80DB-16 Ethernet Anything I/O card
// The MESA 7I80DB is a low cost, general purpose, FPGA based programmable I/O card with 100 BaseT E..

/// hm2_eth io functions

static int hm2_eth_read(hm2_lowlevel_io_t *this, u32 addr, void *buffer, int size) {
    int send, recv, i = 0;
    u8 tmp_buffer[size + 4];
    long long t1, t2;
    
    if (comm_active == 0) return 1;
    if (size == 0) return 1;
    read_cnt++;
    
    LBP16_INIT_PACKET4(read_packet, CMD_READ_HOSTMOT2_ADDR32_INCR(size/4), addr & 0xFFFF);
    
    send = eth_socket_send(sockfd, (void*) &read_packet, sizeof(read_packet), 0);
    if(send < 0)
        LL_PRINT("ERROR: sending packet: %s\n", strerror(errno));
    LL_PRINT_IF(debug, "read(%d) : PACKET SENT [CMD:%02X%02X | ADDR: %02X%02X | SIZE: %d]\n", read_cnt, read_packet.cmd_hi, read_packet.cmd_lo,
                read_packet.addr_lo, read_packet.addr_hi, size);
    t1 = rtapi_get_time();
    do {
        rtapi_delay(READ_PCK_DELAY_NS);
        recv = eth_socket_recv(sockfd, (void*) &tmp_buffer, size, 0);
        t2 = rtapi_get_time();
        i++;
    } while ((recv < 0) && ((t2 - t1) < 200*1000*1000));
    
    if (recv == 4) {
        LL_PRINT_IF(debug, "read(%d) : PACKET RECV [DATA: %08X | SIZE: %d | TRIES: %d | TIME: %llu]\n", read_cnt, *tmp_buffer, recv, i, t2 - t1);
    } else {
        LL_PRINT_IF(debug, "read(%d) : PACKET RECV [SIZE: %d | TRIES: %d | TIME: %llu]\n", read_cnt, recv, i, t2 - t1);
    }
    if (recv < 0)
        return 0;
    memcpy(buffer, tmp_buffer, size);
    return 1;  // success
}

static int hm2_eth_enqueue_read(hm2_lowlevel_io_t *this, u32 addr, void *buffer, int size) {
    if (comm_active == 0) return 1;
    if (size == 0) return 1;
    if (size == -1) {
        int send, recv, i = 0;
        long long t1, t2;
        u8 tmp_buffer[queue_buff_size];
        
        read_cnt++;
        send = eth_socket_send(sockfd, (void*) &queue_packets, sizeof(lbp16_cmd_addr)*queue_reads_count, 0);
        if(send < 0)
            LL_PRINT("ERROR: sending packet: %s\n", strerror(errno));
        t1 = rtapi_get_time();
        do {
            rtapi_delay(READ_PCK_DELAY_NS);
            recv = eth_socket_recv(sockfd, (void*) &tmp_buffer, queue_buff_size, 0);
            t2 = rtapi_get_time();
            i++;
        } while ((recv < 0) && ((t2 - t1) < 200*1000*1000));
        LL_PRINT_IF(debug, "enqueue_read(%d) : PACKET RECV [SIZE: %d | TRIES: %d | TIME: %llu]\n", read_cnt, recv, i, t2 - t1);
        
        for (i = 0; i < queue_reads_count; i++) {
            memcpy(queue_reads[i].buffer, &tmp_buffer[queue_reads[i].from], queue_reads[i].size);
        }
        
        queue_reads_count = 0;
        queue_buff_size = 0;
    } else {
        LBP16_INIT_PACKET4(queue_packets[queue_reads_count], CMD_READ_HOSTMOT2_ADDR32_INCR(size/4), addr);
        queue_reads[queue_reads_count].buffer = buffer;
        queue_reads[queue_reads_count].size = size;
        queue_reads[queue_reads_count].from = queue_buff_size;
        queue_reads_count++;
        queue_buff_size += size;
    }
    return 1;
}

static int hm2_eth_write(hm2_lowlevel_io_t *this, u32 addr, void *buffer, int size) {
    int send;
    static struct {
        lbp16_cmd_addr wr_packet;
        u8 tmp_buffer[127*8];
    } packet;
    
    if (comm_active == 0) return 1;
    if (size == 0) return 1;
    write_cnt++;
    
    memcpy(packet.tmp_buffer, buffer, size);
    LBP16_INIT_PACKET4(packet.wr_packet, CMD_WRITE_HOSTMOT2_ADDR32_INCR(size/4), addr & 0xFFFF);
    
    send = eth_socket_send(sockfd, (void*) &packet, sizeof(lbp16_cmd_addr) + size, 0);
    if(send < 0)
        LL_PRINT("ERROR: sending packet: %s\n", strerror(errno));
    LL_PRINT_IF(debug, "write(%d): PACKET SENT [CMD:%02X%02X | ADDR: %02X%02X | SIZE: %d]\n", write_cnt, packet.wr_packet.cmd_hi, packet.wr_packet.cmd_lo,
                packet.wr_packet.addr_lo, packet.wr_packet.addr_hi, size);
    
    return 1;  // success
}

static int hm2_eth_enqueue_write(hm2_lowlevel_io_t *this, u32 addr, void *buffer, int size) {
    if (comm_active == 0) return 1;
    if (size == 0) return 1;
    if (size == -1) {
        int send;
        long long t0, t1;
        
        write_cnt++;
        t0 = rtapi_get_time();
        send = eth_socket_send(sockfd, (void*) &write_packet, write_packet_size, 0);
        if(send < 0)
            LL_PRINT("ERROR: sending packet: %s\n", strerror(errno));
        t1 = rtapi_get_time();
        LL_PRINT_IF(debug, "enqueue_write(%d) : PACKET SEND [SIZE: %d | TIME: %llu]\n", write_cnt, send, t1 - t0);
        write_packet_ptr = &write_packet;
        write_packet_size = 0;
    } else {
        lbp16_cmd_addr *packet = (lbp16_cmd_addr *) write_packet_ptr;
        
        LBP16_INIT_PACKET4_PTR(packet, CMD_WRITE_HOSTMOT2_ADDR32_INCR(size/4), addr);
        write_packet_ptr += sizeof(*packet);
        memcpy(write_packet_ptr, buffer, size);
        write_packet_ptr += size;
        write_packet_size += (sizeof(*packet) + size);
    }
    return 1;
}

//#--------------------------------------------------------------------------------#//
//hm2_pci.c:
// http://store.mesanet.com/index.php?route=product/product&filter_name=5i20&product_id=66
// 5I20 FPGA based PCI Anything I/O card
// The MESA 5I20 is a general purpose programmable I/O card for the PCI bus. The 5I20 uses a 200K ga..
//
// http://store.mesanet.com/index.php?route=product/product&filter_name=5i25&product_id=55
// 5I25 Superport FPGA based PCI Anything I/O card
// The MESA 5I25 is a low cost, general purpose, FPGA based programmable I/O card for the PCI bus.&n..
// 
// these are the "low-level I/O" functions exported up
//

static int hm2_pci_read(hm2_lowlevel_io_t *this, u32 addr, void *buffer, int size) {
    hm2_pci_t *board = this->private;
    int i;
    u32* src = (u32*) (board->base + addr);
    u32* dst = (u32*) buffer;
    
    /* Per Peter Wallace, all hostmot2 access should be 32 bits and 32-bit aligned */
    /* Check for any address or size values that violate this alignment */
    if ( ((addr & 0x3) != 0) || ((size & 0x03) != 0) ){
        u16* dst16 = (u16*) dst;
        u16* src16 = (u16*) src;
        /* hm2_read_idrom performs a 16-bit read, which seems to be OK, so let's allow it */
        if ( ((addr & 0x1) != 0) || (size != 2) ){
            rtapi_print_msg(RTAPI_MSG_ERR, "hm2_pci_read: Unaligned Access: %08x %04x\n", addr,size);
            memcpy(dst, src, size);
            return 1;  // success
        }
        dst16[0] = src16[0];
        return 1;  // success
    }
    
    //    rtapi_print_msg(RTAPI_MSG_ERR, "pci_read : %08x.%04x", addr,size);
    for (i=0; i<(size/4); i++) {
        dst[i] = src[i];
        //        rtapi_print_msg(RTAPI_MSG_ERR, " %08x", dst[i]);
    }
    //    rtapi_print_msg(RTAPI_MSG_ERR, "\n");
    return 1;  // success
}

static int hm2_pci_write(hm2_lowlevel_io_t *this, u32 addr, void *buffer, int size) {
    hm2_pci_t *board = this->private;
    int i;
    u32* src = (u32*) buffer;
    u32* dst = (u32*) (board->base + addr);
    
    /* Per Peter Wallace, all hostmot2 access should be 32 bits and 32-bit aligned */
    /* Check for any address or size values that violate this alignment */
    if ( ((addr & 0x3) != 0) || ((size & 0x03) != 0) ){
        rtapi_print_msg(RTAPI_MSG_ERR, "hm2_pci_write: Unaligned Access: %08x %04x\n", addr,size);
        memcpy(dst, src, size);
        return 1;  // success
    }
    
    //    rtapi_print_msg(RTAPI_MSG_ERR, "pci_write: %08x.%04x", addr,size);
    for (i=0; i<(size/4); i++) {
        //        rtapi_print_msg(RTAPI_MSG_ERR, " %08x", src[i]);
        dst[i] = src[i];
    }
    //    rtapi_print_msg(RTAPI_MSG_ERR, "\n");
    return 1;  // success
}

static int hm2_plx9030_program_fpga(hm2_lowlevel_io_t *this, const bitfile_t *bitfile) {
    hm2_pci_t *board = this->private;
    int i;
    u32 status, control;
    
    // set /WRITE low for data transfer, and turn on LED
    status = inl(board->ctrl_base_addr + CTRL_STAT_OFFSET);
    control = status & ~_WRITE_MASK & ~_LED_MASK;
    outl(control, board->ctrl_base_addr + CTRL_STAT_OFFSET);
    
    // program the FPGA
    for (i = 0; i < bitfile->e.size; i ++) {
        outb(bitfile_reverse_bits(bitfile->e.data[i]), board->data_base_addr);
    }
    
    // all bytes transferred, make sure FPGA is all set up now
    status = inl(board->ctrl_base_addr + CTRL_STAT_OFFSET);
    if (!(status & _INIT_MASK)) {
        // /INIT goes low on CRC error
        THIS_ERR("FPGA asserted /INIT: CRC error\n");
        goto fail;
    }
    if (!(status & DONE_MASK)) {
        THIS_ERR("FPGA did not assert DONE\n");
        goto fail;
    }
    
    // turn off write enable and LED
    control = status | _WRITE_MASK | _LED_MASK;
    outl(control, board->ctrl_base_addr + CTRL_STAT_OFFSET);
    
    return 0;
    
    
    fail:
    // set /PROGRAM low (reset device), /WRITE high and LED off
    status = inl(board->ctrl_base_addr + CTRL_STAT_OFFSET);
    control = status & ~_PROGRAM_MASK;
    control |= _WRITE_MASK | _LED_MASK;
    outl(control, board->ctrl_base_addr + CTRL_STAT_OFFSET);
    return -EIO;
}

static int hm2_plx9030_reset(hm2_lowlevel_io_t *this) {
    hm2_pci_t *board = this->private;
    u32 status;
    u32 control;
    
    status = inl(board->ctrl_base_addr + CTRL_STAT_OFFSET);
    
    // set /PROGRAM bit low to reset the FPGA
    control = status & ~_PROGRAM_MASK;
    
    // set /WRITE and /LED high (idle state)
    control |= _WRITE_MASK | _LED_MASK;
    
    // and write it back
    outl(control, board->ctrl_base_addr + CTRL_STAT_OFFSET);
    
    // verify that /INIT and DONE went low
    status = inl(board->ctrl_base_addr + CTRL_STAT_OFFSET);
    if (status & (DONE_MASK | _INIT_MASK)) {
        THIS_ERR(
            "FPGA did not reset: /INIT = %d, DONE = %d\n",
            (status & _INIT_MASK ? 1 : 0),
                 (status & DONE_MASK ? 1 : 0)
        );
        return -EIO;
    }
    
    // set /PROGRAM high, let FPGA come out of reset
    control = status | _PROGRAM_MASK;
    outl(control, board->ctrl_base_addr + CTRL_STAT_OFFSET);
    
    // wait for /INIT to go high when it finishes clearing memory
    // This should take no more than 100uS.  If we assume each PCI read
    // takes 30nS (one PCI clock), that is 3300 reads.  Reads actually
    // take several clocks, but even at a microsecond each, 3.3mS is not
    // an excessive timeout value
    {
        int count = 3300;
        
        do {
            status = inl(board->ctrl_base_addr + CTRL_STAT_OFFSET);
            if (status & _INIT_MASK) break;
        } while (count-- > 0);
        
        if (count == 0) {
            THIS_ERR("FPGA did not come out of /INIT\n");
            return -EIO;
        }
    }
    
    return 0;
}

// fix up LASxBRD READY if needed
static void hm2_plx9030_fixup_LASxBRD_READY(hm2_pci_t *board) {
    hm2_lowlevel_io_t *this = &board->llio;
    int offsets[] = { LAS0BRD_OFFSET, LAS1BRD_OFFSET, LAS2BRD_OFFSET, LAS3BRD_OFFSET };
    int i;
    
    for (i = 0; i < 4; i ++) {
        u32 val;
        int addr = board->ctrl_base_addr + offsets[i];
        
        val = inl(addr);
        if (!(val & LASxBRD_READY)) {
            THIS_INFO("LAS%dBRD #READY is off, enabling now\n", i);
            val |= LASxBRD_READY;
            outl(val, addr);
        }
    }
}

static int hm2_plx9054_program_fpga(hm2_lowlevel_io_t *this, const bitfile_t *bitfile) {
    hm2_pci_t *board = this->private;
    int i;
    u32 status;
    
    // program the FPGA
    for (i = 0; i < bitfile->e.size; i ++) {
        outb(bitfile_reverse_bits(bitfile->e.data[i]), board->data_base_addr);
    }
    
    // all bytes transferred, make sure FPGA is all set up now
    for (i = 0; i < DONE_WAIT_5I22; i++) {
        status = inl(board->ctrl_base_addr + CTRL_STAT_OFFSET_5I22);
        if (status & DONE_MASK_5I22) break;
    }
    if (i >= DONE_WAIT_5I22) {
        THIS_ERR("Error: Not /DONE; programming not completed.\n");
        return -EIO;
    }
    
    return 0;
}

static int hm2_plx9054_reset(hm2_lowlevel_io_t *this) {
    hm2_pci_t *board = this->private;
    int i;
    u32 status, control;
    
    // set GPIO bits to GPIO function
    status = inl(board->ctrl_base_addr + CTRL_STAT_OFFSET_5I22);
    control = status | DONE_ENABLE_5I22 | _PROG_ENABLE_5I22;
    outl(control, board->ctrl_base_addr + CTRL_STAT_OFFSET_5I22);
    
    // Turn off /PROGRAM bit and insure that DONE isn't asserted
    outl(control & ~_PROGRAM_MASK_5I22, board->ctrl_base_addr + CTRL_STAT_OFFSET_5I22);
    
    status = inl(board->ctrl_base_addr + CTRL_STAT_OFFSET_5I22);
    if (status & DONE_MASK_5I22) {
        // Note that if we see DONE at the start of programming, it's most
        // likely due to an attempt to access the FPGA at the wrong I/O
        // location.
        THIS_ERR("/DONE status bit indicates busy at start of programming\n");
        return -EIO;
    }
    
    // turn on /PROGRAM output bit
    outl(control | _PROGRAM_MASK_5I22, board->ctrl_base_addr + CTRL_STAT_OFFSET_5I22);
    
    // Delay for at least 100 uS. to allow the FPGA to finish its reset
    // sequencing.  3300 reads is at least 100 us, could be as long as a
    // few ms
    for (i = 0; i < 3300; i++) {
        status = inl(board->ctrl_base_addr + CTRL_STAT_OFFSET);
    }
    
    return 0;
}

//#--------------------------------------------------------------------------------#//
//# int rtapi_app_main(void) {}:
//#--------------------------------------------------------------------------------#//
//hm2_7i90.c:
//
//
// setup and cleanup code
//

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

//#--------------------------------------------------------------------------------#//
//# int rtapi_app_main(void) {}:
//#--------------------------------------------------------------------------------#//
//hm2_7i43.c:
//

//
// setup and cleanup code
//

static void hm2_7i43_cleanup(void) {
    int i;
    
    // NOTE: hal_malloc() doesnt have a matching free
    
    for (i = 0; i < num_boards; i ++) {
        hm2_lowlevel_io_t *this = &board[i].llio;
        THIS_PRINT("releasing board\n");
        hm2_unregister(this);
        hal_parport_release(&board[i].port);
    }
}

static int hm2_7i43_setup(void) {
    int i;
    
    LL_PRINT("loading HostMot2 Mesa 7i43 driver version %s\n", HM2_7I43_VERSION);
    
    // zero the board structs
    memset(board, 0, HM2_7I43_MAX_BOARDS * sizeof(hm2_7i43_t));
    num_boards = 0;
    
    for (i = 0; i < HM2_7I43_MAX_BOARDS; i ++) {
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
            outb(0x94, board[i].port.base_hi + HM2_7I43_ECP_CONTROL_HIGH_OFFSET); // select EPP mode in ECR
        }
        
        //
        // when we get here, the parallel port is in epp mode and ready to go
        //
        
        // select the device and tell it to make itself ready for io
        hm2_7i43_epp_write_control(0x04, &board[i]);  // set control lines and input mode
        hm2_7i43_epp_clear_timeout(&board[i]);
        
        rtapi_snprintf(board[i].llio.name, sizeof(board[i].llio.name), "%s.%d", HM2_LLIO_NAME, i);
        board[i].llio.comp_id = comp_id;
        
        board[i].llio.read = hm2_7i43_read;
        board[i].llio.write = hm2_7i43_write;
        board[i].llio.program_fpga = hm2_7i43_program_fpga;
        board[i].llio.reset = hm2_7i43_reset;
        
        board[i].llio.num_ioport_connectors = 2;
        board[i].llio.pins_per_connector = 24;
        board[i].llio.ioport_connector_name[0] = "P4";
        board[i].llio.ioport_connector_name[1] = "P3";
        board[i].llio.num_leds = 8;
        board[i].llio.private = &board[i];
        
        this = &board[i].llio;
        
        
        //
        // now we want to detect if this 7i43 has the big FPGA or the small one
        // 3s200tq144 for the small board
        // 3s400tq144 for the big
        //
        
        // make sure the CPLD is in charge of the parallel port
        hm2_7i43_reset(&board[i].llio);
        
        //  select CPLD data register
        hm2_7i43_epp_addr8(0, &board[i]);
        
        if (hm2_7i43_epp_read(&board[i]) & 0x01) {
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
    
    r = hm2_7i43_setup();
    if (r) {
        hm2_7i43_cleanup();
        hal_exit(comp_id);
    } else {
        hal_ready(comp_id);
    }
    
    return r;
}

void rtapi_app_exit(void) {
    hm2_7i43_cleanup();
    hal_exit(comp_id);
    LL_PRINT("driver unloaded\n");
}

//#--------------------------------------------------------------------------------#//
//# int rtapi_app_main(void) {}:
//#--------------------------------------------------------------------------------#//
//hm2_test.c:
//

int rtapi_app_main(void) {
    hm2_test_t *me;
    hm2_lowlevel_io_t *this;
    int r = 0;
    
    LL_PRINT("loading HostMot2 test driver with test pattern %d\n", test_pattern);
    
    comp_id = hal_init(HM2_LLIO_NAME);
    if (comp_id < 0) return comp_id;
    
    me = &board[0];
    
    this = &me->llio;
    memset(this, 0, sizeof(hm2_lowlevel_io_t));
    
    me->llio.num_ioport_connectors = 1;
    me->llio.pins_per_connector = 24;
    me->llio.ioport_connector_name[0] = "P99";
    
    switch (test_pattern) {
        
        // 
        // this one has nothing
        // 
        
        case 0: {
            break;
        }
        
        
        // 
        // this one has a good IO Cookie, but that's it
        // 
        
        case 1: {
            set32(me, HM2_ADDR_IOCOOKIE, HM2_IOCOOKIE);
            break;
        }
        
        
        // 
        // this one has a good IO Cookie and Config Name
        // the idrom offset is 0, and there's nothing there
        // 
        
        case 2: {
            set32(me, HM2_ADDR_IOCOOKIE, HM2_IOCOOKIE);
            set8(me, HM2_ADDR_CONFIGNAME+0, 'H');
            set8(me, HM2_ADDR_CONFIGNAME+1, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+2, 'S');
            set8(me, HM2_ADDR_CONFIGNAME+3, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+4, 'M');
            set8(me, HM2_ADDR_CONFIGNAME+5, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+6, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+7, '2');
            break;
        }
        
        
        // 
        // good IO Cookie, Config Name, and IDROM Type
        // the IDROM offset is the usual, 0x400, and there's an invalid IDROM type there
        // 
        
        case 3: {
            set32(me, HM2_ADDR_IOCOOKIE, HM2_IOCOOKIE);
            set8(me, HM2_ADDR_CONFIGNAME+0, 'H');
            set8(me, HM2_ADDR_CONFIGNAME+1, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+2, 'S');
            set8(me, HM2_ADDR_CONFIGNAME+3, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+4, 'M');
            set8(me, HM2_ADDR_CONFIGNAME+5, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+6, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+7, '2');
            
            // put the IDROM at 0x400, where it usually lives
            set32(me, HM2_ADDR_IDROM_OFFSET, 0x400);
            
            // bad idrom type
            set32(me, 0x400, 0x1234);
            
            break;
        }
        
        
        // 
        // good IO Cookie, Config Name, and IDROM Type
        // the IDROM offset is the usual, 0x400, and there's a good IDROM type there
        // but the portwidth is 0
        // 
        
        case 4: {
            set32(me, HM2_ADDR_IOCOOKIE, HM2_IOCOOKIE);
            set8(me, HM2_ADDR_CONFIGNAME+0, 'H');
            set8(me, HM2_ADDR_CONFIGNAME+1, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+2, 'S');
            set8(me, HM2_ADDR_CONFIGNAME+3, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+4, 'M');
            set8(me, HM2_ADDR_CONFIGNAME+5, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+6, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+7, '2');
            set32(me, HM2_ADDR_IDROM_OFFSET, 0x400); // put the IDROM at 0x400, where it usually lives
            set32(me, 0x400, 2); // standard idrom type
            break;
        }
        
        
        // 
        // good IO Cookie, Config Name, and IDROM Type
        // the IDROM offset is the usual, 0x400, and there's a good IDROM type there
        // but the portwidth is 29 which is bogus
        // 
        
        case 5: {
            set32(me, HM2_ADDR_IOCOOKIE, HM2_IOCOOKIE);
            set8(me, HM2_ADDR_CONFIGNAME+0, 'H');
            set8(me, HM2_ADDR_CONFIGNAME+1, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+2, 'S');
            set8(me, HM2_ADDR_CONFIGNAME+3, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+4, 'M');
            set8(me, HM2_ADDR_CONFIGNAME+5, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+6, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+7, '2');
            set32(me, HM2_ADDR_IDROM_OFFSET, 0x400); // put the IDROM at 0x400, where it usually lives
            set32(me, 0x400, 2); // standard idrom type
            
            // bad PortWidth
            set32(me, 0x424, 29);
            
            break;
        }
        
        
        // 
        // good IO Cookie, Config Name, and IDROM Type
        // the IDROM offset is the usual, 0x400, and there's a good IDROM type there
        // good PortWidth
        // 
        
        case 6: {
            set32(me, HM2_ADDR_IOCOOKIE, HM2_IOCOOKIE);
            set8(me, HM2_ADDR_CONFIGNAME+0, 'H');
            set8(me, HM2_ADDR_CONFIGNAME+1, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+2, 'S');
            set8(me, HM2_ADDR_CONFIGNAME+3, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+4, 'M');
            set8(me, HM2_ADDR_CONFIGNAME+5, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+6, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+7, '2');
            set32(me, HM2_ADDR_IDROM_OFFSET, 0x400); // put the IDROM at 0x400, where it usually lives
            set32(me, 0x400, 2); // standard idrom type
            
            // good PortWidth
            set32(me, 0x424, 24);
            
            break;
        }
        
        
        // 
        // good IO Cookie, Config Name, and IDROM Type
        // the IDROM offset is the usual, 0x400, and there's a good IDROM type there
        // good PortWidth, but problematic IOPorts and IOWidth
        // 
        
        case 7: {
            set32(me, HM2_ADDR_IOCOOKIE, HM2_IOCOOKIE);
            set8(me, HM2_ADDR_CONFIGNAME+0, 'H');
            set8(me, HM2_ADDR_CONFIGNAME+1, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+2, 'S');
            set8(me, HM2_ADDR_CONFIGNAME+3, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+4, 'M');
            set8(me, HM2_ADDR_CONFIGNAME+5, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+6, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+7, '2');
            set32(me, HM2_ADDR_IDROM_OFFSET, 0x400); // put the IDROM at 0x400, where it usually lives
            set32(me, 0x400, 2); // standard idrom type
            
            // good PortWidth = 24, which is standard
            set32(me, 0x424, 24);
            
            // IOPorts = 1
            set32(me, 0x41c, 1);
            
            // IOWidth = 99 (!= IOPorts * PortWidth)
            set32(me, 0x420, 99);
            
            break;
        }
        
        
        // 
        // good IO Cookie, Config Name, and IDROM Type
        // the IDROM offset is the usual, 0x400, and there's a good IDROM type there
        // good PortWidth, but IOPorts doesnt match what the llio said
        // 
        
        case 8: {
            set32(me, HM2_ADDR_IOCOOKIE, HM2_IOCOOKIE);
            set8(me, HM2_ADDR_CONFIGNAME+0, 'H');
            set8(me, HM2_ADDR_CONFIGNAME+1, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+2, 'S');
            set8(me, HM2_ADDR_CONFIGNAME+3, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+4, 'M');
            set8(me, HM2_ADDR_CONFIGNAME+5, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+6, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+7, '2');
            set32(me, HM2_ADDR_IDROM_OFFSET, 0x400); // put the IDROM at 0x400, where it usually lives
            set32(me, 0x400, 2); // standard idrom type
            set32(me, 0x424, 24); // PortWidth = 24
            
            // IOPorts = 2 (!= what the llio said)
            set32(me, 0x41c, 2);
            
            // IOWidth == IOPorts * PortWidth)
            set32(me, 0x420, 48);
            
            break;
        }
        
        
        // 
        // good IO Cookie, Config Name, and IDROM Type
        // the IDROM offset is the usual, 0x400, and there's a good IDROM type there
        // good PortWidth, IOPorts, and IOWidth
        // but the clocks are bad
        // 
        
        case 9: {
            set32(me, HM2_ADDR_IOCOOKIE, HM2_IOCOOKIE);
            set8(me, HM2_ADDR_CONFIGNAME+0, 'H');
            set8(me, HM2_ADDR_CONFIGNAME+1, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+2, 'S');
            set8(me, HM2_ADDR_CONFIGNAME+3, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+4, 'M');
            set8(me, HM2_ADDR_CONFIGNAME+5, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+6, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+7, '2');
            set32(me, HM2_ADDR_IDROM_OFFSET, 0x400); // put the IDROM at 0x400, where it usually lives
            set32(me, 0x400, 2); // standard idrom type
            set32(me, 0x424, 24); // PortWidth = 24
            
            // IOWidth = (IOPorts * PortWidth)
            set32(me, 0x41c, 1);
            set32(me, 0x420, 24);
            
            // ClockLow = 12345
            set32(me, 0x428, 12345);
            
            break;
        }
        
        
        // 
        // good IO Cookie, Config Name, and IDROM Type
        // the IDROM offset is the usual, 0x400, and there's a good IDROM type there
        // good PortWidth, IOPorts, and IOWidth
        // but the clocks are bad
        // 
        
        case 10: {
            set32(me, HM2_ADDR_IOCOOKIE, HM2_IOCOOKIE);
            set8(me, HM2_ADDR_CONFIGNAME+0, 'H');
            set8(me, HM2_ADDR_CONFIGNAME+1, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+2, 'S');
            set8(me, HM2_ADDR_CONFIGNAME+3, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+4, 'M');
            set8(me, HM2_ADDR_CONFIGNAME+5, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+6, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+7, '2');
            set32(me, HM2_ADDR_IDROM_OFFSET, 0x400); // put the IDROM at 0x400, where it usually lives
            set32(me, 0x400, 2); // standard idrom type
            set32(me, 0x41c, 1);  // IOPorts = 1
            set32(me, 0x420, 24); // IOWidth = (IOPorts * PortWidth)
            set32(me, 0x424, 24); // PortWidth = 24
            
            // ClockLow = 2e6
            set32(me, 0x428, 2e6);
            
            // ClockHigh = 0
            set32(me, 0x42c, 0);
            
            break;
        }
        
        
        // 
        // good IO Cookie, Config Name, and IDROM Type
        // the IDROM offset is the usual, 0x400, and there's a good IDROM type there
        // good PortWidth, IOPorts, IOWidth, and clocks
        // 
        // The problem with this register file is that the Pin Descriptor
        // array contains no valid PDs, though the IDROM advertised 144 pins.
        //
        
        case 11: {
            set32(me, HM2_ADDR_IOCOOKIE, HM2_IOCOOKIE);
            set8(me, HM2_ADDR_CONFIGNAME+0, 'H');
            set8(me, HM2_ADDR_CONFIGNAME+1, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+2, 'S');
            set8(me, HM2_ADDR_CONFIGNAME+3, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+4, 'M');
            set8(me, HM2_ADDR_CONFIGNAME+5, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+6, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+7, '2');
            set32(me, HM2_ADDR_IDROM_OFFSET, 0x400); // put the IDROM at 0x400, where it usually lives
            set32(me, 0x400, 2); // standard idrom type
            
            // normal offset to Module Descriptors
            set32(me, 0x404, 64);
            
            // unusual offset to PinDescriptors
            set32(me, 0x408, 0x1C0);
            
            // IOPorts
            set32(me, 0x41c, 6);
            
            // IOWidth
            set32(me, 0x420, 6*24);
            
            // PortWidth
            set32(me, 0x424, 24);
            
            // ClockLow = 2e6
            set32(me, 0x428, 2e6);
            
            // ClockHigh = 2e7
            set32(me, 0x42c, 2e7);
            
            me->llio.num_ioport_connectors = 6;
            me->llio.ioport_connector_name[0] = "P4";
            me->llio.ioport_connector_name[1] = "P5";
            me->llio.ioport_connector_name[2] = "P6";
            me->llio.ioport_connector_name[3] = "P9";
            me->llio.ioport_connector_name[4] = "P8";
            me->llio.ioport_connector_name[5] = "P7";
            
            break;
        }
        
        
        //
        // good IO Cookie, Config Name, and IDROM Type
        // the IDROM offset is the usual, 0x400, and there's a good IDROM type there
        // good PortWidth, IOWidth, and clocks
        // but there are no IOPorts instances according to the MDs
        // (this is the case with a firmware Jeff made for testing an RNG circuit)
        //
        
        case 12: {
            int num_io_pins = 24;
            int pd_index;
            
            set32(me, HM2_ADDR_IOCOOKIE, HM2_IOCOOKIE);
            set8(me, HM2_ADDR_CONFIGNAME+0, 'H');
            set8(me, HM2_ADDR_CONFIGNAME+1, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+2, 'S');
            set8(me, HM2_ADDR_CONFIGNAME+3, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+4, 'M');
            set8(me, HM2_ADDR_CONFIGNAME+5, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+6, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+7, '2');
            set32(me, HM2_ADDR_IDROM_OFFSET, 0x400); // put the IDROM at 0x400, where it usually lives
            set32(me, 0x400, 2); // standard idrom type
            
            // normal offset to Module Descriptors
            set32(me, 0x404, 64);
            
            // normal offset to PinDescriptors
            set32(me, 0x408, 0x200);
            
            // IOPorts
            set32(me, 0x41c, 1);
            
            // IOWidth
            set32(me, 0x420, num_io_pins);
            
            // PortWidth
            set32(me, 0x424, 24);
            
            // ClockLow = 2e6
            set32(me, 0x428, 2e6);
            
            // ClockHigh = 2e7
            set32(me, 0x42c, 2e7);
            
            me->llio.num_ioport_connectors = 1;
            me->llio.ioport_connector_name[0] = "P3";
            
            // make a bunch of valid Pin Descriptors
            for (pd_index = 0; pd_index < num_io_pins; pd_index ++) {
                set8(me, 0x600 + (pd_index * 4) + 0, 0);               // SecPin (byte) = Which pin of secondary function connects here eg: A,B,IDX.  Output pins have bit 7 = '1'
                set8(me, 0x600 + (pd_index * 4) + 1, 0);               // SecTag (byte) = Secondary function type (PWM,QCTR etc).  Same as module GTag
                set8(me, 0x600 + (pd_index * 4) + 2, 0);               // SecUnit (byte) = Which secondary unit or channel connects here
                set8(me, 0x600 + (pd_index * 4) + 3, HM2_GTAG_IOPORT); // PrimaryTag (byte) = Primary function tag (normally I/O port)
            }
            
            break;
        }
        
        
        // this board has a non-standard (ie, non-24) number of pins per connector, but the idrom does not match that
        case 13: {
            set32(me, HM2_ADDR_IOCOOKIE, HM2_IOCOOKIE);
            set8(me, HM2_ADDR_CONFIGNAME+0, 'H');
            set8(me, HM2_ADDR_CONFIGNAME+1, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+2, 'S');
            set8(me, HM2_ADDR_CONFIGNAME+3, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+4, 'M');
            set8(me, HM2_ADDR_CONFIGNAME+5, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+6, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+7, '2');
            set32(me, HM2_ADDR_IDROM_OFFSET, 0x400); // put the IDROM at 0x400, where it usually lives
            set32(me, 0x400, 2); // standard idrom type
            
            // default PortWidth
            set32(me, 0x424, 24);
            
            // unusual number of pins per connector
            me->llio.pins_per_connector = 5;
            
            break;
        }
        
        
        // 
        // good IO Cookie, Config Name, and IDROM Type
        // the IDROM offset is the usual, 0x400, and there's a good IDROM type there
        // good but unusual (non-24) PortWidth
        // 
        
        case 14: {
            set32(me, HM2_ADDR_IOCOOKIE, HM2_IOCOOKIE);
            set8(me, HM2_ADDR_CONFIGNAME+0, 'H');
            set8(me, HM2_ADDR_CONFIGNAME+1, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+2, 'S');
            set8(me, HM2_ADDR_CONFIGNAME+3, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+4, 'M');
            set8(me, HM2_ADDR_CONFIGNAME+5, 'O');
            set8(me, HM2_ADDR_CONFIGNAME+6, 'T');
            set8(me, HM2_ADDR_CONFIGNAME+7, '2');
            set32(me, HM2_ADDR_IDROM_OFFSET, 0x400); // put the IDROM at 0x400, where it usually lives
            set32(me, 0x400, 2); // standard idrom type
            
            // good but unusual PortWidth
            set32(me, 0x424, 37);
            me->llio.pins_per_connector = 37;
            
            break;
        }
        
        default: {
            LL_ERR("unknown test pattern %d", test_pattern); 
            return -ENODEV;
        }
    }
    
    
    rtapi_snprintf(me->llio.name, sizeof(me->llio.name), "hm2_test.0");
    
    me->llio.fpga_part_number = "none";
    
    me->llio.program_fpga = hm2_test_program_fpga;
    me->llio.reset = hm2_test_reset;
    
    me->llio.comp_id = comp_id;
    me->llio.private = me;
    
    me->llio.threadsafe = 1;
    
    me->llio.read = hm2_test_read;
    me->llio.write = hm2_test_write;
    
    r = hm2_register(&board->llio, config[0]);
    if (r != 0) {
        THIS_ERR("hm2_test fails HM2 registration\n");
        return -EIO;
    }
    
    THIS_PRINT("initialized hm2 test-pattern %d\n", test_pattern);
    
    hal_ready(comp_id);
    return 0;
}

void rtapi_app_exit(void) {
    hm2_test_t *me = &board[0];
    
    hm2_unregister(&me->llio);
    
    LL_PRINT("driver unloaded\n");
    hal_exit(comp_id);
}

//#--------------------------------------------------------------------------------#//
//# int rtapi_app_main(void) {}:
//#--------------------------------------------------------------------------------#//
//hm2_eth.c:
//

int rtapi_app_main(void) {
    int ret;
    
    LL_PRINT("loading Mesa AnyIO HostMot2 ethernet driver version " HM2_ETH_VERSION "\n");
    
    ret = hal_init(HM2_LLIO_NAME);
    if (ret < 0)
        goto error0;
    comp_id = ret;
    
    ret = init_net();
    if (ret < 0)
        goto error1;
    
    comm_active = 1;
    
    ret = hm2_eth_probe();
    
    if (ret < 0)
        goto error1;
    
    hal_ready(comp_id);
    
    return 0;
    
    error1:
    close_net();
    error0:
    hal_exit(comp_id);
    return ret;
}

void rtapi_app_exit(void) {
    comm_active = 0;
    close_net();
    hal_exit(comp_id);
    LL_PRINT("HostMot2 ethernet driver unloaded\n");
}

//#--------------------------------------------------------------------------------#//
//# int rtapi_app_main(void) {}:
//#--------------------------------------------------------------------------------#//
//hm2_pci.c:
//

// 
// misc internal functions
//

static int hm2_pci_probe(struct pci_dev *dev, const struct pci_device_id *id) {
    int r;
    hm2_pci_t *board;
    hm2_lowlevel_io_t *this;
    
    
    if (num_boards >= HM2_PCI_MAX_BOARDS) {
        LL_PRINT("skipping AnyIO board at %s, this driver can only handle %d\n", pci_name(dev), HM2_PCI_MAX_BOARDS);
        return -EINVAL;
    }
    
    // NOTE: this enables the board's BARs -- this fixes the Arty bug
    if (pci_enable_device(dev)) {
        LL_PRINT("skipping AnyIO board at %s, failed to enable PCI device\n", pci_name(dev));
        return failed_errno = -ENODEV;
    }
    
    
    board = &hm2_pci_board[num_boards];
    this = &board->llio;
    memset(this, 0, sizeof(hm2_lowlevel_io_t));
    
    switch (dev->subsystem_device) {
        case HM2_PCI_SSDEV_5I20: {
            LL_PRINT("discovered 5i20 at %s\n", pci_name(dev));
            rtapi_snprintf(board->llio.name, sizeof(board->llio.name), "hm2_5i20.%d", num_5i20);
            num_5i20 ++;
            board->llio.num_ioport_connectors = 3;
            board->llio.pins_per_connector = 24;
            board->llio.ioport_connector_name[0] = "P2";
            board->llio.ioport_connector_name[1] = "P3";
            board->llio.ioport_connector_name[2] = "P4";
            board->llio.fpga_part_number = "2s200pq208";
            board->llio.num_leds = 8;
            break;
        }
        
        case HM2_PCI_SSDEV_5I21: {
            LL_PRINT("discovered 5i21 at %s\n", pci_name(dev));
            rtapi_snprintf(board->llio.name, sizeof(board->llio.name), "hm2_5i21.%d", num_5i21);
            num_5i21 ++;
            board->llio.num_ioport_connectors = 2;
            board->llio.pins_per_connector = 32;
            board->llio.ioport_connector_name[0] = "P1";
            board->llio.ioport_connector_name[1] = "P1";
            board->llio.fpga_part_number = "3s400pq208";
            board->llio.num_leds = 8;
            break;
        }
        
        case HM2_PCI_SSDEV_4I65: {
            LL_PRINT("discovered 4i65 at %s\n", pci_name(dev));
            rtapi_snprintf(board->llio.name, sizeof(board->llio.name), "hm2_4i65.%d", num_4i65);
            num_4i65 ++;
            board->llio.num_ioport_connectors = 3;
            board->llio.pins_per_connector = 24;
            board->llio.ioport_connector_name[0] = "P1";
            board->llio.ioport_connector_name[1] = "P3";
            board->llio.ioport_connector_name[2] = "P4";
            board->llio.fpga_part_number = "2s200pq208";
            board->llio.num_leds = 8;
            break;
        }
        
        case HM2_PCI_SSDEV_5I22_10:
        case HM2_PCI_SSDEV_5I22_15: {
            if (dev->subsystem_device == HM2_PCI_SSDEV_5I22_10) {
                LL_PRINT("discovered 5i22-1.0M at %s\n", pci_name(dev));
                board->llio.fpga_part_number = "3s1000fg320";
            } else {
                LL_PRINT("discovered 5i22-1.5M at %s\n", pci_name(dev));
                board->llio.fpga_part_number = "3s1500fg320";
            }
            rtapi_snprintf(board->llio.name, sizeof(board->llio.name), "hm2_5i22.%d", num_5i22);
            num_5i22 ++;
            board->llio.num_ioport_connectors = 4;
            board->llio.pins_per_connector = 24;
            board->llio.ioport_connector_name[0] = "P2";
            board->llio.ioport_connector_name[1] = "P3";
            board->llio.ioport_connector_name[2] = "P4";
            board->llio.ioport_connector_name[3] = "P5";
            board->llio.num_leds = 8;
            break;
        }
        
        case HM2_PCI_SSDEV_5I23: {
            LL_PRINT("discovered 5i23 at %s\n", pci_name(dev));
            rtapi_snprintf(board->llio.name, sizeof(board->llio.name), "hm2_5i23.%d", num_5i23);
            num_5i23 ++;
            board->llio.num_ioport_connectors = 3;
            board->llio.pins_per_connector = 24;
            board->llio.ioport_connector_name[0] = "P2";
            board->llio.ioport_connector_name[1] = "P3";
            board->llio.ioport_connector_name[2] = "P4";
            board->llio.fpga_part_number = "3s400pq208";
            board->llio.num_leds = 2;
            break;
        }
        
        case HM2_PCI_SSDEV_5I24: {
            LL_PRINT("discovered 5i24 at %s\n", pci_name(dev));
            rtapi_snprintf(board->llio.name, sizeof(board->llio.name), "hm2_5i24.%d", num_5i24);
            num_5i24 ++;
            board->llio.num_ioport_connectors = 3;
            board->llio.pins_per_connector = 24;
            board->llio.ioport_connector_name[0] = "P4";
            board->llio.ioport_connector_name[1] = "P3";
            board->llio.ioport_connector_name[2] = "P2";
            board->llio.fpga_part_number = "6slx16ftg256";
            board->llio.num_leds = 2;
            break;
        }
        
        case HM2_PCI_SSDEV_5I25:
        case HM2_PCI_SSDEV_6I25: {
            if (dev->subsystem_device == HM2_PCI_SSDEV_5I25) {
                LL_PRINT("discovered 5i25 at %s\n", pci_name(dev));
                rtapi_snprintf(board->llio.name, sizeof(board->llio.name), "hm2_5i25.%d", num_5i25);
                num_5i25 ++;
            } else {
                LL_PRINT("discovered 6i25 at %s\n", pci_name(dev));
                rtapi_snprintf(board->llio.name, sizeof(board->llio.name), "hm2_6i25.%d", num_6i25);
                num_6i25 ++;
            }
            board->llio.num_ioport_connectors = 2;
            board->llio.pins_per_connector = 17;
            board->llio.ioport_connector_name[0] = "P3";
            board->llio.ioport_connector_name[1] = "P2";
            board->llio.fpga_part_number = "6slx9tqg144";
            board->llio.num_leds = 2;
            break;
        }
        
        case HM2_PCI_SSDEV_4I68:
        case HM2_PCI_SSDEV_4I68_OLD: {
            if (dev->subsystem_device == HM2_PCI_SSDEV_4I68_OLD) {
                LL_PRINT("discovered OLD 4i68 at %s, please consider upgrading your EEPROM\n", pci_name(dev));
            } else {
                LL_PRINT("discovered 4i68 at %s\n", pci_name(dev));
            }
            rtapi_snprintf(board->llio.name, sizeof(board->llio.name), "hm2_4i68.%d", num_4i68);
            num_4i68 ++;
            board->llio.num_ioport_connectors = 3;
            board->llio.pins_per_connector = 24;
            board->llio.ioport_connector_name[0] = "P1";
            board->llio.ioport_connector_name[1] = "P2";
            board->llio.ioport_connector_name[2] = "P4";
            board->llio.fpga_part_number = "3s400pq208";
            board->llio.num_leds = 4;
            break;
        }
        
        case HM2_PCI_SSDEV_4I69_16:
        case HM2_PCI_SSDEV_4I69_25: {
            if (dev->subsystem_device == HM2_PCI_SSDEV_4I69_16) {
                LL_PRINT("discovered 4I69-16 at %s\n", pci_name(dev));
                board->llio.fpga_part_number = "6slx16ftg256";
                
            } else {
                LL_PRINT("discovered 4I69-25 at %s\n", pci_name(dev));
                board->llio.fpga_part_number = "6slx25ftg256";
            }
            rtapi_snprintf(board->llio.name, sizeof(board->llio.name), "hm2_4i69.%d", num_4i69);
            num_4i69 ++;
            board->llio.num_ioport_connectors = 3;
            board->llio.pins_per_connector = 24;
            board->llio.ioport_connector_name[0] = "P1";
            board->llio.ioport_connector_name[1] = "P3";
            board->llio.ioport_connector_name[2] = "P4";
            break;
        }
        
        case HM2_PCI_SSDEV_3X20_10:
        case HM2_PCI_SSDEV_3X20_15:
        case HM2_PCI_SSDEV_3X20_20: {
            if (dev->subsystem_device == HM2_PCI_SSDEV_3X20_10) {
                LL_PRINT("discovered 3x20-1.0M at %s\n", pci_name(dev));
                board->llio.fpga_part_number = "3s1000fg456";
            } else if (dev->subsystem_device == HM2_PCI_SSDEV_3X20_15) {
                LL_PRINT("discovered 3x20-1.5M at %s\n", pci_name(dev));
                board->llio.fpga_part_number = "3s1500fg456";
            } else {
                LL_PRINT("discovered 3x20-2.0M at %s\n", pci_name(dev));
                board->llio.fpga_part_number = "3s2000fg456";
            }
            rtapi_snprintf(board->llio.name, sizeof(board->llio.name), "hm2_3x20.%d", num_3x20);
            num_3x20 ++;
            board->llio.num_ioport_connectors = 6;
            board->llio.pins_per_connector = 24;
            board->llio.ioport_connector_name[0] = "P4";
            board->llio.ioport_connector_name[1] = "P5";
            board->llio.ioport_connector_name[2] = "P6";
            board->llio.ioport_connector_name[3] = "P9";
            board->llio.ioport_connector_name[4] = "P8";
            board->llio.ioport_connector_name[5] = "P7";
            board->llio.num_leds = 0;
            break;
        }
        
        default: {
            LL_ERR("unknown subsystem device id 0x%04x\n", dev->subsystem_device);
            return failed_errno = -ENODEV;
        }
    }
    
    
    switch (dev->device) {
        case HM2_PCI_DEV_PLX9030: {
            // get a hold of the IO resources we'll need later
            // FIXME: should request_region here
            board->ctrl_base_addr = (unsigned long) pci_resource_start(dev, 1);
            board->data_base_addr = (unsigned long) pci_resource_start(dev, 2);
            
            // BAR 5 is 64K mem (32 bit)
            board->len = pci_resource_len(dev, 5);
            board->base = pci_ioremap_bar(dev, 5);
            if (board->base == NULL) {
                THIS_ERR("could not map in FPGA address space\n");
                r = -ENODEV;
                goto fail0;
            }
            
            hm2_plx9030_fixup_LASxBRD_READY(board);
            
            board->llio.program_fpga = hm2_plx9030_program_fpga;
            board->llio.reset = hm2_plx9030_reset;
            break;
        }
        
        case HM2_PCI_DEV_PLX9056:
        case HM2_PCI_DEV_PLX9054: {
            // get a hold of the IO resources we'll need later
            // FIXME: should request_region here
            board->ctrl_base_addr = (unsigned long) pci_resource_start(dev, 1);
            board->data_base_addr = (unsigned long) pci_resource_start(dev, 2);
            
            // BAR 3 is 64K mem (32 bit)
            board->len = pci_resource_len(dev, 3);
            board->base = pci_ioremap_bar(dev, 3);
            if (board->base == NULL) {
                THIS_ERR("could not map in FPGA address space\n");
                r = -ENODEV;
                goto fail0;
            }
            
            board->llio.program_fpga = hm2_plx9054_program_fpga;
            board->llio.reset = hm2_plx9054_reset;
            
            break;
        }
        
        case HM2_PCI_DEV_MESA5I24:
        case HM2_PCI_DEV_MESA5I25:
        case HM2_PCI_DEV_MESA6I25: {
            // BAR 0 is 64K mem (32 bit)
            board->len = pci_resource_len(dev, 0);
            board->base = pci_ioremap_bar(dev, 0);
            if (board->base == NULL) {
                THIS_ERR("could not map in FPGA address space\n");
                r = -ENODEV;
                goto fail0;
            }
            break;
        }
        
        default: {
            THIS_ERR("unknown PCI Device ID 0x%04x\n", dev->device);
            r = -ENODEV;
            goto fail0;
        }
    }
    
    
    board->dev = dev;
    
    pci_set_drvdata(dev, board);
    
    board->llio.comp_id = comp_id;
    board->llio.private = board;
    
    board->llio.threadsafe = 1;
    
    board->llio.read = hm2_pci_read;
    board->llio.write = hm2_pci_write;
    
    r = hm2_register(&board->llio, config[num_boards]);
    if (r != 0) {
        THIS_ERR("board fails HM2 registration\n");
        goto fail1;
    }
    
    THIS_PRINT("initialized AnyIO board at %s\n", pci_name(dev));
    
    num_boards ++;
    return 0;
    
    
    fail1:
    pci_set_drvdata(dev, NULL);
    iounmap(board->base);
    board->base = NULL;
    
    fail0:
    pci_disable_device(dev);
    return failed_errno = r;
}

static void hm2_pci_remove(struct pci_dev *dev) {
    int i;
    
    for (i = 0; i < num_boards; i++) {
        hm2_pci_t *board = &hm2_pci_board[i];
        hm2_lowlevel_io_t *this = &board->llio;
        
        if (board->dev == dev) {
            THIS_PRINT("dropping AnyIO board at %s\n", pci_name(dev));
            
            hm2_unregister(&board->llio);
            
            // Unmap board memory
            if (board->base != NULL) {
                iounmap(board->base);
                board->base = NULL;
            }
            
            pci_disable_device(dev);
            pci_set_drvdata(dev, NULL);
            board->dev = NULL;
        }
    }
}

static struct pci_driver hm2_pci_driver = {
    .name = HM2_LLIO_NAME,
    .id_table = hm2_pci_tbl,
    .probe = hm2_pci_probe,
    .remove = hm2_pci_remove,
};

int rtapi_app_main(void) {
    int r = 0;
    
    LL_PRINT("loading Mesa AnyIO HostMot2 driver version " HM2_PCI_VERSION "\n");
    
    comp_id = hal_init(HM2_LLIO_NAME);
    if (comp_id < 0) return comp_id;
    
    r = pci_register_driver(&hm2_pci_driver);
    if (r != 0) {
        LL_ERR("error registering PCI driver\n");
        hal_exit(comp_id);
        return r;
    }
    
    if(failed_errno) {
        // at least one card registration failed
        hal_exit(comp_id);
        pci_unregister_driver(&hm2_pci_driver);
        return failed_errno;
    }
    
    if(num_boards == 0) {
        // no cards were detected
        LL_PRINT("error no supported cards detected\n");
        hal_exit(comp_id);
        pci_unregister_driver(&hm2_pci_driver);
        return -ENODEV;
    }
    
    hal_ready(comp_id);
    return 0;
}

void rtapi_app_exit(void) {
    pci_unregister_driver(&hm2_pci_driver);
    LL_PRINT("driver unloaded\n");
    hal_exit(comp_id);
}

