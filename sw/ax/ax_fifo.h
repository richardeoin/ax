/*
 * Register definitions for ax5043/ax5243
 * Copyright (C) 2016  Richard Meadows <richardeoin>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef AX_FIFO_H
#define AX_FIFO_H

/**
 * See AND9347-D-AX5043 Programming Manual.PDF
 */

/**
 * Chunk Sizes
 */
#define AX_FIFO_CHUNK_NO_PAYLOAD	(0 << 5)
#define AX_FIFO_CHUNK_SINGLE_BYTE	(1 << 5)
#define AX_FIFO_CHUNK_TWO_BYTE		(2 << 5)
#define AX_FIFO_CHUNK_THREE_BYTE	(3 << 5)
#define AX_FIFO_CHUNK_VARIABLE		(7 << 5)

/**
 * Chunk Types
 */
#define AX_FIFO_CHUNK_NOP			0x00 /* No Operation */
#define AX_FIFO_CHUNK_DATA			0xE1 /* Data */
/* Transmit */
#define AX_FIFO_CHUNK_TXCTRL		0x3C /* Transmit Control (Antenna, Power Amp) */
#define AX_FIFO_CHUNK_REPEATDATA	0x62 /* Repeat Data */
#define AX_FIFO_CHUNK_TXPWR			0xFD /* Transmit Power */
/* Receive */
#define AX_FIFO_CHUNK_RSSI			0x31 /* RSSI */
#define AX_FIFO_CHUNK_FREQOFFS		0x52 /* Frequency Offset */
#define AX_FIFO_CHUNK_ANTRSSI2		0x55 /* Background Noise Calculation RSSI */
#define AX_FIFO_CHUNK_TIMER			0x70 /* Timer */
#define AX_FIFO_CHUNK_RFFREQOFFS	0x73 /* RF Frequency Offset */
#define AX_FIFO_CHUNK_DATARATE		0x74 /* Datarate */
#define AX_FIFO_CHUNK_ANTRSSI3		0x75 /* Antenna Selection RSSI */

typedef struct ax_rx_chunk {
  uint8_t chunk_t;
  union {
    struct {
      uint8_t flags;
      uint16_t length;
      uint8_t data[0x100];
    } data;
    int16_t rssi;
    uint16_t freqoffs;
    struct {
      uint8_t rssi;
      uint8_t bgndnoise;
    } antrssi2;
    uint32_t timer;
    int32_t rffreqoffs;
    uint32_t datarate;
    struct {
      uint8_t ant0rssi;
      uint8_t ant1rssi;
      uint8_t bgndnoise;
    } antrssi3;
  } chunk;
} ax_rx_chunk;

/**
 * TXCTRL Command
 */
#define AX_FIFO_TXCTRL_SETTX	(1 << 6) /* Copy TXSE and TXDIFF to MODCFGA */
#define AX_FIFO_TXCTRL_TXSE		(1 << 5)
#define AX_FIFO_TXCTRL_TXDIFF	(1 << 4)
#define AX_FIFO_TXCTRL_SETANT	(1 << 3) /* Copy ANTSTATE to DIVERSITY */
#define AX_FIFO_TXCTRL_ANTSTATE	(1 << 2)
#define AX_FIFO_TXCTRL_SETPA	(1 << 1) /* Copy PASTATE to PWRAMP */
#define AX_FIFO_TXCTRL_PASTATE	(1 << 0)

/**
 * Transmit DATA Command
 */
#define AX_FIFO_TXDATA_UNENC	(1 << 5) /* Bypass framing and encoder */
#define AX_FIFO_TXDATA_RAW		(1 << 4) /* Bypass framing */
#define AX_FIFO_TXDATA_NOCRC	(1 << 3) /* Don't generate CRC */
#define AX_FIFO_TXDATA_RESIDUE	(1 << 2) /* Residue mode on last byte */
#define AX_FIFO_TXDATA_PKTEND	(1 << 1) /* END flag */
#define AX_FIFO_TXDATA_PKTSTART	(1 << 0) /* START flag */

/**
 * Receive DATA Command
 */
#define AX_FIFO_RXDATA_ABORT	(1 << 6) /* Packet has been aborted */
#define AX_FIFO_RXDATA_SIZEFAIL	(1 << 5) /* Size checks failed */
#define AX_FIFO_RXDATA_ADDRFAIL	(1 << 4) /* Address checks failed */
#define AX_FIFO_RXDATA_CRCFAIL	(1 << 3) /* CRC check failed */
#define AX_FIFO_RXDATA_RESIDUE	(1 << 2) /* Residue mode on last byte */
#define AX_FIFO_RXDATA_PKTEND	(1 << 1) /* END flag */
#define AX_FIFO_RXDATA_PKTSTART	(1 << 0) /* START flag */


#endif  /* AX_FIFO_H */
