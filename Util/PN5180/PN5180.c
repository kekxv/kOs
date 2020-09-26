// NAME: PN5180.cpp
//
// DESC: Implementation of PN5180 class.
//
// Copyright (c) 2018 by Andreas Trappmann. All rights reserved.
//
// This file is part of the PN5180 library for the Arduino environment.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
//#define DEBUG 1

#include "PN5180.h"
#include "string.h"

// PN5180 1-Byte Direct Commands
// see 11.4.3.3 Host Interface Command List
#define PN5180_WRITE_REGISTER (0x00)
#define PN5180_WRITE_REGISTER_OR_MASK (0x01)
#define PN5180_WRITE_REGISTER_AND_MASK (0x02)
#define PN5180_READ_REGISTER (0x04)
#define PN5180_READ_EEPROM (0x07)
#define PN5180_SEND_DATA (0x09)
#define PN5180_READ_DATA (0x0A)
#define PN5180_SWITCH_MODE (0x0B)
#define PN5180_LOAD_RF_CONFIG (0x11)
#define PN5180_RF_ON (0x16)
#define PN5180_RF_OFF (0x17)


struct PN5180 pn5180;
void PN5180_PN5180(uint8_t SSpin[2], uint8_t BUSYpin[2], uint8_t RSTpin[2]) {
    memcpy(pn5180.PN5180_NSS, SSpin, 2);
    memcpy(pn5180.PN5180_BUSY, BUSYpin, 2);
    memcpy(pn5180.PN5180_RST, RSTpin, 2);
}

void PN5180_begin() {
    pn5180.digitalWrite(pn5180.PN5180_NSS, GPIO_PIN_LOW); // disable
    pn5180.digitalWrite(pn5180.PN5180_RST, GPIO_PIN_LOW); // no reset
    HAL_Delay(10);
    pn5180.digitalWrite(pn5180.PN5180_NSS, GPIO_PIN_HIGH); // disable
    pn5180.digitalWrite(pn5180.PN5180_RST, GPIO_PIN_HIGH); // no reset
}

void PN5180_end() {
    pn5180.digitalWrite(pn5180.PN5180_NSS, GPIO_PIN_HIGH); // disable
}

/*
 * WRITE_REGISTER - 0x00
 * This command is used to write a 32-bit value (little endian) to a configuration register.
 * The address of the register must exist. If the condition is not fulfilled, an exception is
 * raised.
 */
bool PN5180_writeRegister(uint8_t reg, uint32_t value) {
    uint8_t *p = (uint8_t *) &value;

    /*
  For all 4 byte command parameter transfers (e.g. register values), the payload
  parameters passed follow the little endian approach (Least Significant Byte first).
   */
    uint8_t buf[6] = {PN5180_WRITE_REGISTER, reg, p[0], p[1], p[2], p[3]};

    pn5180.transceiveCommand(buf, 6, NULL, 0);

    return true;
}

/*
 * WRITE_REGISTER_OR_MASK - 0x01
 * This command modifies the content of a register using a logical OR operation. The
 * content of the register is read and a logical OR operation is performed with the provided
 * mask. The modified content is written back to the register.
 * The address of the register must exist. If the condition is not fulfilled, an exception is
 * raised.
 */
bool PN5180_writeRegisterWithOrMask(uint8_t reg, uint32_t mask) {
    uint8_t *p = (uint8_t *) &mask;

    uint8_t buf[6] = {PN5180_WRITE_REGISTER_OR_MASK, reg, p[0], p[1], p[2], p[3]};

    pn5180.transceiveCommand(buf, 6, NULL, 0);

    return true;
}

/*
 * WRITE _REGISTER_AND_MASK - 0x02
 * This command modifies the content of a register using a logical AND operation. The
 * content of the register is read and a logical AND operation is performed with the provided
 * mask. The modified content is written back to the register.
 * The address of the register must exist. If the condition is not fulfilled, an exception is
 * raised.
 */
bool PN5180_writeRegisterWithAndMask(uint8_t reg, uint32_t mask) {
    uint8_t *p = (uint8_t *) &mask;

    uint8_t buf[6] = {PN5180_WRITE_REGISTER_AND_MASK, reg, p[0], p[1], p[2], p[3]};

    pn5180.transceiveCommand(buf, 6, NULL, 0);

    return true;
}

/*
 * READ_REGISTER - 0x04
 * This command is used to read the content of a configuration register. The content of the
 * register is returned in the 4 byte response.
 * The address of the register must exist. If the condition is not fulfilled, an exception is
 * raised.
 */
bool PN5180_readRegister(uint8_t reg, uint32_t *value) {
    uint8_t cmd[2] = {PN5180_READ_REGISTER, reg};

    pn5180.transceiveCommand(cmd, 2, (uint8_t *) value, 4);

    return true;
}

/*
 * READ_EEPROM - 0x07
 * This command is used to read data from EEPROM memory area. The field 'Address'
 * indicates the start address of the read operation. The field Length indicates the number
 * of bytes to read. The response contains the data read from EEPROM (content of the
 * EEPROM); The data is read in sequentially increasing order starting with the given
 * address.
 * EEPROM Address must be in the range from 0 to 254, inclusive. Read operation must
 * not go beyond EEPROM address 254. If the condition is not fulfilled, an exception is
 * raised.
 */
bool PN5180_readEEprom(uint8_t addr, uint8_t *buffer, int len) {
    if ((addr > 254) || ((addr + len) > 254)) {
        return false;
    }

    uint8_t cmd[3] = {PN5180_READ_EEPROM, addr, len};

    pn5180.transceiveCommand(cmd, 3, buffer, len);

    return true;
}

/*
 * SEND_DATA - 0x09
 * This command writes data to the RF transmission buffer and starts the RF transmission.
 * The parameter ‘Number of valid bits in last Byte’ indicates the exact number of bits to be
 * transmitted for the last byte (for non-byte aligned frames).
 * Precondition: Host shall configure the Transceiver by setting the register
 * SYSTEM_CONFIG.COMMAND to 0x3 before using the SEND_DATA command, as
 * the command SEND_DATA is only writing data to the transmission buffer and starts the
 * transmission but does not perform any configuration.
 * The size of ‘Tx Data’ field must be in the range from 0 to 260, inclusive (the 0 byte length
 * allows a symbol only transmission when the TX_DATA_ENABLE is cleared).‘Number of
 * valid bits in last Byte’ field must be in the range from 0 to 7. The command must not be
 * called during an ongoing RF transmission. Transceiver must be in ‘WaitTransmit’ state
 * with ‘Transceive’ command set. If the condition is not fulfilled, an exception is raised.
 */
bool PN5180_sendData(uint8_t *data, int len, uint8_t validBits) {
    if (len > 260) {
        return false;
    }

    uint8_t buffer[len + 2];
    buffer[0] = PN5180_SEND_DATA;
    buffer[1] = validBits; // number of valid bits of last byte are transmitted (0 = all bits are transmitted)
    for (int i = 0; i < len; i++) {
        buffer[2 + i] = data[i];
    }

    pn5180.writeRegisterWithAndMask(SYSTEM_CONFIG, 0xfffffff8); // Idle/StopCom Command
    pn5180.writeRegisterWithOrMask(SYSTEM_CONFIG, 0x00000003);  // Transceive Command
    /*
   * Transceive command; initiates a transceive cycle.
   * Note: Depending on the value of the Initiator bit, a
   * transmission is started or the receiver is enabled
   * Note: The transceive command does not finish
   * automatically. It stays in the transceive cycle until
   * stopped via the IDLE/StopCom command
   */

    uint8_t transceiveState = pn5180.getTransceiveState();
    if (PN5180_TS_WaitTransmit != transceiveState) {
        return false;
    }

    pn5180.transceiveCommand(buffer, len + 2, NULL, 0);

    return true;
}

/*
 * READ_DATA - 0x0A
 * This command reads data from the RF reception buffer, after a successful reception.
 * The RX_STATUS register contains the information to verify if the reception had been
 * successful. The data is available within the response of the command. The host controls
 * the number of bytes to be read via the SPI interface.
 * The RF data had been successfully received. In case the instruction is executed without
 * preceding an RF data reception, no exception is raised but the data read back from the
 * reception buffer is invalid. If the condition is not fulfilled, an exception is raised.
 */
uint8_t *PN5180_readData(int len) {
    if (len > 508) {
        return 0L;
    }

    uint8_t cmd[2] = {PN5180_READ_DATA, 0x00};

    pn5180.transceiveCommand(cmd, 2, pn5180.readBuffer, len);

    return pn5180.readBuffer;
}

/*
 * LOAD_RF_CONFIG - 0x11
 * Parameter 'Transmitter Configuration' must be in the range from 0x0 - 0x1C, inclusive. If
 * the transmitter parameter is 0xFF, transmitter configuration is not changed.
 * Field 'Receiver Configuration' must be in the range from 0x80 - 0x9C, inclusive. If the
 * receiver parameter is 0xFF, the receiver configuration is not changed. If the condition is
 * not fulfilled, an exception is raised.
 * The transmitter and receiver configuration shall always be configured for the same
 * transmission/reception speed. No error is returned in case this condition is not taken into
 * account.
 *
 * Transmitter: RF   Protocol          Speed     Receiver: RF    Protocol    Speed
 * configuration                       (kbit/s)  configuration               (kbit/s)
 * byte (hex)                                    byte (hex)
 * ----------------------------------------------------------------------------------------------
 * ->0D              ISO 15693 ASK100  26        8D              ISO 15693   26
 *   0E              ISO 15693 ASK10   26        8E              ISO 15693   53
 */
bool PN5180_loadRFConfig(uint8_t txConf, uint8_t rxConf) {
    uint8_t cmd[3] = {PN5180_LOAD_RF_CONFIG, txConf, rxConf};

    pn5180.transceiveCommand(cmd, 3, NULL, 0);

    return true;
}

/*
 * RF_ON - 0x16
 * This command is used to switch on the internal RF field. If enabled the TX_RFON_IRQ is
 * set after the field is switched on.
 */
bool PN5180_setRF_on() {
    uint8_t cmd[2] = {PN5180_RF_ON, 0x00};

    pn5180.transceiveCommand(cmd, 2, NULL, 0);

    while (0 == (TX_RFON_IRQ_STAT & pn5180.getIRQStatus()))
        ; // wait for RF field to set up
    pn5180.clearIRQStatus(TX_RFON_IRQ_STAT);
    return true;
}

/*
 * RF_OFF - 0x17
 * This command is used to switch off the internal RF field. If enabled, the TX_RFOFF_IRQ
 * is set after the field is switched off.
 */
bool PN5180_setRF_off() {
    uint8_t cmd[2] = {PN5180_RF_OFF, 0x00};

    pn5180.transceiveCommand(cmd, 2, NULL, 0);

    while (0 == (TX_RFOFF_IRQ_STAT & pn5180.getIRQStatus()))
        ; // wait for RF field to shut down
    pn5180.clearIRQStatus(TX_RFOFF_IRQ_STAT);
    return true;
}

//---------------------------------------------------------------------------------------------

/*
11.4.3.1 A Host Interface Command consists of either 1 or 2 SPI frames depending whether the
host wants to write or read data from the PN5180. An SPI Frame consists of multiple
bytes.

All commands are packed into one SPI Frame. An SPI Frame consists of multiple bytes.
No NSS toggles allowed during sending of an SPI frame.

For all 4 byte command parameter transfers (e.g. register values), the payload
parameters passed follow the little endian approach (Least Significant Byte first).

Direct Instructions are built of a command code (1 Byte) and the instruction parameters
(max. 260 bytes). The actual payload size depends on the instruction used.
Responses to direct instructions contain only a payload field (no header).
All instructions are bound to conditions. If at least one of the conditions is not fulfilled, an exception is
raised. In case of an exception, the IRQ line of PN5180 is asserted and corresponding interrupt
status register contain information on the exception.
*/

/*
 * A Host Interface Command consists of either 1 or 2 SPI frames depending whether the
 * host wants to write or read data from the PN5180. An SPI Frame consists of multiple
 * bytes.
 * All commands are packed into one SPI Frame. An SPI Frame consists of multiple bytes.
 * No NSS toggles allowed during sending of an SPI frame.
 * For all 4 byte command parameter transfers (e.g. register values), the payload
 * parameters passed follow the little endian approach (Least Significant Byte first).
 * The BUSY line is used to indicate that the system is BUSY and cannot receive any data
 * from a host. Recommendation for the BUSY line handling by the host:
 * 1. Assert NSS to Low
 * 2. Perform Data Exchange
 * 3. Wait until BUSY is high
 * 4. Deassert NSS
 * 5. Wait until BUSY is low
 * If there is a parameter error, the IRQ is set to ACTIVE and a GENERAL_ERROR_IRQ is set.
 */
bool PN5180_transceiveCommand(uint8_t *sendBuffer, size_t sendBufferLen, uint8_t *recvBuffer, size_t recvBufferLen) {
    // 0.
    while (GPIO_PIN_LOW != pn5180.digitalRead(pn5180.PN5180_BUSY))
        ; // wait until busy is low
    // 1.
    pn5180.digitalWrite(pn5180.PN5180_NSS, GPIO_PIN_LOW);
    HAL_Delay(2);
    // 2.
    UtilSpiSendWait(sendBuffer, sendBufferLen);
    // 3.
    while (GPIO_PIN_HIGH == pn5180.digitalRead(pn5180.PN5180_BUSY))
        ; // wait until BUSY is high
    // 4.
    // pn5180.digitalWrite(pn5180.PN5180_NSS, GPIO_PIN_HIGH);
    // HAL_Delay(1);
    // 5.
    // while (GPIO_PIN_LOW != pn5180.digitalRead(pn5180.PN5180_BUSY))
    //     ; // wait unitl BUSY is low

    // check, if write-only
    //
    if ((0 == recvBuffer) || (0 == recvBufferLen)) return true;

    // 1.
    pn5180.digitalWrite(pn5180.PN5180_NSS, GPIO_PIN_LOW);
    HAL_Delay(2);
    // 2.
    UtilSpiReadWait(recvBuffer, recvBufferLen);
    // 3.
    // while (GPIO_PIN_HIGH != pn5180.digitalRead(pn5180.PN5180_BUSY))
    //     ; // wait until BUSY is high
    // 4.
    pn5180.digitalWrite(pn5180.PN5180_NSS, GPIO_PIN_HIGH);
    HAL_Delay(1);
    // 5.
    // while (GPIO_PIN_LOW != pn5180.digitalRead(pn5180.PN5180_BUSY))
    //     ; // wait until BUSY is low

    return true;
}

/*
 * Reset NFC device
 */
void PN5180_reset() {
    pn5180.digitalWrite(pn5180.PN5180_RST, GPIO_PIN_LOW); // at least 10us required
    HAL_Delay(10);
    pn5180.digitalWrite(pn5180.PN5180_RST, GPIO_PIN_HIGH); // 2ms to ramp up required
    HAL_Delay(10);

    while (0 == (IDLE_IRQ_STAT & pn5180.getIRQStatus()))
        ; // wait for system to start up

    pn5180.clearIRQStatus(0xffffffff); // clear all flags
}

/**
 * @name  getInterrrupt
 * @desc  read interrupt status register and clear interrupt status
 */
uint32_t PN5180_getIRQStatus() {
    uint32_t irqStatus;
    pn5180.readRegister(IRQ_STATUS, &irqStatus);
    return irqStatus;
}

bool PN5180_clearIRQStatus(uint32_t irqMask) {
    return pn5180.writeRegister(IRQ_CLEAR, irqMask);
}

uint8_t PN5180_getTransceiveState() {

    uint32_t rfStatus;
    if (!pn5180.readRegister(RF_STATUS, &rfStatus)) {
        return (0);
    }

    /*
   * TRANSCEIVE_STATEs:
   *  0 - idle
   *  1 - wait transmit
   *  2 - transmitting
   *  3 - wait receive
   *  4 - wait for data
   *  5 - receiving
   *  6 - loopback
   *  7 - reserved
   */
    uint8_t state = ((rfStatus >> 24) & 0x07);
    return (state);
}

int8_t PN5180_digitalRead(uint8_t pin[2]) {
    return UtilGpioRead(pin[0], pin[1]);
}
void PN5180_digitalWrite(uint8_t pin[2], GPIO_PIN_STATE state) {
    UtilGpio(pin[0], pin[1], state);
}


void PN5180_Init() {
    pn5180.PN5180 = PN5180_PN5180;
    pn5180.digitalRead = PN5180_digitalRead;
    pn5180.digitalWrite = PN5180_digitalWrite;
    pn5180.begin = PN5180_begin;
    pn5180.end = PN5180_end;
    pn5180.writeRegister = PN5180_writeRegister;
    pn5180.writeRegisterWithOrMask = PN5180_writeRegisterWithOrMask;
    pn5180.writeRegisterWithAndMask = PN5180_writeRegisterWithAndMask;
    pn5180.readRegister = PN5180_readRegister;
    pn5180.readEEprom = PN5180_readEEprom;
    pn5180.sendData = PN5180_sendData;
    pn5180.readData = PN5180_readData;
    pn5180.loadRFConfig = PN5180_loadRFConfig;
    pn5180.setRF_on = PN5180_setRF_on;
    pn5180.setRF_off = PN5180_setRF_off;
    pn5180.reset = PN5180_reset;
    pn5180.getIRQStatus = PN5180_getIRQStatus;
    pn5180.clearIRQStatus = PN5180_clearIRQStatus;
    pn5180.getTransceiveState = PN5180_getTransceiveState;
    pn5180.transceiveCommand = PN5180_transceiveCommand;


    uint8_t SSpin[2] = {'A', 4};
    uint8_t BUSYpin[2] = {'B', 10};
    uint8_t RSTpin[2] = {'B', 1};
    pn5180.PN5180(SSpin,
                  BUSYpin,
                  RSTpin);

    pn5180.begin();
    while (1) {
        uint8_t buff[2];
        pn5180.readEEprom(FIRMWARE_VERSION, buff, 2);
        HAL_Delay(10);
    }
    pn5180.end();
}