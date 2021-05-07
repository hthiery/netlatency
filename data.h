/*
 * Copyright (c) 2018, Kontron Europe GmbH
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
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __DATA_H__
#define __DATA_H__

#include <net/ethernet.h>
#include <netinet/ether.h>

#define TP_ETHER_TYPE 0x0808

enum {
	TS_T0 = 0,
	TS_WAKEUP,
	TS_PROG_SEND,
	TS_LAST_KERNEL_SCHED,
	TS_LAST_KERNEL_SW_TX,
	TS_LAST_KERNEL_HW_TX,

	/* this must be the last one */
	TS_MAX_NUM,
	TS_MAX_SHORT = 1,
};

struct ether_testpacket {
	struct ether_header hdr;
	guint8 version;
	guint8 stream_id;
	guint32 seq;
	guint16 interval_usec;
	guint16 offset_usec;
	guint32 flags;
	struct timespec timestamps[TS_MAX_NUM];
} __attribute__((__packed__));

struct result {
    struct ether_testpacket *tp;
    struct ether_testpacket *last_tp;

    struct timespec *rx_tss;
    struct timespec *last_rx_tss;

    gint dropped;
    gboolean seq_error;
};

enum {
    TS_KERNEL_HW_RX,
    TS_KERNEL_SW_RX,
    TS_PROG_RECV,

    MAX_TS_RX
};

#define TP_HDR_LEN offsetof(struct ether_testpacket, timestamps)
#define TP_LEN(x) (TP_HDR_LEN + sizeof(struct timespec) * (x))

#define TP_FLAG_END_OF_STREAM  (1 << 0)
#define TP_FLAG_SMALL_MODE     (1 << 1)

#endif /* #ifndef __DATA_H__ */
