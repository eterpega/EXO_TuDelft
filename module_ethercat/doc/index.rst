.. _somanet_ethercat_slave:

=============================
EtherCAT Slave Module
=============================

.. contents:: In this document
    :backlinks: none
    :depth: 3

**EtherCAT** is a real-time open communication protocol based on Ethernet networks.
The standardization process is coordinated by the EtherCAT_ Technology Group.

This module (**module_ethercat** ) allows an abstract access to the application layer of the EtherCAT
communication stack. The underlying device access and data transfer is
handled without the interaction of the device user application.

The EtherCAT access is implemented using a **single thread**. All communication
is done by interfaces. The PDOs are stored transparently for quick access.

The module supports the following protocols:

* **PDO transfer**
* **CAN over EtherCAT (CoE) with CiA402**
* **File over EtherCAT (FoE)**

.. cssclass:: github

  `See Module on Public Repository <https://github.com/synapticon/sc_sncn_ethercat/tree/master/module_ethercat>`_

.. _EtherCAT: http://www.ethercat.org

How to Use
==========

One design goal for this module was easy usability to make the EtherCAT
communication as simple as possible.

There is the main header :ethercat_service.h: with the essential function:

:``ethercat_service()``: start the EtherCAT communication and serve the data via interfaces

After the call to ``ethercat_service()`` the module is operational.

If the firmware also want to use the device object dictionary than the header
files :dictionary.h: (for the object dictionary itself) and :canod.h: for a
more comfortable access are required.

PDO Communication
-----------------

The process data object (PDO) are exchanged with the interface
`i_pdo_communication`. There are two interface calls defined:

- `get_pdos_value()` to receive the current set of PDO values
- `set_pdos_value()` to set the values of the outgoing PDOs, these are sent to the master in the next cycle.

The exchange of the PDO values is done with the `module_pdo_handler` from the
:ref:`EtherCAT Drive` repository. It is recommendet to use this module.

CoE Communication
-----------------

To access the object dictionary the CoE interface `i_coe_communication` needs
to be used. The object dictionary contains all necessary drive configuration
which may be needed to configure the drive for proper operation.

The CoE interface provides the following calls:

- `get_object_value(index, subindex)` get the value of the object specified by `index` and `subindex`
- `set_object_value(index, subindex, value)` set the value of the object specified by `index` and `subindex` to value `value`
- `slave void configuration_ready()` this notification is send when the ethercat drive is set to operational mode (OP-MODE)
  This notification signals the motorcontrol that the current values in the object dictionary are up to date and can be read
  to configure the drive.
- `void configuration_done(void)` the client of the interface has to clear the notification with this call


FoE Communication
-----------------

The File over EtherCAT (FoE) is primary used to transfer the firmware update
from the master to the slave. Technically you have to set the slave to BOOT
mode and then initiate the FoE transfer of the data. The new firmware is
received by the `reboot service` and stored in flash. It will be booted next
time the device starts.

To transfer data from the master to the slave the interface
`i_foe_communication` provides the following calls:

- `{size_t, uint32_t, enum eFoeStat} read_data(int8_t data[])` - access data of the current FoE transfere (max size = 512 as defined by the FoE spec)
- `[[clears_notification]] int get_notification_type(void)`

- `void result(uint32_t packet_number, int error)` - the consumer sets the FoE result, 0 for success != for Error
- `[[notification]] slave void data_ready(void)` - this signals the  consumer that a new data chunk is ready for reading


API
===

The main user API for the **module_ethercat** component is found in **ethercat_service.h**.

Symbolic constants
-------------------

.. doxygendefine:: DATA_REQUEST

Interface for PDO send and request
``````````````````````````````````

.. doxygeninterface:: i_pdo_communication

Interface Commands for CAN Object Dictionary requests
`````````````````````````````````````````````````````

.. doxygeninterface:: i_coe_communication

Interface Commands for FoE transfer
```````````````````````````````````

.. doxygeninterface:: i_foe_communication

Types
-----

.. doxygenstruct:: EthercatPorts

Functions
---------

.. doxygenfunction:: ethercat_service
