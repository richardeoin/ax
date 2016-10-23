/*
 * Example mode implementations for ax5243
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

#include "ax/ax.h"
#include "ax/ax_reg_values.h"

/**
 * Each struct represents a useful mode
 */

/* FSK test */
struct ax_modulation fsk1_modulation = {
  .modulation = AX_MODULATION_FSK,
  .encoding = AX_ENC_NRZI,
  .framing = AX_FRAMING_MODE_HDLC | AX_FRAMING_CRCMODE_CCITT,
  .bitrate = 2000,
  .fec = 0,
  .power = 0.1,
  .parameters = { .fsk = { .modulation_index = 2.0/3 }},
  .continuous = 1,
};


/* PSK test - TODO FSKDEV register for PSK mode??? */
struct ax_modulation psk1_modulation = {
  .modulation = AX_MODULATION_PSK,
  .encoding = AX_ENC_NRZI,
  .framing = AX_FRAMING_MODE_HDLC | AX_FRAMING_CRCMODE_CCITT,
  .bitrate = 2000,
  .fec = 0,
  .power = 0.1,
  .parameters = { .fsk = { .modulation_index = 2.0/3 }},
};


/* APRS */
struct ax_modulation aprs_modulation = {
  .modulation = AX_MODULATION_AFSK,
  .encoding = AX_ENC_NRZI,
  .framing = AX_FRAMING_MODE_HDLC, /* also crc?? */
  .bitrate = 1200,
  .fec = 0,
  .power = 1.0,
  .parameters = { .afsk = {
      .space = 1200, .mark = 2200, .deviation = 3000 }},
};
