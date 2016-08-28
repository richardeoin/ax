/*
 * Register values definitions for ax5243
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

#define AX_PWRMODE_POWERDOWN	0  /* 400nA */
#define AX_PWRMODE_DEEPSLEEP	1  /* 50nA */
#define AX_PWRMODE_STANDBY	5  /* 230µA */
#define AX_PWRMODE_FIFOON	7  /* 310µA */
#define AX_PWRMODE_SYNTHRX	8  /* 5mA */
#define AX_PWRMODE_FULLRX	9  /* 7-11mA */
#define AX_PWRMODE_WORRX	11 /* 500nA */
#define AX_PWRMODE_SYNTHTX	12 /* 5mA */
#define AX_PWRMODE_FULLTX	13 /* 6-70mA */

#define AX_PWRMODE_WDS		(1 << 4) /* Wakeup from Deep Sleep */
#define AX_PWRMODE_REFEN	(1 << 5) /* Power to internal reference */
#define AX_PWRMODE_XOEN		(1 << 6) /* Crystal Oscillator Enable */
#define AX_PWRMODE_RST		(1 << 7) /* Reset */

/**
 * Power Status
 */
#define AX_POWSTAT_SVIO		(1 << 0) /* IO Voltage Large Enough (not Brownout) */
#define AX_POWSTAT_SBEVMODEM	(1 << 1) /* Modem Domain Voltage Brownout Error (Inverted) */
#define AX_POWSTAT_SBEVANA	(1 << 2) /* Analog Domain Voltage Brownout Error (Inverted) */
#define AX_POWSTAT_SVMODEM	(1 << 3) /* Modem Domain Voltage Regulator Ready */
#define AX_POWSTAT_SVANA	(1 << 4) /* Analog Domain Voltage Regulator Ready */
#define AX_POWSTAT_SVREF	(1 << 5) /* Reference Voltage Regulator Ready */
#define AX_POWSTAT_SREF		(1 << 6) /* Reference Ready */
#define AX_POWSTAT_SSUM		(1 << 7) /* Summary Ready Status */

/**
 * Interrupt Control
 */
#define AX_IRQMFIFONOTEMPTY	(1 << 0) /* FIFO not empty interrupt */
#define AX_IRQMFIFONOTFULL	(1 << 1) /* FIFO not full interrupt */
#define AX_IRQMFIFOTHRCNT	(1 << 2) /* FIFO count > threshold interrupt */
#define AX_IRQMFIFOTHRFREE	(1 << 3) /* FIFO free > threshold interrupt */
#define AX_IRQMFIFOERROR	(1 << 4) /* FIFO error interrupt */
#define AX_IRQMPLLUNLOCK	(1 << 5) /* PLL lock lost interrupt */
#define AX_IRQMRADIOCTRL	(1 << 6) /* Radio Controller interrupt */
#define AX_IRQMPOWER		(1 << 7) /* Power interrupt */
#define AX_IRQMXTALREADY	(1 << 8) /* Crystal Oscillator Ready interrupt */
#define AX_IRQMWAKEUPTIMER	(1 << 9) /* Wakeup Timer interrupt */
#define AX_IRQMLPOSC		(1 << 10) /* Low Power Oscillator interrupt */
#define AX_IRQMGPADC		(1 << 11) /* GPADC interrupt */
#define AX_IRQMPLLRNGDONE	(1 << 12) /* PLL autoranging done interrupt */

/**
 * Radio Event Mask
 */
#define AX_REVMDONE		(1 << 0) /* Transmit or Receive Done Radio Event */
#define AX_REVMSETTLED		(1 << 1) /* PLL Settled Radio Event */
#define AX_REVMRADIOSTATECHG	(1 << 2) /* Radio State Changed Event */
#define AX_REVMRXPARAMSETCHG	(1 << 3) /* Receiver Parameter Set Changed Event */
#define AX_REVMFRAMECLK		(1 << 4) /* Frame Clock Event */

/**
 * Modulation
 */
#define AX_MODULATION_ASK		0
#define AX_MODULATION_ASK_COHERENT	1
#define AX_MODULATION_PSK		2
#define AX_MODULATION_OQSK		3
#define AX_MODULATION_MSK		4
#define AX_MODULATION_FSK		5
#define AX_MODULATION_4_FSK		6
#define AX_MODULATION_AFSK		7
#define AX_MODULATION_FM		8
#define AX_MODULATION_HALFSPEED		(1 << 4)

/**
 * Encoding
 */
#define AX_ENC_INV	(1 << 0) /* Invert data */
#define AX_ENC_DIFF	(1 << 1) /* Differential Encode/Decode data */
#define AX_ENC_SCRAM	(1 << 2) /* Enable Scrambler / Descrambler */
#define AX_ENC_MANCH	(1 << 3) /* Enable manchester encoding/decoding */
#define AX_ENC_NOSYNC	(1 << 4) /* Disable Dibit synchronisation in 4-FSK mode */

/**
 * Framing
 */
#define AX_FRAMING_ABORT			(1 << 0)
#define AX_FRAMING_MODE_RAW			(0 << 1)
#define AX_FRAMING_MODE_RAW_SOFT_BITS		(1 << 1)
#define AX_FRAMING_MODE_HDLE			(2 << 1)
#define AX_FRAMING_MODE_RAW_PATTERN_MATCH	(3 << 1)
#define AX_FRAMING_MODE_WIRELESS_MBUS		(4 << 1)
#define AX_FRAMING_MODE_WIRELESS_MBUS_4_TO_6	(5 << 1)
#define AX_FRAMING_CRCMODE_OFF			(0 << 4)
#define AX_FRAMING_CRCMODE_CCITT		(1 << 4)
#define AX_FRAMING_CRCMODE_CRC_16		(2 << 4)
#define AX_FRAMING_CRCMODE_DNP			(3 << 4)
#define AX_FRAMING_CRCMODE_CRC_32		(4 << 4)
#define AX_FRAMING_RX				(1 << 7)

/**
 * FEC
 */
#define AX_FEC_ENA		(1 << 0) /* Enable FEC (Convolutional Encoder) */
#define AX_FEC_POS		(1 << 2) /* Enable noninverted Interleaver Synchronisation */
#define AX_FEC_NEG		(1 << 3) /* Enable inverted Interleaver Synchronisation */
#define AX_FEC_RSTVITERBI	(1 << 4) /* Reset Viterbi Decoder */
#define AX_FEC_SHORTMEM		(1 << 5) /* Shorten Backtrack Memory */

/**
 * FEC Status
 */
#define AX_FECSTATUS_INVERTED_SYNC	(1 << 7)

/**
 * Radio State
 */
#define AX_RADIOSTATE_IDLE		0
#define AX_RADIOSTATE_POWERDOWN		1
#define AX_RADIOSTATE_TX_PLL_SETTLING	4
#define AX_RADIOSTATE_TX		6
#define AX_RADIOSTATE_TX_TAIL		7
#define AX_RADIOSTATE_RX_PLL_SETTLING	8
#define AX_RADIOSTATE_RX_ANTENNA_SELECTION	9
#define AX_RADIOSTATE_RX_PREAMBLE_1	12
#define AX_RADIOSTATE_RX_PREAMBLE_2	13
#define AX_RADIOSTATE_RX_PREAMBLE_3	14
#define AX_RADIOSTATE_RX		15

/**
 * XTAL Status
 */
#define AX_XTALSTATUS_RUNNING	(1 << 0)

/**
 * Pin State
 */
#define AX_PSSYSCLK	(1 << 0) /* Signal Level on Pin SYSCLK */
#define AX_PSDCLK	(1 << 1) /* Signal Level on Pin DCLK */
#define AX_PSDATA	(1 << 2) /* Signal Level on Pin DATA */
#define AX_PSIRQ	(1 << 3) /* Signal Level on Pin IRQ */
#define AX_PSANTSEL	(1 << 4) /* Signal Level on Pin ANTSEL */
#define AX_PSPWRAMP	(1 << 5) /* Signal Level on Pin PWRAMP */

/**
 * Sysclk Pin Status
 */
#define AX_SYSCLK_OUTPUT_0		0
#define AX_SYSCLK_OUTPUT_1		1
#define AX_SYSCLK_OUTPUT_Z		2
#define AX_SYSCLK_OUTPUT_INVERTED_XTAL	3
#define AX_SYSCLK_OUTPUT_XTAL		4
#define AX_SYSCLK_OUTPUT_XTAL_DIV_2	5
#define AX_SYSCLK_OUTPUT_XTAL_DIV_4	6
#define AX_SYSCLK_OUTPUT_XTAL_DIV_8	7
#define AX_SYSCLK_OUTPUT_XTAL_DIV_16	8
#define AX_SYSCLK_OUTPUT_XTAL_DIV_32	9
#define AX_SYSCLK_OUTPUT_XTAL_DIV_64	10
#define AX_SYSCLK_OUTPUT_XTAL_DIV_128	11
#define AX_SYSCLK_OUTPUT_XTAL_DIV_256	12
#define AX_SYSCLK_OUTPUT_XTAL_DIV_512	13
#define AX_SYSCLK_OUTPUT_XTAL_DIV_1024	14
#define AX_SYSCLK_OUTPUT_LPOSC		15
#define AX_SYSCLK_OUTPUT_TEST		16
#define AX_SYSCLK_PU_ENABLE		(1 << 7) /* Weak Pullup Enable */

/**
 * FIFO Status
 */
#define AX_FIFO_EMPTY		(1 << 0) /* FIFO is empty if 1 (SEE NOTE)  */
#define AX_FIFO_FULL		(1 << 1) /* FIFO is full if 1 */
#define AX_FIFO_UNDER		(1 << 2) /* FIFO underrun occured since last read of FIFOSTAT when 1 */
#define AX_FIFO_OVER		(1 << 3) /* FIFO overrun occured since last read of FIFOSTAT when 1 */
#define AX_FIFO_CNT_THR		(1 << 4) /* 1 if the FIFO count is > FIFOTHRESH */
#define AX_FIFO_FREE_THR	(1 << 5) /* 1 if the FIFO free space is > FIFOTHRESH */
#define AX_FIFO_AUTO_COMMIT	(1 << 7) /* If one, FIFO bytes are automatically committed on every write */

#define AX_FIFOCMD_NO_OPERATION		0
#define AX_FIFOCMD_CLEAR_FIFO_DATA	1
#define AX_FIFOCMD_CLEAR_FIFO_ERROR_FLAGS	2
#define AX_FIFOCMD_CLEAR_FIFO_DATA_AND_FLAGS	3
#define AX_FIFOCMD_COMMIT		4
#define AX_FIFOCMD_ROLLBACK		5
