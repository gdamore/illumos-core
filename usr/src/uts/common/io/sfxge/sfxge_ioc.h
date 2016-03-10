/*
 * Copyright (c) 2008-2015 Solarflare Communications Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of the FreeBSD Project.
 */

#ifndef	_SYS_SFXGE_IOC_H
#define	_SYS_SFXGE_IOC_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <sys/types.h>

/* Ensure no ambiguity over structure layouts */
#pragma pack(1)

#define	SFXGE_IOC	('S' << 24 | 'F' << 16 | 'C' << 8)

#define	SFXGE_STOP_IOC	(SFXGE_IOC | 0x01)
#define	SFXGE_START_IOC	(SFXGE_IOC | 0x02)

/* MDIO was SFXGE_IOC 0x03 */

/* I2C was SFXGE_IOC 0x04 */

/* SPI was SFXGE_IOC 0x05 */

/* BAR */

#define	SFXGE_BAR_IOC	(SFXGE_IOC | 0x06)

typedef	struct sfxge_bar_ioc_s {
	uint32_t	sbi_op;
	uint32_t	sbi_addr;
	uint32_t	sbi_data[4];
} sfxge_bar_ioc_t;

#define	SFXGE_BAR_OP_READ	0x00000001
#define	SFXGE_BAR_OP_WRITE	0x00000002

/* PCI */

#define	SFXGE_PCI_IOC	(SFXGE_IOC | 0x07)

typedef	struct sfxge_pci_ioc_s {
	uint32_t	spi_op;
	uint8_t		spi_addr;
	uint8_t		spi_data;
} sfxge_pci_ioc_t;

#define	SFXGE_PCI_OP_READ	0x00000001
#define	SFXGE_PCI_OP_WRITE	0x00000002

/* MAC */

#define	SFXGE_MAC_IOC	(SFXGE_IOC | 0x08)

typedef	struct sfxge_mac_ioc_s {
	uint32_t	smi_op;
	uint32_t	smi_data;
} sfxge_mac_ioc_t;

#define	SFXGE_MAC_OP_LOOPBACK	0x00000001

/* PHY */

#define	SFXGE_PHY_IOC	(SFXGE_IOC | 0x09)

typedef	struct sfxge_phy_ioc_s {
	uint32_t	spi_op;
	uint32_t	spi_data;
} sfxge_phy_ioc_t;

#define	SFXGE_PHY_OP_LOOPBACK	0x00000001
#define	SFXGE_PHY_OP_LINK	0x00000002
#define	SFXGE_PHY_OP_LED	0x00000003

/* SRAM */

#define	SFXGE_SRAM_IOC	(SFXGE_IOC | 0x0a)

typedef	struct sfxge_sram_ioc_s {
	uint32_t	ssi_op;
	uint32_t	ssi_data;
} sfxge_sram_ioc_t;

#define	SFXGE_SRAM_OP_TEST	0x00000001

/* TX */

#define	SFXGE_TX_IOC	(SFXGE_IOC | 0x0b)

typedef	struct sfxge_tx_ioc_s {
	uint32_t	sti_op;
	uint32_t	sti_data;
} sfxge_tx_ioc_t;

#define	SFXGE_TX_OP_LOOPBACK	0x00000001

/* RX */

#define	SFXGE_RX_IOC	(SFXGE_IOC | 0x0c)

typedef	struct sfxge_rx_ioc_s {
	uint32_t	sri_op;
	uint32_t	sri_data;
} sfxge_rx_ioc_t;

#define	SFXGE_RX_OP_LOOPBACK	0x00000001

/* NVRAM */

#define	SFXGE_NVRAM_IOC	(SFXGE_IOC | 0x0d)

typedef	struct sfxge_nvram_ioc_s {
	uint32_t	sni_op;
	uint32_t	sni_type;
	uint32_t	sni_offset;
	uint32_t	sni_size;
	uint32_t	sni_subtype;
	uint16_t	sni_version[4];		/* get/set_ver */
	/*
	 * Streams STRMSGSZ limit (default 64kb)
	 * See write(2) and I_STR in streamio(7i)
	 */
	uint8_t		sni_data[32*1024];	/* read/write */
} sfxge_nvram_ioc_t;

#define	SFXGE_NVRAM_OP_SIZE		0x00000001
#define	SFXGE_NVRAM_OP_READ		0x00000002
#define	SFXGE_NVRAM_OP_WRITE		0x00000003
#define	SFXGE_NVRAM_OP_ERASE		0x00000004
#define	SFXGE_NVRAM_OP_GET_VER		0x00000005
#define	SFXGE_NVRAM_OP_SET_VER		0x00000006

#define	SFXGE_NVRAM_TYPE_BOOTROM	0x00000001
#define	SFXGE_NVRAM_TYPE_BOOTROM_CFG	0x00000002
#define	SFXGE_NVRAM_TYPE_MC		0x00000003
#define	SFXGE_NVRAM_TYPE_MC_GOLDEN	0x00000004
#define	SFXGE_NVRAM_TYPE_PHY		0x00000005
#define	SFXGE_NVRAM_TYPE_NULL_PHY	0x00000006
#define	SFXGE_NVRAM_TYPE_FPGA		0x00000007
#define	SFXGE_NVRAM_TYPE_FCFW		0x00000008
#define	SFXGE_NVRAM_TYPE_CPLD		0x00000009
#define	SFXGE_NVRAM_TYPE_FPGA_BACKUP	0x0000000a
#define	SFXGE_NVRAM_TYPE_DYNAMIC_CFG	0x0000000b


/* PHY BIST */

#define	SFXGE_PHY_BIST_IOC	(SFXGE_IOC | 0x0e)

typedef	struct sfxge_phy_bist_ioc_s {
	boolean_t	spbi_break_link;
	uint8_t		spbi_status_a;
	uint8_t		spbi_status_b;
	uint8_t		spbi_status_c;
	uint8_t		spbi_status_d;
	uint16_t	spbi_length_ind_a;
	uint16_t	spbi_length_ind_b;
	uint16_t	spbi_length_ind_c;
	uint16_t	spbi_length_ind_d;
} sfxge_phy_bist_ioc_t;

#define	SFXGE_PHY_BIST_CABLE_OK			0
#define	SFXGE_PHY_BIST_CABLE_INVALID		1
#define	SFXGE_PHY_BIST_CABLE_OPEN		2
#define	SFXGE_PHY_BIST_CABLE_INTRAPAIRSHORT	3
#define	SFXGE_PHY_BIST_CABLE_INTERPAIRSHORT	4
#define	SFXGE_PHY_BIST_CABLE_BUSY		5
#define	SFXGE_PHY_BIST_CABLE_UNKNOWN		6

/* MCDIv1 */

/* Legacy IOC for MCDIv1 protocol - do not use in new code */
#define	SFXGE_MCDI_IOC	(SFXGE_IOC | 0x0f)

typedef	struct sfxge_mcdi_ioc_s {
	uint8_t		smi_payload[256];
	uint8_t		smi_cmd;
	uint8_t		smi_len; /* In and out */
	uint8_t		smi_rc;
} sfxge_mcdi_ioc_t;

/* Reset the NIC */

#define	SFXGE_NIC_RESET_IOC	(SFXGE_IOC | 0x10)

/* VPD */

#define	SFXGE_VPD_IOC	(SFXGE_IOC | 0x11)

#define	SFXGE_VPD_MAX_PAYLOAD 0x100

typedef	struct sfxge_vpd_ioc_s {
	uint8_t		svi_op;
	uint8_t		svi_tag;
	uint16_t	svi_keyword;
	uint8_t		svi_len; /* In or out */
	uint8_t		svi_payload[SFXGE_VPD_MAX_PAYLOAD]; /* In or out */
} sfxge_vpd_ioc_t;

#define	SFXGE_VPD_OP_GET_KEYWORD	0x00000001
#define	SFXGE_VPD_OP_SET_KEYWORD	0x00000002

/* MCDIv2 */

#define	SFXGE_MCDI2_IOC	(SFXGE_IOC | 0x12)

typedef	struct sfxge_mcdi2_ioc_s {
	uint8_t		smi_payload[1024];
	uint32_t	smi_cmd;
	uint32_t	smi_len; /* In and out */
	uint32_t	smi_rc;
} sfxge_mcdi2_ioc_t;


#pragma pack()

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_SFXGE_IOC_H */
