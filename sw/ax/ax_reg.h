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

/**
 * See AND9347-D-AX5043 Programming Manual.PDF
 */

//typedef uint16_t ax_reg_address_t

#define AX_REG_SILICONREVISION	0x000	/* Silicon Revision */
#define AX_REG_SCRATCH			0x001	/* Scratch */
#define AX_REG_PWRMODE			0x002	/* Power Mode */

/**
 * Voltage Regulator
 */
#define AX_REG_POWSTAT			0x003	/* Power Management Status */
#define AX_REG_POWSTICKYSTAT	0x004	/* Power Management Sticky Status */
#define AX_REG_POWIRQMASK		0x005	/* Power Management Interrupt Mask */

/**
 * Interrupt control
 */
#define AX_REG_IRQMASK			0x006	/* IRQ Mask */
#define AX_REG_RADIOEVENTMASK	0x008	/* Radio Event Mask */
#define AX_REG_IRQINVERSION		0x00A	/* IRQ Inversion */
#define AX_REG_IRQREQUEST		0x00C	/* IRQ Request */
#define AX_REG_RADIOEVENTREQ	0x00E	/* Radio Event Request */

/**
 * Modulation & Framing
 */
#define AX_REG_MODULATION		0x010	/* Modulation */
#define AX_REG_ENCODING			0x011	/* Encoding */
#define AX_REG_FRAMING			0x012	/* Framing Mode */
#define AX_REG_CRCINIT			0x014	/* CRC Initial Value */

/**
 * FEC
 */
#define AX_REG_FEC				0x018	/* Forward Error Correction */
#define AX_REG_FECSYNC			0x019	/* Forward Error Correction Sync Threshold */
#define AX_REG_FECSTATUS		0x01A	/* Forward Error Correction Status */

/**
 * Status
 */
#define AX_REG_RADIOSTATE		0x01C	/* Radio Controller State */
#define AX_REG_XTALSTATUS		0x01D	/* Crystal Oscillator Status */

/**
 * Pin Configuration
 */
#define AX_REG_PINSTATE			0x020	/* Pin State */
#define AX_REG_PINFUNCSYSCLK	0x021	/* Pin Function SYSCLK */
#define AX_REG_PINFUNCDCLK		0x022	/* Pin Function DCLK */
#define AX_REG_PINFUNCDATA		0x023	/* Pin Function DATA */
#define AX_REG_PINFUNCIRQ		0x024	/* Pin Function IRQ */
#define AX_REG_PINFUNCANTSEL	0x025	/* Pin Function ANTSEL */
#define AX_REG_PINFUNCPWRAMP	0x026	/* Pin Function PWRAMP */
#define AX_REG_PWRAMP			0x027	/* PWRAMP Control */

/**
 * FIFO
 */
#define AX_REG_FIFOSTAT			0x028	/* FIFO Control */
#define AX_REG_FIFODATA			0x029	/* FIFO Data */
#define AX_REG_FIFOCOUNT		0x02A	/* Number of Words currently in FIFO */
#define AX_REG_FIFOFREE			0x02C	/* Number of Words that can be written to FIFO */
#define AX_REG_FIFOTHRESH		0x02E	/* FIFO Threshold */

/**
 * Synth
 */
#define AX_REG_PLLLOOP			0x030	/* PLL Loop Filter Settings */
#define AX_REG_PLLCPI			0x031	/* PL Charge Pump Current */
#define AX_REG_PLLVCODIV		0x032	/* PLL Divider Settings */
#define AX_REG_PLLRANGINGA		0x033	/* PLL Autoranging A */
#define AX_REG_FREQA			0x034	/* Frequency A */
#define AX_REG_PLLLOOPBOOST		0x038	/* PLL Loop Filter Settings (Boosted) */
#define AX_REG_PLLCPIBOOST		0x039	/* PLL Charge Pump Current (Boosted) */
#define AX_REG_PLLRANGINGB		0x03B	/* PLL Autoranging B */
#define AX_REG_FREQB			0x03C	/* Frequency B */

/**
 * Signal strength
 */
#define AX_REG_RSSI				0x040	/* Received Signal Strength Indicator */
#define AX_REG_BGNDRSSI			0x041	/* Background RSSI */
#define AX_REG_DIVERSITY		0x042	/* Antenna Diversity Configuration */
#define AX_REG_AGCCOUNTER		0x043	/* AGC Counter */

/**
 * Rx tracking
 */
#define AX_REG_TRKDATARATE		0x045	/* Datarate Tracking */
#define AX_REG_TRKAMPLITUDE		0x048	/* Amplitude Tracking */
#define AX_REG_TRKPHASE			0x04A	/* Phase Tracking */
#define AX_REG_TRKRFFREQ		0x04D	/* RF Frequency Tracking */
#define AX_REG_TRKFREQ			0x050	/* Frequency Tracking */
#define AX_REG_TRKFSKDEMOD		0x052	/* FSK Demodulator Tracking */
#define AX_REG_TRKAFSKDEMOD		0x054	/* AFSK Demodulator Tracking */

/**
 * Timer
 */
#define AX_REG_TIMER			0x059	/* 1MHz Timer */
#define AX_REG_WAKEUPTIMER		0x068	/* Wakeup Timer */
#define AX_REG_WAKEUP			0x06A	/* Wakeup Time */
#define AX_REG_WAKEUPFREQ		0x06C	/* Wakeup Frequency */
#define AX_REG_WAKEUPXOEARLY	0x06E	/* Wakeup Crystal Oscillator Early */

/**
 * Receiver parameters
 */
#define AX_REG_IFFREQ			0x100	/* 2nd LO / IF Frequency */
#define AX_REG_DECIMATION		0x102	/* Decimation Factor */
#define AX_REG_RXDATARATE		0x103	/* Receiver Datarate */
#define AX_REG_MAXDROFFSET		0x106	/* Maximum Receiver Datarate Offset */
#define AX_REG_MAXRFOFFSET		0x109	/* Maximum Receiver RF Offset */
#define AX_REG_FSKDMAX			0x10C	/* Four FSK Rx Maximum Deviation */
#define AX_REG_FSKDMIN			0x10E	/* Four FSK Rx Minimum Deviation */
#define AX_REG_AFSKSPACE		0x110	/* AFSK Space (0) Frequency */
#define AX_REG_AFSKMARK			0x112	/* AFSK Mark (1) Frequency */
#define AX_REG_AFSKCTRL			0x114	/* AFSK Control */
#define AX_REG_AMPLFILTER		0x115	/* Amplitude Filter */
#define AX_REG_FREQUENCYLEAK	0x116	/* Baseband Frequency Recovery Loop Leakiness */
#define AX_REG_RXPARAMSETS		0x117	/* Receiver Parameter Set Indirection */
#define AX_REG_RXPARAMCURSET	0x118	/* Receiver Parameter Current Set */

/**
 * Receiver Parameter Sets
 */
#define AX_REG_RX_PARAMETER0	0x120
#define AX_REG_RX_PARAMETER1	0x130
#define AX_REG_RX_PARAMETER2	0x140
#define AX_REG_RX_PARAMETER3	0x150

#define AX_RX_AGCGAIN			0x0	/* AGC Speed */
#define AX_RX_AGCTARGET			0x1	/* AGC Target */
#define AX_RX_AGCAHYST			0x2	/* AGC Analog Hysteresis */
#define AX_RX_AGCMINMAX			0x3	/* AGC Analog Update Behaviour */
#define AX_RX_TIMEGAIN			0x4	/* Time Estimator Bandwidth */
#define AX_RX_DRGAIN			0x5	/* Data Rate Estimator Bandwidth */
#define AX_RX_PHASEGAIN			0x6	/* Phase Estimator Bandwidth */
#define AX_RX_FREQUENCYGAINA	0x7	/* Frequency Estimator Bandwidth A */
#define AX_RX_FREQUENCYGAINB	0x8	/* Frequency Estimator Bandwidth B */
#define AX_RX_FREQUENCYGAINC	0x9	/* Frequency Estimator Bandwidth C */
#define AX_RX_FREQUENCYGAIND	0xA	/* Frequency Estimator Bandwidth D */
#define AX_RX_AMPLITUDEGAIN		0xB	/* Amplitude Estimator Bandwidth */
#define AX_RX_FREQDEV			0xC	/* Receiver Frequency Deviation */
#define AX_RX_FOURFSK			0xE	/* Four FSK Control */
#define AX_RX_BBOFFSRES			0xF	/* Baseband Offset Compensation Resistors */

/**
 * Transmitter Parameters
 */
#define AX_REG_MODCFGF			0x160	/* Modulator Configuration F */
#define AX_REG_FSKDEV			0x161	/* FSK Deviation */
#define AX_REG_MODCFGA			0x164	/* Modulator Configuration A */
#define AX_REG_TXRATE			0x165	/* Transmitter Bitrate */
#define AX_REG_TXPWRCOEFFA		0x168	/* Transmitter Predistortion Coefficient A */
#define AX_REG_TXPWRCOEFFB		0x16A	/* Transmitter Predistortion Coefficient B */
#define AX_REG_TXPWRCOEFFC		0x16C	/* Transmitter Predistortion Coefficient C */
#define AX_REG_TXPWRCOEFFD		0x16E	/* Transmitter Predistortion Coefficient D */
#define AX_REG_TXPWRCOEFFE		0x170	/* Transmitter Predistortion Coefficient E */

/**
 * PLL Paramters
 */
#define AX_REG_PLLVCOI			0x180	/* PLL VCO Current */
#define AX_REG_PLLVCOIR			0x181	/* PLL VCO Current Readback */
#define AX_REG_PLLLOCKDET		0x182	/* PLL Lock Detect Delay */
#define AX_REG_PLLRNGCLK		0x183	/* PLL Autoranging Clock */

/**
 * Crystal Oscillator
 */
#define AX_REG_XTALCAP			0x184	/* Crystal Oscillator Load Capacitance */

/**
 * Baseband
 */
#define AX_REG_BBTUNE			0x188	/* Baseband Tuning */
#define AX_REG_BBOFFSCAP		0x189	/* Baseband Offset Compensation Capacitors */

/**
 * Packet format
 */
#define AX_REG_PKTADDRCFG		0x200	/* Packet Address Config */
#define AX_REG_PKTLENCFG		0x201	/* Packet Length Configuration */
#define AX_REG_PKTLENOFFSET		0x202	/* Packet Length Offset */
#define AX_REG_PKTMAXLEN		0x203	/* Packet Maximum Length */
#define AX_REG_PKTADDR			0x204	/* Packet Address */
#define AX_REG_PKTADDRMASK		0x208	/* Packet Address Mask */

/**
 * Pattern match
 */
#define AX_REG_MATCH0PAT		0x210	/* Pattern Match Unit 0, Pattern */
#define AX_REG_MATCH0LEN		0x214	/* Pattern Match Unit 0, Pattern Length */
#define AX_REG_MATCH0MIN		0x215	/* Pattern Match Unit 0, Minimum Match */
#define AX_REG_MATCH0MAX		0x216	/* Pattern Match Unit 0, Maximum Match */
#define AX_REG_MATCH1PAT		0x218	/* Pattern Match Unit 1, Pattern */
#define AX_REG_MATCH1LEN		0x21C	/* Pattern Match Unit 1, Pattern Length */
#define AX_REG_MATCH1MIN		0x21D	/* Pattern Match Unit 1, Minimum Match */
#define AX_REG_MATCH1MAX		0x21E	/* Pattern Match Unit 1, Maximum Match */

/**
 * Packet controller
 */
#define AX_REG_TMGTXBOOST		0x220	/* Transmit PLL Boost Time */
#define AX_REG_TMGTXSETTLE		0x221	/* Transmit PLL (post Boost) Settling Time */
#define AX_REG_TMGRXBOOST		0x223	/* Receive PLL Boost Time */
#define AX_REG_TMGRXSETTLE		0x224	/* Receive PLL (post Boost) Settling Time */
#define AX_REG_TMGRXOFFSACQ		0x225	/* Receive Baseband DC Offset Acquisition Time */
#define AX_REG_TMGRXCOARSEAGC	0x226	/* Receive Coarse AGC Time */
#define AX_REG_TMGRXAGC			0x227	/* Receiver AGC Settling Time */
#define AX_REG_TMGRXRSSI		0x228	/* Receiver RSSI Settling Time */
#define AX_REG_TMGRXPREAMBLE1	0x229	/* Receiver Preamble 1 Timeout */
#define AX_REG_TMGRXPREAMBLE2	0x22A	/* Receiver Preamble 2 Timeout */
#define AX_REG_TMGRXPREAMBLE3	0x22B	/* Receiver Preamble 3 Timeout */
#define AX_REG_RSSIREFERENCE	0x22C	/* RSSI Offset */
#define AX_REG_RSSIABSTHR		0x22D	/* RSSI Absolute Threshold */
#define AX_REG_BGNDRSSIGAIN		0x22E	/* Background RSSI Averaging Time Constant */
#define AX_REG_BGNDRSSITHR		0x22F	/* Background RSSI Relative Threshold */
#define AX_REG_PKTCHUNKSIZE		0x230	/* Packet Chunk Size */
#define AX_REG_PKTMISCFLAGS		0x231	/* Packet Controller Miscellaneous Flags */
#define AX_REG_PKTSTOREFLAGS	0x232	/* Packet Controller Store Flags */
#define AX_REG_PKTACCEPTFLAGS	0x233	/* Packet Controller Accept Flags */

/**
 * GPADC
 */
#define AX_REG_GPADCCTRL		0x300	/* General Purpose ADC Control */
#define AX_REG_GPADCPERIOD		0x301	/* GPADC Sampling Period */
#define AX_REG_GPADC13VALUE		0x308	/* GPADC13 Value */

/**
 * LPOSC Calibration
 */
#define AX_REG_LPOSCCONFIG		0x310	/* LPOSC Calibration Configuration */
#define AX_REG_LPOSCSTATUS		0x311	/* LPOSC Calibration Status */
#define AX_REG_LPOSCKFILT		0x312	/* LPOSC Calibration Filter Constant */
#define AX_REG_LPOSCREF			0x314	/* LPOSC Reference Frequency */
#define AX_REG_LPOSCFREQ		0x316	/* LPOSC Frequency Tuning */
#define AX_REG_LPOSCPER			0x318	/* LPOSC Period */

/**
 * DAC
 */
#define AX_REG_DACVALUE			0x330	/* DAC Value */
#define AX_REG_DACCONFIG		0x332	/* DAC Configuration */

/**
 * Performance Tuning
 */
#define AX_REG_POWCTRL			0xF08	/* Power Control */
#define AX_REG_REF				0xF0D	/* Reference */
#define AX_REG_XTALOSC			0xF10	/* Crystal Oscillator Control */
#define AX_REG_XTALAMPL			0xF11	/* Crystal Oscillator Amplitude Control */
