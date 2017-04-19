
/**
 * @file canod.h
 * @brief Common defines for object dictionary access
 */

#ifndef CANOD_H
#define CANOD_H

/* SDO Information operation code */
#define CANOD_OP_

/* local configuration defines */
#define USER_DEFINED_PDOS       1

/* list of dictionary lists identifiers */
#define CANOD_GET_NUMBER_OF_OBJECTS   0x00
#define CANOD_ALL_OBJECTS             0x01
#define CANOD_RXPDO_MAPABLE           0x02
#define CANOD_TXPDO_MAPABLE           0x03
#define CANOD_DEVICE_REPLACEMENT      0x04
#define CANOD_STARTUP_PARAMETER       0x05

/* possible object types of dictionary objects */
#define CANOD_TYPE_DOMAIN     0x0
#define CANOD_TYPE_DEFTYPE    0x5
#define CANOD_TYPE_DEFSTRUCT  0x6
#define CANOD_TYPE_VAR        0x7
#define CANOD_TYPE_ARRAY      0x8
#define CANOD_TYPE_RECORD     0x9

/* value info values */
#define CANOD_VALUEINFO_UNIT      0x08
#define CANOD_VALUEINFO_DEFAULT   0x10
#define CANOD_VALUEINFO_MIN       0x20
#define CANOD_VALUEINFO_MAX       0x40

/* list types */
#define CANOD_LIST_ALL        0x01  ///< all objects
#define CANOD_LIST_RXPDO_MAP  0x02  ///< only objects which are mappable in a RxPDO
#define CANOD_LIST_TXPDO_MAP  0x03  ///< only objects which are mappable in a TxPDO
#define CANOD_LIST_REPLACE    0x04  ///< objects which has to stored for a device replacement ???
#define CANOD_LIST_STARTUP    0x05  ///< objects which can be used as startup parameter

/* create pdo mapping parameter */
#define PDOMAPING(idx,sub,bit)    ( ((unsigned)idx<<16) | ((unsigned)sub<<8) | bit )

/* object dictionary address defines for CIA 402 - see object_name_defines.h */

/* Operating modes for use in objects CIA402_OP_MODES and CIA402_OP_MODES_DISPLAY */
#define CIA402_OP_MODE_CSP    8
#define CIA402_OP_MODE_CSV    9
#define CIA402_OP_MODE_CST   10

/* CAN Object Entry Access Flags */
#define COD_RD_PO_STATE         0x0001
#define COD_RD_SO_STATE         0x0002
#define COD_RD_OP_STATE         0x0004
#define COD_WR_PO_STATE         0x0008
#define COD_WR_SO_STATE         0x0010
#define COD_WR_OP_STATE         0x0020
#define COD_RXPDO_MAPABLE       0x0040
#define COD_TXPDO_MAPABLE       0x0080
#define COD_USED_BACKUP         0x0100
#define COD_USED_SETTINGS       0x0200

#endif /* CANOD_H */
