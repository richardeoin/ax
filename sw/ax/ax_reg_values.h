/*
 * Register values definitions for ax5043/ax5243
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

/**
 * See AND9347-D-AX5043 Programming Manual.PDF
 */

/**
 * Status field
 */
#define AX_STATUS_POWER_READY				(1 << 15)
#define AX_STATUS_PLL_LOCK					(1 << 14)
#define AX_STATUS_FIFO_OVERFLOW				(1 << 13)
#define AX_STATUS_FIFO_UNDERFLOW			(1 << 12)
#define AX_STATUS_THRESHOLD_FREE			(1 << 11)
#define AX_STATUS_THRESHOLD_COUNT			(1 << 10)
#define AX_STATUS_FIFO_FULL					(1 << 9)
#define AX_STATUS_FIFO_EMPTY				(1 << 8)
#define AX_STATUS_PWRGOOD					(1 << 7)
#define AX_STATUS_PWR_INTERRUPT_PENDING		(1 << 6)
#define AX_STATUS_RADIO_EVENT_PENDING		(1 << 5)
#define AX_STATUS_XTAL_OSCILLATOR_RUNNING	(1 << 4)
#define AX_STATUS_WAKEUP_INTERRUPT_PENDING	(1 << 3)
#define AX_STATUS_LPOSC_INTERRUPT_PENDING	(1 << 2)
#define AX_STATUS_GPADC_INTERRUPT_PENDING	(1 << 1)

/**
 * Revision
 */
#define AX_SILICONREVISION	0x51

/**
 * Scratch
 */
#define AX_SCRATCH			0xC5

/**
 * Power Mode
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
#define AX_MODULATION_ASK			0
#define AX_MODULATION_ASK_COHERENT	1
#define AX_MODULATION_PSK			4
#define AX_MODULATION_CW			5 /* not documented.. */
#define AX_MODULATION_OQSK			6
#define AX_MODULATION_MSK			7
#define AX_MODULATION_FSK			8
#define AX_MODULATION_4_FSK			9
#define AX_MODULATION_AFSK			10
#define AX_MODULATION_FM			11
#define AX_MODULATION_HALFSPEED		(1 << 4)

/**
 * Encoding
 */
#define AX_ENC_INV		(1 << 0) /* Invert data */
#define AX_ENC_DIFF		(1 << 1) /* Differential Encode/Decode data */
#define AX_ENC_SCRAM	(1 << 2) /* Enable Scrambler / Descrambler */
#define AX_ENC_MANCH	(1 << 3) /* Enable manchester encoding/decoding */
#define AX_ENC_NOSYNC	(1 << 4) /* Disable Dibit synchronisation in 4-FSK mode */

#define AX_ENC_NRZ		(0x0)      /* NRZ - level encodes data */
#define AX_ENC_NRZI		(0x3)      /* NRZI - inversion encodes data */
#define AX_ENC_FM0		(0xA)      /* FM0 - biphase space */
#define AX_ENC_FM1		(0xB)      /* FM1 - biphase mark */

/**
 * Framing
 */
#define AX_FRAMING_ABORT			(1 << 0)
#define AX_FRAMING_MODE_RAW			(0 << 1)
#define AX_FRAMING_MODE_RAW_SOFT_BITS		(1 << 1)
#define AX_FRAMING_MODE_HDLC			(2 << 1)
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
#define AX_FEC_POS		(1 << 4) /* Enable noninverted Interleaver Synchronisation */
#define AX_FEC_NEG		(1 << 5) /* Enable inverted Interleaver Synchronisation */
#define AX_FEC_RSTVITERBI	(1 << 6) /* Reset Viterbi Decoder */
#define AX_FEC_SHORTMEM		(1 << 7) /* Shorten Backtrack Memory */

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
 * FIFO Status (FIFOSAT)
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

/**
 * PLL LOOP
 */
#define AX_PLLLOOP_EXTERNAL_FILTER				0
#define AX_PLLLOOP_INTERNAL_FILTER_BW_100_KHZ	1
#define AX_PLLLOOP_INTERNAL_FILTER_BW_200_KHZ	2
#define AX_PLLLOOP_INTERNAL_FILTER_BW_500_KHZ	3
#define AX_PLLLOOP_FILTER_ENABLE_EXTERNAL		(1 << 2)
#define AX_PLLLOOP_FILTER_DIRECT				(1 << 3)
#define AX_PLLLOOP_FREQSEL_A					(0 << 7)
#define AX_PLLLOOP_FREQSEL_B					(1 << 7)

/**
 * PLL VCO Divider
 */
#define AX_PLLVCODIV_DIVIDE_1	0
#define AX_PLLVCODIV_DIVIDE_2	1
#define AX_PLLVCODIV_DIVIDE_4	2
#define AX_PLLVCODIV_DIVIDE_8	3
#define AX_PLLVCODIV_RF_DIVIDER_NONE			(0 << 2)
#define AX_PLLVCODIV_RF_DIVIDER_DIV_TWO			(1 << 2)

#define AX_PLLVCODIV_RF_INTERNAL_VCO					(0 << 4)
#define AX_PLLVCODIV_RF_INTERNAL_VCO_EXTERNAL_INDUCTOR	(3 << 4)
#define AX_PLLVCODIV_RF_EXTERNAL_VCO					(1 << 4)


/**
 * PLL Ranging
 */
#define AX_PLLRANGING_RNG_START		(1 << 4)
#define AX_PLLRANGING_RNGERR		(1 << 5)
#define AX_PLLRANGING_PLL_LOCK		(1 << 6)
#define AX_PLLRANGING_STICKY_LOCK	(1 << 7)

/**
 * 5.15 Receiver Parameters ----------------------------------------
 */

/**
 * Max RF offset
 */
#define AX_MAXRFOFFSET_FREQOFFSCORR_FIRST_LO	(1 << 23)
#define AX_MAXRFOFFSET_FREQOFFSCORR_SECOND_LO	(0 << 23)

/**
 * FREQGAIN registers
 */
/* A */
#define AX_FREQGAIN_BB_FREQUENCY_RECOVERY_AMPLITUDE_GATE	(1 << 4)
#define AX_FREQGAIN_BB_FREQUENCY_RECOVERY_WRAP_HALF_MODULO	(1 << 5)
#define AX_FREQGAIN_BB_FREQUENCY_RECOVERY_WRAP_MODULO		(1 << 6)
#define AX_FREQGAIN_BB_FREQUENCY_RECOVERY_WRAP_LIMIT		(1 << 7)
/* B */
#define AX_FREQGAIN_BB_FREQUENCY_RECOVERY_AVERAGE_CONSECUTIVE	(1 << 6)
#define AX_FREQGAIN_BB_FREQUENCY_RECOVERY_FREEZE_LOOP			(1 << 7)
/* D */
#define AX_FREQGAIN_RF_FREQUENCY_RECOVERY_FREEZE_LOOP		(1 << 7)

/**
 * AMPLGAIN
 */
#define AX_AMPLGAIN_TRY_TO_CORRECT_AMPLITUDE_ON_AGC_JUMP	(1 << 6)
#define AX_AMPLGAIN_AMPLITUDE_RECOVERY_PEAKDET		(0 << 7)
#define AX_AMPLGAIN_AMPLITUDE_RECOVERY_AVERAGING	(1 << 7)

/**
 * FOURFSK
 */
#define AX_FOURFSK_ENABLE_DEVIATION_UPDATE	(1 << 4)

/**
 * 5.16 Transmitter Parameters ----------------------------------------
 */

/**
 * Modulation Config - Frequency (MODCFGF)
 */
#define AX_MODCFGF_FREQSHAPE_UNSHAPED			(0 << 0)
#define AX_MODCFGF_FREQSHAPE_GAUSSIAN_BT_0_3	(2 << 0)
#define AX_MODCFGF_FREQSHAPE_GAUSSIAN_BT_0_5	(3 << 0)

/**
 * FSK Deviation - FM Mode
 */
#define AX_FSKDEV_FMSHIFT_DIV_32768	0
#define AX_FSKDEV_FMSHIFT_DIV_16384	1
#define AX_FSKDEV_FMSHIFT_DIV_8192	2
#define AX_FSKDEV_FMSHIFT_DIV_4096	3
#define AX_FSKDEV_FMSHIFT_DIV_2048	4
#define AX_FSKDEV_FMSHIFT_DIV_1024	5
#define AX_FSKDEV_FMSHIFT_DIV_512	6
#define AX_FSKDEV_FMSHIFT_DIV_256	7
#define AX_FSKDEV_FMINPUT_GPADC13	(0 << 8)
#define AX_FSKDEV_FMINPUT_GPADC1	(1 << 8)
#define AX_FSKDEV_FMINPUT_GPADC2	(2 << 8)
#define AX_FSKDEV_FMINPUT_GPADC3	(3 << 8)
#define AX_FSKDEV_FMSEXT			(1 << 14) /* ADC Sign Extension */
#define AX_FSKDEV_FMOFFS			(1 << 15) /* ADC Offset Subtract */

/**
 * Modulation Config - Amplitude (MODCFGA)
 */
#define AX_MODCFGA_TXDIFF					(1 << 0) /* Differential TX */
#define AX_MODCFGA_TXSE						(1 << 1) /* Single Ended TX */
#define AX_MODCFGA_AMPLSHAPE_RAISED_COSINE	(1 << 2) /* RC Filter on TX */
#define AX_MODCFGA_SLOWRAMP_1_BIT_TIME		(0 << 4) /* 1 bit time startup */
#define AX_MODCFGA_SLOWRAMP_2_BIT_TIME		(1 << 4) /* 2 bit time startup */
#define AX_MODCFGA_SLOWRAMP_4_BIT_TIME		(2 << 4) /* 4 bit time startup */
#define AX_MODCFGA_SLOWRAMP_8_BIT_TIME		(3 << 4) /* 8 bit time startup */
#define AX_MODCFGA_PTTLCK_GATE_ENABLE		(1 << 6) /* Disable TX if PLL unlocks */
#define AX_MODCFGA_BROWN_GATE_ENABLE		(1 << 7) /* Disable TX if brownout */

/**
 * 5.17 PLL Parameters ----------------------------------------
 */

/**
 * PLL VCO Current
 */
#define AX_PLLVCOI_ENABLE_MANUAL	(1 << 7)

/**
 * PLL Ranging Clock
 */
#define AX_PLLRNGCLK_DIV_256	0
#define AX_PLLRNGCLK_DIV_512	1
#define AX_PLLRNGCLK_DIV_1024	2
#define AX_PLLRNGCLK_DIV_2048	3
#define AX_PLLRNGCLK_DIV_4096	4
#define AX_PLLRNGCLK_DIV_8192	5
#define AX_PLLRNGCLK_DIV_16384	6
#define AX_PLLRNGCLK_DIV_32768	7

/**
 * 5.22 Packet Controller ----------------------------------------
 */

#define AX_PKT_MAXIMUM_CHUNK_SIZE_1_BYTES		1
#define AX_PKT_MAXIMUM_CHUNK_SIZE_2_BYTES		2
#define AX_PKT_MAXIMUM_CHUNK_SIZE_4_BYTES		3
#define AX_PKT_MAXIMUM_CHUNK_SIZE_8_BYTES		4
#define AX_PKT_MAXIMUM_CHUNK_SIZE_16_BYTES		5
#define AX_PKT_MAXIMUM_CHUNK_SIZE_32_BYTES		6
#define AX_PKT_MAXIMUM_CHUNK_SIZE_64_BYTES		7
#define AX_PKT_MAXIMUM_CHUNK_SIZE_128_BYTES		8
#define AX_PKT_MAXIMUM_CHUNK_SIZE_160_BYTES		9
#define AX_PKT_MAXIMUM_CHUNK_SIZE_192_BYTES		10
#define AX_PKT_MAXIMUM_CHUNK_SIZE_224_BYTES		11
#define AX_PKT_MAXIMUM_CHUNK_SIZE_240_BYTES		12

#define AX_PKT_FLAGS_RSSI_UNITS_MICROSECONDS	(0 << 0)
#define AX_PKT_FLAGS_RSSI_UNITS_BIT_TIME		(1 << 0)
#define AX_PKT_FLAGS_AGC_UNITS_MICROSECONDS		(0 << 1)
#define AX_PKT_FLAGS_AGC_UNITS_BIT_TIME			(1 << 1)
#define AX_PKT_FLAGS_ENABLE_BGND_RSSI			(1 << 2)
#define AX_PKT_FLAGS_AGC_SETTLING_DETECTION		(1 << 3)

#define AX_PKT_STORE_TIMER					(1 << 0)
#define AX_PKT_STORE_FREQUENCY_OFFSET		(1 << 1)
#define AX_PKT_STORE_RF_OFFSET				(1 << 2)
#define AX_PKT_STORE_DATARATE_OFFSET		(1 << 3)
#define AX_PKT_STORE_RSSI					(1 << 4)
#define AX_PKT_STORE_CRC_BYTES				(1 << 5)
#define AX_PKT_STORE_RSSI_ON_ANTENNA_SELECT	(1 << 6)

#define AX_PKT_ACCEPT_RESIDUE			(1 << 0)
#define AX_PKT_ACCEPT_ABORTED			(1 << 1)
#define AX_PKT_ACCEPT_CRC_FAILURES		(1 << 2)
#define AX_PKT_ACCEPT_ADDRESS_FAILURES	(1 << 3)
#define AX_PKT_ACCEPT_SIZE_FAILURES		(1 << 4)
#define AX_PKT_ACCEPT_MULTIPLE_CHUNKS	(1 << 5)

/**
 * 5.24 Low Power Oscillator --------------------------------------
 */
#define AX_LPOSC_ENABLE		(1 << 0)
#define AX_LPOSC_640_HZ		(0 << 1)
#define AX_LPOSC_10240_HZ	(1 << 1)
#define AX_LPOSC_IRQR		(1 << 2)
#define AX_LPOSC_IRQF		(1 << 3)
#define AX_LPOSC_CALIBF		(1 << 4)
#define AX_LPOSC_CALIBR		(1 << 5)
#define AX_LPOSC_OSC_DOUBLE	(1 << 6)
#define AX_LPOSC_OSC_INVERT	(1 << 7)

/**
 * 5.25 DAC --------------------------------------
 */
#define AX_DAC_INPUT_DACVALUE		(0)
#define AX_DAC_INPUT_TRKAMPLITUDE	(1)
#define AX_DAC_INPUT_TRKRFFREQUENCY	(2)
#define AX_DAC_INPUT_TRKFREQUENCY	(3)
#define AX_DAC_INPUT_FSKDEMOD		(4)
#define AX_DAC_INPUT_AFSKDEMOD		(5)
#define AX_DAC_INPUT_RXSOFTDATA		(6)
#define AX_DAC_INPUT_RSSI			(7)
#define AX_DAC_INPUT_SAMPLE_ROT_I	(8)
#define AX_DAC_INPUT_SAMPLE_ROT_Q	(9)
#define AX_DAC_INPUT_GPADC13		(10)

#define AX_DEC_CLKX2			(1 << 6)
#define AX_DAC_MODE_DELTA_SIGMA	(0 << 7)
#define AX_DAC_MODE_PWM			(1 << 7)
