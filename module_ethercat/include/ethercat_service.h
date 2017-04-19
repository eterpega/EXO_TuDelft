
/**
 * @file ethercat_service.h
 */

#ifndef ETHERCAT_H
#define ETHERCAT_H

#include <stdint.h>
#include <stdlib.h>

static const char ecat_version[] = "SOMANET EtherCAT Service Version 2.0-dev\n";

//#define ETHERCAT_WITH_BOOTLOADER    1

/**
* @brief Control word to request PDO data.
*/
#define DATA_REQUEST     1

/**
 * @brief Configured SyncManager Mailbox size
 *
 * Currently two settings are used in the SOMANET family:
 * - (old) standard 128 bytes
 * - new mailbox size 1024 bytes
 *
 * WARNING before changing this value make sure your SII configuration supports
 * the setting.
 */
#define CONFIG_SM_MAILBOX_SIZE      1024

/** 
 *  @brief Structure containing resources required for interfing the EtherCAT chip.
 *
 *  This structure contains resources required for interfing the EtherCAT chip.
 *  It consists of 8 ports.
 *
 **/
typedef struct {
     out port p_ecatRST;
     in  port p_ecatIRQ;
     in  port p_ecatBUSY;
     out port p_ecatWR;
     out port p_ecatRD;
     out port p_ecatCS;
         port p_ecatData;
     out port p_ecatAddress;
} EthercatPorts;

/**
* @brief Lorem ipsum...
*/
enum EC_MailboxProtocolTypes {
	ERROR_PACKET=0,            /**< Error Packet */
	VENDOR_BECKHOFF_PACKET,    /**< Beckhoff vendor specific packet */
	EOE_PACKET,                /**< Ethernet-over-EtherCAT packet */
	COE_PACKET,                /**< CAN over EtherCAT packet */
	FOE_PACKET,                /**< File over EtherCAT packet */
	SOE_PACKET,                /**< SoE */
	VOE_PACKET=0xf             /**< Vendor specific mailbox packet */
};

/**
* @brief Lorem ipsum...
*/
struct _ec_mailbox_header {
	uint16_t length;           /**< length of data area */
	uint16_t address;          /**< originator address */
	uint8_t  channel;          /**< =0 reserved for future use */
	uint8_t  priority;         /**< 0 (lowest) to 3 (highest) */
	uint8_t  type;             /**< Protocol types -> enum EC_MailboxProtocolTypes */
	uint8_t  control;          /**< sequence number to detect duplicates */
};

/**
 * @brief Communication interface for CoE communication
 */
/* for some reasons, unknown to me, the xTIMEComposer shows here an syntax error. But the source code compiles and runs perfectly. */
interface i_coe_communication {
    uint32_t get_object_value(uint16_t index, uint8_t subindex);
    void     set_object_value(uint16_t index, uint8_t subindex, uint32_t value);

    [[clears_notification]] void configuration_done(void);
    [[notification]] slave void configuration_ready(void);
};

/* slave interface for reboot service */
interface EtherCATRebootInterface {
    void boot_to_bootloader(void);
};

/**
 *  @brief Communication interface for PDO communications
 */
interface i_pdo_communication {
    size_t   get_pdos_value(unsigned char pdo_value[]);
    void     set_pdos_value(unsigned char pdo_value[], size_t byte_count);
};

/**
 * @brief Enum definition for use with the data ready signal
 */
enum eFoeStat {
    FOE_STAT_DATA   = 0
    ,FOE_STAT_ERROR = -1
    ,FOE_STAT_EOF   = 1
};

enum eFoeNotificationType {
    FOE_NTYPE_UNDEF  = 0
    ,FOE_NTYPE_BOOT
    ,FOE_NTYPE_DATA
};

/**
 * @brief Communication interface for FoE communication
 *
 * Interface calls:
 * read_data() - access data of the current FoE transfere (max size = 512 as defined by the FoE spec)
 * result() - the consumer sets the FoE result, 0 for success != for Error
 * data_ready() - this signals the  consumer that a new data chunk is ready for reading
 */
interface i_foe_communication {
    {size_t, uint32_t, enum eFoeStat} read_data(int8_t data[]);
    [[clears_notification]] int get_notification_type(void);

    void result(uint32_t packet_number, int error);
    [[notification]] slave void data_ready(void);
};

/**
 * @brief Stub interface for future EoE communication
 */
interface i_eoe_communication {
    [[clears_notification]] int read_packet(int raw[]);
    [[notification]] slave void new_packet(void);
};

/**
 * @brief Main EtherCAT handler function.
 *
 * This function should run in a separate thread on the XMOS core controlling the I/O pins for
 * EtherCAT communication.
 *
 * For every packet send or received from or to this EtherCAT handler, the
 * first word transmitted indicates the number of words to follow (the packet
 * itself).
 *
 * @param i_ecat_reboot  access the reboot service
 * @param i_coe_communication send, receive and signal for CoE
 * @param i_eoe_communication interface for EoE data exchange
 * @param i_foe_communication send, receive and signal for FoE
 * @param c_pdo_r push received File packets
 * @param c_pdo_s read packets to send as File
 * @param ethercat_interface ports for interfacing EtherCAT chip.
 */
void ethercat_service(
            client interface EtherCATRebootInterface ?i_ecat_reboot,
            server interface i_coe_communication i_coecomm,
            server interface i_eoe_communication ?i_eoecom,
			server interface i_foe_communication i_foe,
			server interface i_pdo_communication i_pdo,
			EthercatPorts &ethercat_interface);

/**
 * @brief Init function for the EtherCAT module
 *
 * This function must be called in the first place to enable EtherCAT service.
 * @param ethercat_interface ports for interfacing EtherCAT chip.
 *
 * @return  0 on success
 */
int ecat_init(EthercatPorts &ethercat_interface);

/**
 * @brief Reset ethercat handler and services
 *
 * .. warning:: Currently this function is only a stub and doesn't perform any
 * functionality.
 */
int ecat_reset(void);

#ifdef ECAT_HWTEST
/**
 * The following functions are only used in the test compilation
 * of this module. During normal operation this code isn't generated.
 */

/**
 * @brief EtherCAT hardware test stub
 *
 * This function is a stub for a local test function.
 *
 * For the real implementation
 * @see https://github.com/sncn-private/SOMANET-hardware-tests/tree/master/sw_sncn_hw_tests_EtherCAT
 */
void ecat_hwtest(EthercatPorts &ethercat_ports);

/**
 * @brief Low-level read function
 *
 * @param address address to read from
 * @return data word from given address
 */
uint16_t ecatRead(uint16_t address, EthercatPorts &ethercat_ports);

/**
 * @brief Low-level write function
 *
 * @param address  address to write to
 * @param word     data word to write
 * @return 0 on success, >0 otherwise
 */
int ecatWrite(uint16_t address, uint16_t word, EthercatPorts &ethercat_ports);

/**
 * @brief Set Chipselect to allow read/write.
 */
void ecat_cs_set(EthercatPorts &ethercat_ports);

/**
 * @brief Unset Chipselect.
 */
void ecat_cs_unset(EthercatPorts &ethercat_ports);
#endif /* ECAT_HWTEST */

#endif /* ETHERCAT_H */
