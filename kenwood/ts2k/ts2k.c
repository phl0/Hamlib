/*
 *  Hamlib Kenwood backend - TS2K description
 *  Copyright (c) 2000-2002 by Stephane Fillod
 *  Copyright (c) 2002-2003 by Dale E. Edmons
 *
 *		$Id: ts2k.c,v 1.1.2.7 2003-03-03 14:27:18 dedmons Exp $
 *
 *   This library is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of
 *   the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <hamlib/rig.h>
#include "ts2k/ts2k_menu.h"
#include "ts2k/ts2k.h"
#include "ts2k/ts2k_cmds.h"

ts2k_pm_t ts2k_pm[TS2K_PMSIZ];

ts2k_menu_t ts2k_menu[2*TS2K_PMSIZ];	// One set for each PM

const struct ts2k_priv_caps  ts2k_priv_caps  = {
	cmdtrm: EOM_KEN,
};

/*
 * ts2k rig capabilities.
 *
 * TODO: antenna caps
 *
 * part of infos comes from http://www.kenwood.net/
 */
const struct rig_caps ts2k_caps = {
	rig_model: RIG_MODEL_TS2K,
	model_name:"TS-2K",
	mfg_name: "Kenwood",
	version: "0.1.2",
	copyright: "LGPL",
	status: RIG_STATUS_ALPHA,
	rig_type: RIG_TYPE_TRANSCEIVER,
	ptt_type: RIG_PTT_RIG,
	dcd_type: RIG_DCD_RIG,
	port_type: RIG_PORT_SERIAL,
	serial_rate_min: 1200,
	serial_rate_max: 57600,
	serial_data_bits: 8,
	serial_stop_bits: 1,
	serial_parity: RIG_PARITY_NONE,
	serial_handshake: RIG_HANDSHAKE_NONE,
	write_delay: 0,
// set 1,20,4 and cured timeouts for "if...;" on ts2k_get_freq()	--Dale
	post_write_delay: 1,
	timeout: 30,
	retry: 4,
	has_get_func: TS2K_FUNC_ALL,
	has_set_func: TS2K_FUNC_ALL,
	has_get_level: TS2K_LEVEL_ALL,
	has_set_level: RIG_LEVEL_SET(TS2K_LEVEL_ALL),
	has_get_parm: TS2K_PARM_OP,
	has_set_parm: TS2K_PARM_OP,
	level_gran: {},                 /* FIXME: granularity */
	parm_gran: {},
	vfo_ops: TS2K_VFO_OP,
	scan_ops: TS2K_SCAN_OP,
	vfo_all: RIG_VFO_VALID,
	ctcss_list: ts2k_ctcss_list,
	dcs_list: ts2k_dcs_list,
	preamp:  { 20, RIG_DBLST_END, },	/* FIXME: real preamp? */
	attenuator:  { 20, RIG_DBLST_END, },
	max_rit: kHz(20),
	max_xit: kHz(20),
	max_ifshift: kHz(1),
	targetable_vfo: RIG_TARGETABLE_FREQ,
	transceive: RIG_TRN_RIG,
	bank_qty:  0,
	chan_desc_sz: 8,
	chan_list: {
		{ 0, 289, RIG_MTYPE_MEM, 0 },		/* regular memories */
		{ 290, 299, RIG_MTYPE_EDGE, 0 },	/* band tune limits (not scan-only) */
		{ 0, 9, RIG_MTYPE_MEMOPAD, 0 },		/* Quick Memories, Main+sub both saved:) */
		{ 0, 1, RIG_MTYPE_CALL, 0 },		/* each TX band has one call */
		{ 0, 9, RIG_MTYPE_SAT, 0 },		/* direct operation from these */
		RIG_CHAN_END,
	},
	rx_range_list1: {
		{kHz(300),MHz(60),TS2K_ALL_MODES,-1,-1,TS2K_MAINVFO},
		{MHz(144),MHz(146),TS2K_ALL_MODES,-1,-1,TS2K_MAINVFO},
		{MHz(430),MHz(440),TS2K_ALL_MODES,-1,-1,TS2K_MAINVFO},
		{MHz(144),MHz(146),TS2K_ALL_MODES,-1,-1,TS2K_SUBVFO},
		{MHz(430),MHz(440),TS2K_ALL_MODES,-1,-1,TS2K_SUBVFO},
		RIG_FRNG_END,
	}, /* rx range */
	tx_range_list1: {
		{kHz(1830),kHz(1850),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{kHz(1830),kHz(1850),TS2K_AM_TX_MODES,2000,25000,TS2K_MAINVFO},
		{kHz(3500),kHz(3800),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{kHz(3500),kHz(3800),TS2K_AM_TX_MODES,W(5),W(25),TS2K_MAINVFO},
		{MHz(7),kHz(7100),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{MHz(7),kHz(7100),TS2K_AM_TX_MODES,W(5),W(25),TS2K_MAINVFO},
		{MHz(10.1),MHz(10.15),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{MHz(10.1),MHz(10.15),TS2K_AM_TX_MODES,W(5),W(25),TS2K_MAINVFO},
		{MHz(14),kHz(14350),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{MHz(14),kHz(14350),TS2K_AM_TX_MODES,W(5),W(25),TS2K_MAINVFO},
		{kHz(18068),kHz(18168),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{kHz(18068),kHz(18168),TS2K_AM_TX_MODES,W(5),W(25),TS2K_MAINVFO},
		{MHz(21),kHz(21450),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{MHz(21),kHz(21450),TS2K_AM_TX_MODES,W(5),W(25),TS2K_MAINVFO},
		{kHz(24890),kHz(24990),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{kHz(24890),kHz(24990),TS2K_AM_TX_MODES,W(5),W(25),TS2K_MAINVFO},
		{MHz(28),kHz(29700),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{MHz(28),kHz(29700),TS2K_AM_TX_MODES,W(5),W(25),TS2K_MAINVFO},
		{MHz(50),MHz(50.2),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{MHz(50),MHz(50.2),TS2K_AM_TX_MODES,W(5),W(25),TS2K_MAINVFO},
		{MHz(144),MHz(146),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{MHz(144),MHz(146),TS2K_AM_TX_MODES,W(5),W(25),TS2K_MAINVFO},
		{MHz(430),MHz(440),TS2K_OTHER_TX_MODES,W(5),W(50),TS2K_MAINVFO},
		{MHz(430),MHz(440),TS2K_AM_TX_MODES,W(5),W(12.5),TS2K_MAINVFO},
		RIG_FRNG_END,
	}, /* tx range */
	rx_range_list2: {
		{kHz(300),MHz(60),TS2K_ALL_MODES,-1,-1,TS2K_MAINVFO},
		{MHz(142),MHz(152),TS2K_ALL_MODES,-1,-1,TS2K_MAINVFO},
		{MHz(420),MHz(450),TS2K_ALL_MODES,-1,-1,TS2K_MAINVFO},
		{MHz(118),MHz(174),TS2K_ALL_MODES,-1,-1,TS2K_SUBVFO},
		{MHz(220),MHz(512),TS2K_ALL_MODES,-1,-1,TS2K_SUBVFO},
		RIG_FRNG_END,
	}, /* rx range */
	tx_range_list2: {
		{kHz(1800),MHz(2),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{kHz(1800),MHz(2),TS2K_AM_TX_MODES,2000,25000,TS2K_MAINVFO},
		{kHz(3500),MHz(4),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{kHz(3500),MHz(4),TS2K_AM_TX_MODES,W(5),W(25),TS2K_MAINVFO},
		{MHz(7),kHz(7300),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{MHz(7),kHz(7300),TS2K_AM_TX_MODES,W(5),W(25),TS2K_MAINVFO},
		{MHz(10.1),MHz(10.15),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{MHz(10.1),MHz(10.15),TS2K_AM_TX_MODES,W(5),W(25),TS2K_MAINVFO},
		{MHz(14),kHz(14350),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{MHz(14),kHz(14350),TS2K_AM_TX_MODES,W(5),W(25),TS2K_MAINVFO},
		{kHz(18068),kHz(18168),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{kHz(18068),kHz(18168),TS2K_AM_TX_MODES,W(5),W(25),TS2K_MAINVFO},
		{MHz(21),kHz(21450),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{MHz(21),kHz(21450),TS2K_AM_TX_MODES,W(5),W(25),TS2K_MAINVFO},
		{kHz(24890),kHz(24990),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{kHz(24890),kHz(24990),TS2K_AM_TX_MODES,W(5),W(25),TS2K_MAINVFO},
		{MHz(28),kHz(29700),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{MHz(28),kHz(29700),TS2K_AM_TX_MODES,W(5),W(25),TS2K_MAINVFO},
		{MHz(50),MHz(54),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{MHz(50),MHz(54),TS2K_AM_TX_MODES,W(5),W(25),TS2K_MAINVFO},
		{MHz(144),MHz(148),TS2K_OTHER_TX_MODES,W(5),W(100),TS2K_MAINVFO},
		{MHz(144),MHz(148),TS2K_AM_TX_MODES,W(5),W(25),TS2K_MAINVFO},
		{MHz(430),MHz(450),TS2K_OTHER_TX_MODES,W(5),W(50),TS2K_MAINVFO},
		{MHz(430),MHz(450),TS2K_AM_TX_MODES,W(5),W(12.5),TS2K_MAINVFO},
		RIG_FRNG_END,
	}, /* tx range */
	tuning_steps: {
		{TS2K_ALL_MODES,50},
		{TS2K_ALL_MODES,100},
		{TS2K_ALL_MODES,kHz(1)},
		{TS2K_ALL_MODES,kHz(5)},
		{TS2K_ALL_MODES,kHz(9)},
		{TS2K_ALL_MODES,kHz(10)},
		{TS2K_ALL_MODES,12500},
		{TS2K_ALL_MODES,kHz(20)},
		{TS2K_ALL_MODES,kHz(25)},
		{TS2K_ALL_MODES,kHz(100)},
		{TS2K_ALL_MODES,MHz(1)},
		{TS2K_ALL_MODES,0},	/* any tuning step */
		RIG_TS_END,
	},
	/* mode/filter list, remember: order matters! */
	filters: {
		{RIG_MODE_SSB, kHz(2.2)},
		{RIG_MODE_CW, Hz(600)},
		{RIG_MODE_RTTY, Hz(1500)},
		{RIG_MODE_AM, kHz(6)},
		{RIG_MODE_FM|RIG_MODE_AM, kHz(12)},
		RIG_FLT_END,
	},
	priv: (void *)&ts2k_priv_caps,
	rig_init: ts2k_init,
/*
 * stuff I've written--kd7eni
 */

/* ts2k */
// Temporary!  We might be able to use priv.  Until I sort that out, I just
// added a new stucture.  Locally nothing should be affected.

//	pm: (ts2k_pm_t *) &ts2k_pm,
	pm: NULL,

	set_freq: ts2k_set_freq,	// 
	get_freq: ts2k_get_freq,	// "if;", "oi;"
	get_mem: ts2k_get_mem,		// "qr;"=memo
	set_mem: ts2k_set_mem,
	get_mode: ts2k_get_mode,	// "md;"
	set_mode: ts2k_set_mode,
	get_ptt: ts2k_get_ptt,		// "tx;"
	set_ptt: ts2k_set_ptt,		// "rx;"
	set_vfo: ts2k_set_vfo,		// "ts;"=REV, 
	get_vfo: ts2k_get_vfo,
	rig_open: ts2k_open,		// calls ts2k_pm_init in ts2k_menu.c
	rig_close: ts2k_pm_close,	// ts2k_menu.c

/* Untested */
	get_tone:	ts2k_get_tone,
	get_ctcss:	ts2k_get_ctcss,
	get_dcs:	ts2k_get_dcs,
	get_rit:	ts2k_get_rit,		// "rc;", "rd;", "rt;", "ru;"
	set_rit:	ts2k_set_rit,
	get_xit:	ts2k_get_xit,		// "xt;"
	set_xit:	ts2k_set_xit,
	get_rptr_offs:	ts2k_get_rptr_offs,	// "os;"
	get_rptr_shift:	ts2k_get_rptr_shift,
	set_rptr_offs:	ts2k_set_rptr_offs,
	set_rptr_shift:	ts2k_set_rptr_shift,
	get_tone_sql:	ts2k_get_tone_sql,	// "to;"
	get_ctcss_sql:	ts2k_get_ctcss_sql,	// "ct;" 
	get_dcs_sql:	ts2k_get_dcs_sql,	// "dq;"
	set_tone_sql:	ts2k_set_tone_sql,
	set_ctcss_sql:	ts2k_set_ctcss_sql,
	set_dcs_sql:	ts2k_set_dcs_sql,
	set_tone:	ts2k_set_tone,		// "tn;"
	set_ctcss:	ts2k_set_ctcss,		// "cn;"
	set_dcs:	ts2k_set_dcs,		// "qc;"
	reset:		ts2k_reset,		// "sr;"
	get_split:	ts2k_get_split,
	set_split:	ts2k_set_split,
	get_split_freq:	ts2k_get_split_freq,
	set_split_freq:	ts2k_set_split_freq,
	get_split_mode:	ts2k_get_split_mode,
	set_split_mode:	ts2k_set_split_mode,
	get_channel:	ts2k_get_channel,	// "qr;", ...
	set_channel:	ts2k_set_channel,
	scan:		ts2k_scan,		// "sc;"

/* comming soon... */	/* highest */
//	get_info:	ts2k_get_info,		// "ty;"=firmware, "id;"=019=ts2k
//	get_ts:		ts2k_get_ts,
//	vfo_op:		ts2k_vfo_op,
//	set_ts:		ts2k_set_ts,
//	get_conf:	ts2k_get_conf,
//	set_conf:	ts2k_set_conf,
//	get_ant:	ts2k_get_ant,		// "ac;"=tune, "an;"=ant
//	set_ant:	ts2k_set_ant,
//	send_morse:	ts2k_send_morse,	// "ks;"=speed, "ky;"=code
//	send_dtmf:	ts2k_send_dtmf,		// "td;"
//	recv_dtmf:	ts2k_recv_dtmf,		/* possible? */
//	get_trn:	ts2k_get_trn,
//	set_trn:	ts2k_set_trn,
//	get_level:	ts2k_get_level,
//	set_level:	ts2k_set_level,
//	get_func:	ts2k_get_func,
//	set_func:	ts2k_set_func,
//	get_parm:	ts2k_get_parm,
//	set_parm:	ts2k_set_parm,
//	get_powerstat:	ts2k_get_powerstat,	// "ps;"
//	set_powerstat:	ts2k_set_powerstat,
//	get_dcd:	ts2k_get_dcd,
//	decode_event:	ts2k_decode_event,
	/* lowest */

/* New function I want to create */
//	set_menu:	ts2k_set_menu,		// set menu parameter
//	get_menu:	ts2k_get_menu,		// get menu parameter
//	list_menu:	ts2k_list_menu,		// string text of menu_t

//	In src/rig.c:
//		rig_to_rig(RIG* dest, channel_t * dstlst, RIG* src, channel_t src_lst);

/* and never... */
//	set_bank:		/* I abuse this for other purposes */
/*
 * end ts2k_caps
 */
};

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include <hamlib/rig.h>
#include "ts2k.h"

#include <stdlib.h>
#include <stdio.h>		/* Standard input/output definitions */
#include <string.h>		/* String function definitions */
#include <unistd.h>		/* UNIX standard function definitions */
#include <fcntl.h>		/* File control definitions */
#include <errno.h>		/* Error number definitions */
#include <termios.h>		/* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <math.h>

#include <serial.h>
#include <misc.h>

/* Moved all const stuff to ts2k.h where it wants to be. */
#undef _ENABLE_PTT_CTRL

/**
 * kenwood_transaction
 * Assumes rig!=NULL rig->state!=NULL rig->caps!=NULL
 *
 * cmdstr - Command to be sent to the rig. Cmdstr can also be NULL, indicating
 *		that only a reply is needed (nothing will be send).
 * data - Buffer for reply string.  Can be NULL, indicating that no reply is
 *	   is needed and will return with RIG_OK after command was sent.
 * datasize - in: Size of buffer. It is the caller's responsibily to provide
 *		  a large enough buffer for all possible replies for a command.
 *		  out: location where to store number of bytes read.
 *
 * returns:
 *   RIG_OK  -  if no error occured.
 *   RIG_EIO  -  if an I/O error occured while sending/receiving data.
 *   RIG_ETIMEOUT  -  if timeout expires without any characters received.
 *   RIG_REJECTED  -  if a negative acknowledge was received or command not
 *				recognized by rig.
 */

/* FIXME: cmd_len appears to change and needs set every invocation? --Dale */

int
ts2k_transaction(RIG * rig, const char *cmdstr, int cmd_len,
		 char *data, size_t * datasize)
{
//       return kenwood_transaction(rig, cmdstr, cmd_len, data, datasize);

	struct rig_state *rs;
	int retval;
	const char *cmdtrm = EOM_KEN;	/* Default Command/Reply termination char */
	int retry_read = 2;
//       char *errtxt;

#define MAX_RETRY_READ 8

	rs = &rig->state;
	rs->hold_decode = 1;

	serial_flush(&rs->rigport);

	cmdtrm = cmd_trm(rig);

	if (cmdstr != NULL) {
		//       rig_debug(RIG_DEBUG_ERR, __FUNCTION__": 1) sending '%s'\n\n", cmdstr);
		retval = write_block(&rs->rigport, cmdstr, strlen(cmdstr));
		if (retval != RIG_OK) {
			ta_quit;
		}
#undef	TH_ADD_CMDTRM
#ifdef	TH_ADD_CMDTRM
		//       rig_debug(RIG_DEBUG_ERR, __FUNCTION__": 2) sending '%s'\n\n", cmdtrm);
		retval = write_block(&rs->rigport, cmdtrm, strlen(cmdtrm));
		if (retval != RIG_OK) {
			ta_quit;
		}
#endif
	}

/* FIXME: Everything below this line wants to be completely rewritten!!!! */

	if (data == NULL || datasize <= 0) {
		rig->state.hold_decode = 0;
		return RIG_OK;	/* don't want a reply */
	}

      transaction_read:
	/* FIXME : TS-2k gets alot of 'timedout' on read_string()! */
	//rig_debug(RIG_DEBUG_ERR, __FUNCTION__": 3a) reading %u bytes...\n", *datasize);
	retval =
	    read_string(&rs->rigport, data, *datasize, cmdtrm,
			strlen(cmdtrm));
	//rig_debug(RIG_DEBUG_ERR, __FUNCTION__": 3b) read '%s', retval=%u\n\n", data, retval);
	*datasize = retval;
	if (retval > 0) {
		//rig_debug(RIG_DEBUG_ERR, __FUNCTION__": 3b) read cmd '%s', retval=%u\n\n", data, retval);
		retval = RIG_OK;
		goto transaction_check;
	}

	/* Check that command termination is correct */
	if (!strchr(cmdtrm, data[strlen(data)])) {
		if (retry_read++ < MAX_RETRY_READ)
			goto transaction_read;
		rig_debug(RIG_DEBUG_ERR,
			  __FUNCTION__
			  ": Command is not correctly terminated '%s'\n",
			  data);
		retval = -RIG_EPROTO;
		ta_quit;
	}

	/* Errors */
	if (strlen(data) == 2 && data[0] == 'E') {
		switch (data[0]) {
		case 'E':
			rig_debug(RIG_DEBUG_ERR,
				  __FUNCTION__
				  ": Communication Error for '%s'\n",
				  cmdstr);
			break;

		case 'O':
			rig_debug(RIG_DEBUG_ERR,
				  __FUNCTION__
				  ": Communication Error for '%s'\n",
				  cmdstr);
			break;

		case '?':
			rig_debug(RIG_DEBUG_ERR,
				  __FUNCTION__
				  ": Communication Error for '%s'\n",
				  cmdstr);
			break;

		default:
			rig_debug(RIG_DEBUG_ERR,
				  __FUNCTION__ ": Hamlib Error for '%s'\n",
				  cmdstr);
			break;

		}
		retval = -RIG_ERJCTED;
		ta_quit;
	}
#undef	CONFIG_STRIP_CMDTRM
#ifdef	CONFIG_STRIP_CMDTRM
	if (strlen(data) > 0)
		data[strlen(data) - 1] = '\0';	/* not very elegant, but should work. */
	else
		data[0] = '\0';
#endif
	/*
	 * Check that received the correct reply. The first two characters
	 * should be the same as command.
	 */
      transaction_check:
	if (cmdstr && (toupper(data[0]) != toupper(cmdstr[0])
		       || toupper(data[1]) != toupper(cmdstr[1]))) {
		/*
		 * TODO: When RIG_TRN is enabled, we can pass the string
		 *        to the decoder for callback. That way we don't ignore
		 *        any commands.
		 */
		if (retry_read++ < MAX_RETRY_READ)
			goto transaction_read;

		rig_debug(RIG_DEBUG_ERR,
			  __FUNCTION__ ": Unexpected reply '%s'\n", data);
		retval = -RIG_EPROTO;
		{
			ta_quit;
		}
	}

	retval = RIG_OK;

//       transaction_quit:
	{
		ta_quit;
	}
}

/* kenwood_init
 *
 * Basically, it sets up *priv
 * REM: serial port is already open (rig->state.rigport.fd)
 */
int ts2k_init(RIG * rig)
{
	const struct rig_caps *caps;
	const struct ts2k_priv_caps *priv_caps;
	rig_debug(RIG_DEBUG_WARN, __FUNCTION__ ": called\n");
	if (!rig || !rig->caps)
		return -RIG_EINVAL;
	caps = rig->caps;
	if (!caps->priv)
		return -RIG_ECONF;
	priv_caps = (const struct ts2k_priv_caps *) caps->priv;

#if 0
	/* this section won't compile --Dale */
	priv = (struct ts2k_priv_data *)
	    malloc(sizeof(struct ts2k_priv_data));
	if (!priv) {
		/* whoops! memory shortage! */
		return -RIG_ENOMEM;
	}

	rig->state.priv = (void *) priv;
	/* Assign default values */
	priv->dummy = -1;	// placeholder for real entries.
#endif

	return RIG_OK;
}

/* kenwood_cleanup
 * the serial port is closed by the frontend
 */
int ts2k_cleanup(RIG * rig)
{
	rig_debug(RIG_DEBUG_TRACE, __FUNCTION__ ": called\n");
	if (!rig)
		return -RIG_EINVAL;
	if (rig->state.priv)
		free(rig->state.priv);
	rig->state.priv = NULL;
	return RIG_OK;
}

/*
 * ts2k_open()	Called just before rig.c does first rig_get_vfo().
 *	We must be able to talk to the rig so ts2k_pm_init() must
 *	be done here.
 *
 *	TODO:	Call a startup script to set things up the user's way.
 */
int ts2k_open(RIG *rig)
{
	return ts2k_pm_init(rig);
}

/***********************************************************
 * I've kept the above functions and deleted and rewritten
 *	everything from here down.  The new stuff will all
 *	use *only* the ts2k_g_??() and ts2k_s_??() routines
 *	for I/O.  They in turn call ts2k_transaction().  I
 *	kept only the function prototype.
 ***********************************************************/

/*****************************************************************************
	Added the following functions.
 	(C) Copyright 2002 by Dale E. Edmons.   All rights Reserved.
	License:	Identical to all other Hamlib code.
*****************************************************************************/

/*
 * status:	Working
 */
int ts2k_set_vfo(RIG * rig, vfo_t vfo)
{
	return ts2k_uniq_SetSimpleVfo(rig, vfo);
}

/*
 * status:	Simple Vfo ok, Complex not implemented
 */
int ts2k_get_vfo(RIG * rig, vfo_t * vfo)
{
	//int retval;

	/* Complex VFOs not yet implemented */
	// retval = ts2k_uniq_GetComplexVfo(rig, vfo);
	// CHKERR(retval);

	return ts2k_uniq_GetSimpleVfo(rig, vfo);
}

/*
 * Set Rx frequency.  Tx frequency unchanged.
 *
 * Note: The PLL is the device which implements the
 *	hardware VFO.  The rig saves and restores
 *	the value in the PLL when new modes are
 *	selected (e.g. Satelite).  We must place
 *	the rig in the same mode to ensure the rig's
 *	view of the frequency gets changed and not
 *	just the PLL.  For example:
 *		I wish to operate satelite and change
 *		the dnlink to freq = F1 using the
 *		"fb;" command.  I then turn on SAT
 *		mode.  The dnlink freq is *not* F1.
 *		The rig took the last SAT values out
 *		of memory an wrote them to the PLLs,
 *		and overwrote F1 with freq = F2.
 *		Thus, *just* writing to the correct
 *		PLL is not enough to ensure that we
 *		have stored F1 as the dnlink freq!
 *		However, I deselect SAT and the PLL
 *		now has freq F1 instead of F2.
 *	As a consequence, I do rig_set_vfo() before
 *	all operations that are not RIG_VFO_CURR and
 *	that apply to a single VFO rather than the
 *	entire rig.
 *
 *	Status:	working
 */
int ts2k_set_freq(RIG * rig, vfo_t vfo, freq_t freq)
{
	int retval, skip;
	vfo_t vorig, vcurr;
	TS2K_FA_T param;

	skip = (vfo & RIG_VFO_CURR);

	// Must be outside skip.  We need *real* VFO.
	retval = rig_get_vfo(rig, &vorig);
	CHKERR(retval);

	// Set correct VFO
	if( !skip ) {
		retval = rig_set_vfo(rig, vfo);
		CHKERR(retval);
		vcurr = vfo;
	} else {
		vcurr = vorig;	// I don't like changing caller's value
	}

	param.p1 = freq;	// Good starting place

	// Simplest VFOs
	switch(vcurr) {

	case RIG_SAT_UPLINK:
	case RIG_VFO_AB:
	case RIG_VFO_A:
		retval = ts2k_s_fa(rig, &param);
		break;

	case RIG_SAT_DNLINK:
	case RIG_VFO_BA:
	case RIG_VFO_B:
		retval = ts2k_s_fb(rig, &param);
		break;

	case RIG_VFO_C:
		retval = ts2k_s_fc(rig, &param);
		break;

	case RIG_CALL_C:
	case RIG_CALL_A:	// ugly hack!
		retval = ts2k_emul_SetCallFreq(rig, vcurr, freq);
		break;

	case RIG_MEM_A:
	case RIG_MEM_C:
	default:
		rig_debug(RIG_DEBUG_ERR, __FUNCTION__ 
			": Target not currently recognized, vfo = %s\n",
			strvfo(vcurr));
		return -RIG_ENIMPL;	// or, not available
		break;
	}

	if( !skip ) {
		retval = rig_set_vfo(rig, vorig);
		CHKERR(retval);
	}

	return RIG_OK;
}

/*
 *	status:	working
 */
int ts2k_get_freq(RIG *rig, vfo_t vfo, freq_t *freq)
{
	int retval, skip;
	vfo_t vorig;
	TS2K_IF_T param;

	skip = (vfo & RIG_VFO_CURR);

	// Must be outside skip.  We need *real* VFO.
	retval = rig_get_vfo(rig, &vorig);
	CHKERR(retval);

	// Set correct VFO
	if( !skip ) {
		retval = rig_set_vfo(rig, vfo);
		CHKERR(retval);
	}

	// Simplest VFOs
	switch(vfo) {
	case RIG_SAT_DNLINK:
		retval = ts2k_g_oi(rig, &param);
		break;

	default:
		retval = ts2k_g_if(rig, &param);
		break;
	}

	*freq = param.p1;

	if( !skip ) {
		retval = rig_set_vfo(rig, vorig);
		CHKERR(retval);
	}

	return RIG_OK;
}

/*
 * Preserves original vfo.  Width is only available for the
 *	entire transceiver, and not per VFO.  Use rig_passband_*().
 *
 *	status:	working
 */
int ts2k_set_mode(RIG * rig, vfo_t vfo, rmode_t mode, pbwidth_t width)
{
	int retval, skip;
	vfo_t vorig;
	TS2K_MD_T param;

	/*
 	 * Width is a transceiver parameter set by rig_passband_*()
	 *	functions and available for main only.  Rig applies
	 *	it to each identical mode (Main only) for all VFOs.
	 *	(My rig is E-type.  Manual says K-type can use narrow
	 *	on Sub.)
	 */

	skip = (vfo & RIG_VFO_CURR);

	// Set correct VFO
	if( !skip ) {
		retval = rig_get_vfo(rig, &vorig);
		CHKERR(retval);
		retval = rig_set_vfo(rig, vfo);
		CHKERR(retval);
	}

	if(width != RIG_PASSBAND_NORMAL) {
		rig_debug(RIG_DEBUG_WARN, __FUNCTION__
			": Passband not available.  "
			"Use rig_passsband_*()\n");
	}

	// Set Modes
	switch(mode) {

	case RIG_MODE_LSB:	param.p1 = 1; break;
	case RIG_MODE_USB:	param.p1 = 2; break;
	case RIG_MODE_CW:	param.p1 = 3; break;
	case RIG_MODE_FM:	param.p1 = 4; break;
	case RIG_MODE_AM:	param.p1 = 5; break;
	case RIG_MODE_RTTY:	param.p1 = 6; break;
	case RIG_MODE_CWR:	param.p1 = 7; break;

	case RIG_MODE_RTTYR:	param.p1 = 9; break;

	default:
		rig_debug(RIG_DEBUG_ERR, __FUNCTION__
			": Unknown mode.\n");
		return -RIG_ENAVAIL;
		break;
	}

	retval = ts2k_s_md(rig, &param);
	CHKERR(retval);

	if( !skip ) {
		retval = rig_set_vfo(rig, vorig);
		CHKERR(retval);
	}

	return RIG_OK;
}

/*
 *	status:	working
 */
int ts2k_get_mode(RIG * rig, vfo_t vfo, rmode_t * mode, pbwidth_t * width)
{
	STDPARAM;	// define skip, vorig, retval
	union {
		TS2K_MD_T md;
		TS2K_FW_T fw;
	} param;

	TESTVFO(skip);		// semicolon optional

	retval = ts2k_g_md(rig, &param.md);
	CHKERR(retval);

	switch(param.md.p1) {
	case 1:	*mode = RIG_MODE_LSB; break;
	case 2:	*mode = RIG_MODE_USB; break;
	case 3:	*mode = RIG_MODE_CW; break;
	case 4:	*mode = RIG_MODE_FM; break;
	case 5:	*mode = RIG_MODE_AM; break;
	case 6:	*mode = RIG_MODE_RTTY; break;
	case 7:	*mode = RIG_MODE_CWR; break;

	case 9:	*mode = RIG_MODE_RTTYR; break;

	default:
		*mode = RIG_MODE_NONE;
		rig_debug(RIG_DEBUG_ERR, __FUNCTION__
			": Unknown mode = %i\n", param.md.p1);
		return -RIG_ENAVAIL;
		break;
	}

	*width = RIG_PASSBAND_NORMAL;	// FIXME:

	RESETVFO(skip);		// semicolon optional

	return RIG_OK;
}

/*
 *	status:	working
 */
int ts2k_set_ptt(RIG * rig, vfo_t vfo, ptt_t ptt)
{
	int retval, skip;
	vfo_t vorig;

	skip = (vfo & RIG_VFO_CURR);

	if( !skip ) {
		/*
		 * Do NOT Restore original VFO with PTT enabled!
		 */
		retval = rig_get_vfo(rig, &vorig);
		CHKERR(retval);
		retval = rig_set_vfo(rig, vfo);
		CHKERR(retval);
	}

	switch(ptt) {
	case RIG_PTT_ON:
		retval = ts2k_s_tx(rig, NULL);	// Expect errors.  Rig Bug.
		skip |= 1;
		/* no CHKERR(retval) */
		break;

	case RIG_PTT_OFF:
		retval = ts2k_s_rx(rig, NULL);	// Expect errors.  Rig Bug.
		skip |= 0;
		/* no CHKERR(retval) */
		break;

	default:
		rig_debug(RIG_DEBUG_ERR, __FUNCTION__
			": Unknown ptt value = %i\n", ptt);
		return -RIG_ENAVAIL;
		break;
	}

	/*
	 * Do No Restore VFO with PTT enabled!
	 */
	if( !skip ) {
		retval = rig_set_vfo(rig, vorig);
		CHKERR(retval);
	}

	return RIG_OK;
}

/*
 * Manual incorrectly states "tx;" will query.  We
 *	must use "if;" instead.  "tx;" transmits!!!
 *	(Or causes an error if already set.  Geez!)
 *
 *	Status:	working!
 */
int ts2k_get_ptt(RIG * rig, vfo_t vfo, ptt_t * ptt)
{
	STDPARAM;
	TS2K_IF_T param;

	TESTVFO(skip);

	retval = ts2k_g_if(rig, &param);
	CHKERR(retval);

	switch(param.p8) {

	case 0:	*ptt = RIG_PTT_OFF; break;
	case 1:	*ptt = RIG_PTT_ON; break;

	default:
		return -RIG_EINTERNAL;	// or rig
	}

	RESETVFO(skip);

	return RIG_OK;
}

/*
 */
int ts2k_reset(RIG * rig, reset_t reset)
{
	int i;
	union {
		TS2K_SA_S_T sa;

		TS2K_FR_T fr;
		TS2K_SC_T sc;
		TS2K_DC_T dc;
		TS2K_SB_T sb;
		TS2K_SR_T sr;
		TS2K_QR_T qr;
	} param;

	switch(reset) {
	case RIG_RESET_VFO:
		rig_debug(RIG_DEBUG_ERR, __FUNCTION__
			": Performing VFO Reset.\n\n");
		param.sr.p1 = 1;
		ts2k_s_sr(rig, &param.sr);
		break;

	case RIG_RESET_MASTER:
		rig_debug(RIG_DEBUG_ERR, __FUNCTION__
			": Performing Master Reset.\n\n");
		rig_debug(RIG_DEBUG_ERR, __FUNCTION__
			": Change Menu(56) to proper speed and cycle power.\n");
		param.sr.p1 = 2;
		ts2k_s_sr(rig, &param.sr);
		break;

	case RIG_RESET_MCALL:	// mem clear
		rig_debug(RIG_DEBUG_ERR, __FUNCTION__
			": Not available.  Do Master reset instead.\n");
		return -RIG_ENAVAIL; break;

	case RIG_RESET_SOFT:
		rig_debug(RIG_DEBUG_ERR, __FUNCTION__
			": Performing Soft Reset.\n\n");
		rig_debug(RIG_DEBUG_ERR, __FUNCTION__
			"\tSending single \';\' to rig to ensure reply.\n"
			"\t\tThis won't work if you're in TNC mode.\n"
			"\t\tIf so, set Menu(55) to Off and try again.\n"
			"\t\tMenu(56) is the baud rate.\n");
		ts2k_transaction(rig, ";", 1, NULL, NULL);
		usleep( 100000 );	// 0.2 sec

		rig_debug(RIG_DEBUG_ERR, __FUNCTION__
			"\tTurning ON Subreceiver.\n");
		param.sb.p1 = 1; ts2k_s_sb(rig, &param.sb);
		usleep( 100000 );	// 0.1 sec

		rig_debug(RIG_DEBUG_ERR, __FUNCTION__
			"\tTurning off SAT.\n");
		param.sa.p2 = 0;
		param.sa.p1 = 0; ts2k_s_sa(rig, &param.sa);
		usleep( 100000 );	// 0.1 sec

		rig_debug(RIG_DEBUG_ERR, __FUNCTION__
			"\tTurning off QuickMemory.\n");
		param.qr.p2 = 0;
		param.qr.p1 = 0; ts2k_s_qr(rig, &param.qr);
		usleep( 100000 );	// 0.1 sec

		// Do both Sub and Main
		for(i=0; i<2; i++) {

		param.dc.p2 = i;	// Select Main/Sub
		param.dc.p1 = i; ts2k_s_dc(rig, &param.dc);
		
		rig_debug(RIG_DEBUG_ERR, __FUNCTION__
			"\tTurning off Scan modes.\n");
		param.sc.p1 = 0; ts2k_s_sc(rig, &param.sc);
		
		rig_debug(RIG_DEBUG_ERR, __FUNCTION__
			"\tTurning off Memory/Call/Split modes.\n");
		param.fr.p1 = 0; ts2k_s_fr(rig, &param.fr);
		}

		rig_debug(RIG_DEBUG_ERR, __FUNCTION__
			"\n\tWell, that's enough for me.  Anything else\n"
			"\tand you might as well just do a VFO reset.\n\n");
		break;

	case RIG_RESET_NONE:
		return RIG_OK; break;

	default:
		rig_debug(RIG_DEBUG_WARN, __FUNCTION__
			": Invalid reset type.\n");
		return -RIG_EINVAL; break;
	}

	return RIG_OK;
}

/*
 *	status: working
 */
int ts2k_set_mem(RIG * rig, vfo_t vfo, int ch)
{
	int retval, skip;
	vfo_t vorig;
	TS2K_MC_T param;

	skip = (vfo & RIG_VFO_CURR);

	if( !skip ) {
		retval = rig_get_vfo(rig, &vorig);
		CHKERR(retval);
	}

	switch(vfo) {

	/* Note: Main/Sub memory pointers are always valid.
	 *	Memory mode needn't be enabled! */
	case RIG_MEM_A:
	case RIG_MEM_C:
		retval = rig_set_vfo(rig, vfo);
		/* no break */

	case RIG_MEM_CURR:
	case RIG_VFO_CURR:	// Compatibility with existing code
		param.p1 = ch;
		retval = ts2k_s_mc(rig, &param);
		CHKERR(retval);
		break;

	case RIG_MEMO:	// scratchpad or temp memories
		return -RIG_ENIMPL;
		break;

	case RIG_SAT_MEM:
		return -RIG_ENIMPL;
		break;
	
	default:
		rig_debug(RIG_DEBUG_ERR, __FUNCTION__
			": Unknown mem = %s\n", strvfo(vfo));
		return -RIG_ENAVAIL;
		break;
	}

	if( !skip ) {
		retval = rig_set_vfo(rig, vorig);
		CHKERR(retval);
	}

	return RIG_OK;
}

/*
 *	status:	working
 */
int ts2k_get_mem(RIG * rig, vfo_t vfo, int *ch)
{
	int retval, skip;
	vfo_t vorig;
	TS2K_MC_T param;

	skip = (vfo & RIG_VFO_CURR) || (vfo & RIG_MEM_CURR);

	if( !skip ) {
		retval = rig_get_vfo(rig, &vorig);
		CHKERR(retval);
	}

	switch(vfo) {
	case RIG_MEM_A:
	case RIG_MEM_C:
		retval = rig_set_vfo(rig, vfo);
		/* no break */

	case RIG_MEM_CURR:	// Like RIG_VFO_CURR but for mem
	case RIG_VFO_CURR:	// compatibility with existing code
		retval = ts2k_g_mc(rig, &param);
		CHKERR(retval);
		*ch = param.p1;
		break;

	case RIG_MEMO:	// scratchpad or temp memories
		return -RIG_ENIMPL;
		break;

	case RIG_SAT_MEM:
		return -RIG_ENIMPL;
		break;
	
	default:
		rig_debug(RIG_DEBUG_ERR, __FUNCTION__
			": Unknown mem = %s\n", strvfo(vfo));
		return -RIG_ENAVAIL;
		break;
	}

	if( !skip ) {
		retval = rig_set_vfo(rig, vorig);
		CHKERR(retval);
	}

	return RIG_OK;
}

/*
 *	status:	untested
 */
const char *ts2k_get_info(RIG * rig)
{
	return NULL;
}

/*
 *	status:	untested
 */
int ts2k_set_rit(RIG * rig, vfo_t vfo, shortfreq_t rit)
{
	STDPARAM;
	union {
		TS2K_RD_S_T rd;
		TS2K_RU_S_T ru;	// 'nuther TS2K bug or manual error
		TS2K_RC_T rc;
		TS2K_RT_T rt;
	} param;

	// VFO doesn't really apply, except it's unavailable for Sub.
	TESTVFO2(skip);	// original vfo contents destroyed.

	switch(vfo) {

	case RIG_VFO_A:
	case RIG_VFO_B:
	case RIG_VFO_AB:
	case RIG_VFO_BA:
		if(rit != 0) {
			if(rit < 0) {
				param.rd.p1 = -rit;	// absolute value
				retval = ts2k_s_rd(rig, &param.rd);
			} else {
				param.ru.p1 = rit;	// rd/ru identical
				retval = ts2k_s_ru(rig, &param.ru);
			}
			CHKERR(retval);
			param.rt.p1 = 1;	// XIT mode on.
		} else {
			param.rt.p1 = 0;	// XIT mode off.
		}
		retval = ts2k_s_rt(rig, &param.rt);	// send on/off to rig
		CHKERR(retval);

		break;

	default:
		rig_debug(RIG_DEBUG_WARN, __FUNCTION__
			": RIT available only on Main.\n");
		RESETVFO(skip);
		return -RIG_ENAVAIL;
	}

	RESETVFO(skip);

	return retval;

}

/*
 * Much more complex than it should be.  Hamlib should only
 *	set the XIT/RIT offset here.  Instead it is turned
 *	on/off too!
 *
 *	status:	untested
 */
int ts2k_set_xit(RIG * rig, vfo_t vfo, shortfreq_t rit)
{
	STDPARAM;
	union {
		TS2K_RD_S_T rd;
		TS2K_RU_S_T ru;	// rigbug
		TS2K_RC_T rc;
		TS2K_XT_T xt;
	} param;

	// VFO doesn't really apply, except it's unavailable for Sub.
	TESTVFO2(skip);	// original vfo contents destroyed.

	switch(vfo) {

	case RIG_VFO_A:
	case RIG_VFO_B:
	case RIG_VFO_AB:
	case RIG_VFO_BA:
		if(rit != 0) {
			if(rit < 0) {
				param.rd.p1 = -rit;	// absolute value
				retval = ts2k_s_rd(rig, &param.rd);
			} else {
				param.ru.p1 = rit;	// rd/ru identical
				retval = ts2k_s_ru(rig, &param.ru);
			}
			CHKERR(retval);
			param.xt.p1 = 1;	// XIT mode on.
		} else {
			param.xt.p1 = 0;	// XIT mode off.
		}
		retval = ts2k_s_xt(rig, &param.xt);	// send on/off to rig
		CHKERR(retval);

		break;

	default:
		rig_debug(RIG_DEBUG_WARN, __FUNCTION__
			": XIT available only on Main.\n");
		RESETVFO(skip);
		return -RIG_ENAVAIL;
	}

	RESETVFO(skip);

	return retval;
}

/*
 *	status:	untested
 */
int ts2k_get_xit(RIG * rig, vfo_t vfo, shortfreq_t * rit)
{
	STDPARAM;
	TS2K_IF_T param;

	TESTVFO(skip);

	retval = ts2k_g_if(rig, &param);
	CHKERR(retval);

	if((param.p5 == 0) || (vfo & RIG_CTRL_SUB)) {
		*rit = 0;	// Just do it Hamlib's way.
	} else {
		*rit = param.p3;	// Actual value even zero!
	}

	RESETVFO(skip);

	return retval;

}

/*
 * RigBug:  XIT/RIT only on Main but displays as set
 *	on Sub.  This is wrong!
 *
 *	status:	untested
 */
int ts2k_get_rit(RIG * rig, vfo_t vfo, shortfreq_t * rit)
{
	STDPARAM;
	TS2K_IF_T param;

	TESTVFO(skip);

	retval = ts2k_g_if(rig, &param);
	CHKERR(retval);

	if((param.p4 == 0) || (vfo & RIG_CTRL_SUB)) {
		*rit = 0;	// Just do it Hamlib's way.
	} else {
		*rit = param.p3;	// Actual value even zero!
	}

	RESETVFO(skip);	

	return retval;
}

/*
 *	status:	untested
 */
int ts2k_get_ts(RIG * rig, vfo_t vfo, shortfreq_t * ts)
{
	STDPARAM;
	TS2K_ST_T param;

	TESTVFO(skip);

	retval = ts2k_g_st(rig, &param);
	*ts = param.p1;

	RESETVFO(skip);

	return -RIG_ENIMPL;	// untested
	return retval;
}

/*
 *	status:	untested
 */
int ts2k_set_ts(RIG * rig, vfo_t vfo, shortfreq_t ts)
{
	STDPARAM;
	TS2K_ST_T param;

	TESTVFO(skip);

	param.p1 = ts;
	retval = ts2k_s_st(rig, &param);

	RESETVFO(skip);

	return -RIG_ENIMPL;	// untested
	return retval;
}

/*
 *	status:	untested
 */
int ts2k_set_tone_sql(RIG *rig, vfo_t vfo, tone_t state)
{
	STDPARAM;
	TS2K_TO_T param;

	TESTVFO(skip);

	param.p1 = (state == 0)? 0: 1;	// 0 = off;
	retval = ts2k_s_to(rig, &param);

	RESETVFO(skip);

	return retval;
}

/**
 * \brief Turn on/off ctcss squelch.
 *
 *	status:	untested
 */
int ts2k_set_ctcss_sql(RIG *rig, vfo_t vfo, tone_t state)
{
	STDPARAM;
	TS2K_CT_T param;

	TESTVFO(skip);

	param.p1 = (state == 0)? 0: 1;	// 0 = off;
	retval = ts2k_s_ct(rig, &param);

	RESETVFO(skip);

	return retval;
}

/*
 *	status:	untested
 */
int ts2k_set_dcs_sql(RIG *rig, vfo_t vfo, tone_t state)
{
	STDPARAM;
	TS2K_DQ_T param;

	TESTVFO(skip);

	param.p1 = (state == 0)? 0: 1;	// 0 = off;
	retval = ts2k_s_dq(rig, &param);

	RESETVFO(skip);

	return retval;
}

/*
 *	status:	untested
 */
int ts2k_get_tone_sql(RIG *rig, vfo_t vfo, tone_t *state)
{
	STDPARAM;
	TS2K_TO_T param;

	TESTVFO(skip);

	retval = ts2k_g_to(rig, &param);
	*state = param.p1;	// 0 = off;

	RESETVFO(skip);

	return retval;
}

/*
 *	status:	untested
 */
int ts2k_get_ctcss_sql(RIG *rig, vfo_t vfo, tone_t *state)
{
	STDPARAM;
	TS2K_CT_T param;

	TESTVFO(skip);

	retval = ts2k_g_ct(rig, &param);
	*state = param.p1;	// 0 = off;

	RESETVFO(skip);

	return retval;
}

/*
 *	status:	untested
 */
int ts2k_get_dcs_sql(RIG *rig, vfo_t vfo, tone_t *state)
{
	STDPARAM;
	TS2K_DQ_T param;

	TESTVFO(skip);

	retval = ts2k_g_dq(rig, &param);
	*state = param.p1;	// 0 = off;

	RESETVFO(skip);

	return retval;
}

/**
 * \brief Set tone (rig tx tone, repeater rx tone)
 *
 *	status: untested
 */
int ts2k_set_tone(RIG * rig, vfo_t vfo, tone_t tone)
{
	STDPARAM;
	int i, low_idx, hi_idx, t;
	TS2K_TN_T param;

	TESTVFO(skip);

	param.p1 = ts2k_uniq_tone2idx(tone);	// only the index is sent to rig
	retval = ts2k_s_tn(rig, &param);

	RESETVFO(skip);

	return retval;
}

/**
 * \brief Set tone (rig tx tone, repeater rx tone)
 *
 *	status: untested
 */
int ts2k_set_ctcss(RIG * rig, vfo_t vfo, tone_t tone)
{
	STDPARAM;
	TS2K_CN_T param;

	TESTVFO(skip);

	param.p1 = ts2k_uniq_tone2idx(tone);	// only the index is sent to rig
	retval = ts2k_s_cn(rig, &param);

	RESETVFO(skip);

	return retval;
}

/**
 * \brief Set dcs tone
 *
 *	status: untested
 */
int ts2k_set_dcs(RIG * rig, vfo_t vfo, tone_t tone)
{
	STDPARAM;
	int i, low_idx, hi_idx, t;
	TS2K_QC_T param;

	TESTVFO(skip);

	param.p1 = ts2k_uniq_dcs2idx(tone);	// only the index is sent to rig
	retval = ts2k_s_qc(rig, &param);

	RESETVFO(skip);

	return retval;
}

/*
 * \brief Get ctcss (rig rx tone, repeater tx tone)
 *	to \a nearest value in tenths of Hz.
 *
 *	status:	untested
 */
int ts2k_get_ctcss(RIG * rig, vfo_t vfo, tone_t * tone)
{
	STDPARAM;
	TS2K_CN_T param;

	TESTVFO(skip);

	retval = ts2k_g_cn(rig, &param);
	*tone = ts2k_ctcss_list[param.p1 - 1];

	RESETVFO(skip);

	return retval;
}

/*
 * \brief Set tone (rig tx tone, repeater rx tone)
 *	to \a nearest value in tenths of Hz.
 *
 *	status:	untested
 */
int ts2k_get_tone(RIG * rig, vfo_t vfo, tone_t * tone)
{
	STDPARAM;
	TS2K_TN_T param;

	TESTVFO(skip);

	retval = ts2k_g_tn(rig, &param);
	*tone = ts2k_ctcss_list[param.p1 - 1];

	RESETVFO(skip);

	return retval;
}

/*
 *	status:	untested
 */
int ts2k_get_dcs(RIG * rig, vfo_t vfo, tone_t * tone)
{
	STDPARAM;
	TS2K_QC_T param;

	TESTVFO(skip);

	retval = ts2k_g_qc(rig, &param);
	*tone = ts2k_dcs_list[param.p1 - 1];

	RESETVFO(skip);

	return retval;
}

/*
 *	status:	untested
 */
int ts2k_get_rptr_offs(RIG * rig, vfo_t vfo, shortfreq_t * rptr_offs)
{
	STDPARAM;
	TS2K_OF_T param;

	TESTVFO(skip);

	retval = ts2k_g_of(rig, &param);
	*rptr_offs = param.p1;

	RESETVFO(skip);

	return retval;
}

/*
 *	status:	untested
 */
int ts2k_set_rptr_offs(RIG * rig, vfo_t vfo, shortfreq_t rptr_offs)
{
	STDPARAM;
	TS2K_OF_T param;

	TESTVFO(skip);

	param.p1 = rptr_offs;
	retval = ts2k_s_of(rig, &param);

	RESETVFO(skip);

	return retval;
}

/*
 *	status:	untested
 */
int ts2k_get_rptr_shift(RIG * rig, vfo_t vfo, rptr_shift_t * rptr_shift)
{
	STDPARAM;
	TS2K_OS_T param;

	TESTVFO(skip);

	retval = ts2k_g_os(rig, &param);
	*rptr_shift = ts2k_shift_list[param.p1];

	RESETVFO(skip);

	return retval;
}

/*
 *	status:	untested
 */
int ts2k_set_rptr_shift(RIG * rig, vfo_t vfo, rptr_shift_t rptr_shift)
{
	STDPARAM;
	TS2K_OS_T param;

	TESTVFO(skip);

	param.p1 = ts2k_uniq_shift2idx(rptr_shift);
	retval = ts2k_s_os(rig, &param);

	RESETVFO(skip);

	return retval;
}

/*
 *	status:	untested
 */
int ts2k_get_split(RIG * rig, vfo_t vfo, split_t * split)
{
	vfo_t vtmp;
	int retval;

	retval = rig_get_vfo(rig, &vtmp);
	
	return vtmp & RIG_CTRL_SPLIT;	// Why have a function for it?
}

/*
 *	status:	untested
 */
int ts2k_set_split(RIG * rig, vfo_t vfo, split_t split)
{
	return rig_set_vfo(rig, RIG_VFO_AB);	// Why have a function for it?
}

/*
 *	status:	untested
 */
int ts2k_get_split_freq(RIG * rig, vfo_t vfo, freq_t * tx_freq)
{
	STDPARAM;
	union {
		TS2K_MR_T mr;
		TS2K_FA_T fa;
	} param;

	TESTVFO2(skip);

	switch(vfo) {
	case RIG_VFO_AB:
	//case RIG_VFO_MB:
		retval = ts2k_g_fb(rig, &param.fa);
		CHKERR(retval);
		*tx_freq = param.fa.p1;
		break;

	case RIG_VFO_BA:
	case RIG_VFO_MA:
		retval = ts2k_g_fa(rig, &param.fa);
		CHKERR(retval);
		*tx_freq = param.fa.p1;
		break;

	case RIG_VFO_AM:
	case RIG_VFO_BM:	// FIXME:
		return -RIG_ENIMPL;	// mem/vfo split, if enabled
		break;

	default:
		return -RIG_EINVAL;	// Gotta know which VFO!!
		break;
	}

	RESETVFO(skip);

	return retval;
}

/*
 *	status:	untested
 */
int ts2k_set_split_freq(RIG * rig, vfo_t vfo, freq_t tx_freq)
{
	STDPARAM;
	union {
		TS2K_MW_T mw;
		TS2K_FA_T fa;
	} param;

	TESTVFO2(skip);

	switch(vfo) {
	case RIG_VFO_AB:
	case RIG_VFO_MB:
		param.fa.p1 = tx_freq;
		retval = ts2k_s_fb(rig, &param.fa);
		CHKERR(retval);
		break;

	case RIG_VFO_BA:
	case RIG_VFO_MA:
		param.fa.p1 = tx_freq;
		retval = ts2k_s_fa(rig, &param.fa);
		CHKERR(retval);
		break;

	case RIG_VFO_AM:	// Must be enabled via menu first!
	case RIG_VFO_BM:	// FIXME:
		return -RIG_ENIMPL;	// mem/vfo split, if enabled
		break;

	default:
		return -RIG_EINVAL;	// Gotta know which VFO!!
		break;
	}

	RESETVFO(skip);

	return retval;
}

/*
 *	status:	untested
 */
int ts2k_set_split_mode(RIG * rig,
			vfo_t vfo, rmode_t txmode, pbwidth_t txwidth)
{
	STDPARAM;
/*	union {
		TS2K_MW_T mw;
	} param;
*/
	TESTVFO2(skip);

	switch(vfo) {
	case RIG_VFO_AB:
	case RIG_VFO_MB:
		retval = rig_set_mode(rig, RIG_VFO_B, txmode, txwidth);
		CHKERR(retval);
		break;

	case RIG_VFO_BA:
	case RIG_VFO_MA:
		retval = rig_set_mode(rig, RIG_VFO_A, txmode, txwidth);
		CHKERR(retval);
		break;

	case RIG_VFO_AM:	// Must be enabled via menu first!
	case RIG_VFO_BM:	// FIXME:
		return -RIG_ENIMPL;	// mem/vfo split, if enabled
		break;

	default:
		return -RIG_EINVAL;	// Gotta know which VFO!!
		break;
	}

	RESETVFO(skip);

	return retval;

}

/*
 *	status:	untested
 */
int ts2k_get_split_mode(RIG * rig,
	vfo_t vfo, rmode_t * txmode, pbwidth_t * txwidth)
{
	STDPARAM;
/*	union {
		TS2K_MR_T mr;
		TS2K_MD_T md;
	} param;
*/
	TESTVFO2(skip);

	switch(vfo) {
	case RIG_VFO_AB:	// Have to switch this time.
	case RIG_VFO_MB:	// FIXME:
		retval = rig_get_mode(rig, RIG_VFO_B, txmode, txwidth);
		CHKERR(retval);
		break;

	case RIG_VFO_BA:
	case RIG_VFO_MA:
		retval = rig_get_mode(rig, RIG_VFO_A, txmode, txwidth);
		CHKERR(retval);
		break;

	case RIG_VFO_AM:	// Must be enabled via menu first!
	case RIG_VFO_BM:
		return -RIG_ENIMPL;	// mem/vfo split, if enabled
		break;

	default:
		return -RIG_EINVAL;	// Gotta know which VFO!!
		break;
	}

	RESETVFO(skip);

	return retval;
}

/**
 * \brief Get a channel, which includes mode, memory
 *	offset, etc...  This function calls other
 *	routines which perform the actual operations.
 *
 *	status:	untested
 */
int ts2k_get_channel(RIG * rig, channel_t * chan)
{
	STDPARAM;
	vfo_t vfo;

	skip = 1;	// don't reset VFO unless required.

	vfo = chan->vfo;

	rig_debug(RIG_DEBUG_ERR,__FUNCTION__": chan = %i\n", chan->channel_num);

	switch(vfo) {
	/* Simple Real Targets First */

	/* Allow everything to do channel ops */
	case RIG_VFO_A:
	case RIG_VFO_B:
	case RIG_VFO_C:
		return -RIG_ENIMPL;
		break;

	/* Of course, we Must do memory */
	/* We set channel_num and set ->next to NULL */
	case RIG_MEM_A:
		retval = rig_get_mem(rig, RIG_MEM_A, &chan->channel_num);
		chan->next = NULL;
		return ts2k_uniq_GetMem(rig, chan);

	case RIG_MEM_B:
		return -RIG_ENIMPL;

	case RIG_MEM_C:	// Indirect memory access
		retval = rig_get_mem(rig, RIG_MEM_A, &chan->channel_num);
		chan->next = NULL;
		return ts2k_uniq_GetMem(rig, chan);
		break;

	/* and someday SAT memory */
	case RIG_SAT_MEM:
		return -RIG_ENIMPL;
		break;

	/* Fake Targets */

	case RIG_VFO_CURR:	// maybe move above vfoa
		return -RIG_ENIMPL;
		break;

	/* User responsible for setting ->channel_num and ->next! */
	case RIG_VFO_MEM:	// Direct memory access
		chan->next = NULL;	// debug
		return ts2k_uniq_GetMem(rig, chan);
		break;

	/* Complex Real Targets Last */

	default:
		return -RIG_ENIMPL;
		break;
	}

	RESETVFO(skip);

	return RIG_OK;
}

/*
 *	status:	untested
 */
int ts2k_set_channel(RIG * rig, channel_t * chan)
{
	STDPARAM;
	vfo_t vfo;

	skip = 1;	// don't reset VFO unless required.

	vfo = chan->vfo;

	rig_debug(RIG_DEBUG_ERR,__FUNCTION__": chan = %i\n", chan->channel_num);

	switch(vfo) {
	/* Simple Real Targets First */

	/* Allow everything to do channel ops */
	case RIG_VFO_A:
	case RIG_VFO_B:
	case RIG_VFO_C:
		return -RIG_ENIMPL;
		break;

	/* Of course, we Must do memory */
	/* We set channel_num and set ->next to NULL */
	case RIG_MEM_A:
		retval = rig_get_mem(rig, RIG_MEM_A, &chan->channel_num);
		chan->next = NULL;
		return ts2k_uniq_SetMem(rig, chan);

	case RIG_MEM_B:
		return -RIG_ENIMPL;

	case RIG_MEM_C:	// Indirect memory access
		retval = rig_get_mem(rig, RIG_MEM_C, &chan->channel_num);
		chan->next = NULL;
		return ts2k_uniq_SetMem(rig, chan);
		break;

	/* and someday SAT memory */
	case RIG_SAT_MEM:
		return -RIG_ENIMPL;
		break;

	/* Fake Targets */

	case RIG_VFO_CURR:	// maybe move above vfoa
		return -RIG_ENIMPL;
		break;

	/* User responsible for setting ->channel_num and ->next! */
	case RIG_VFO_MEM:	// Direct memory access
		chan->next = NULL;	// debug
		return ts2k_uniq_SetMem(rig, chan);
		break;

	/* Complex Real Targets Last */

	default:
		return -RIG_ENIMPL;
		break;
	}

	RESETVFO(skip);

	return RIG_OK;
}

/*
 *	status:	untested
 */
int ts2k_scan(RIG * rig, vfo_t vfo, scan_t scan, int ch)
{
	STDPARAM;
	TS2K_SC_T param;

	if(vfo == RIG_VFO_CURR) {
		retval = rig_get_vfo(rig, &vfo);
		CHKERR(retval);
	}

	TESTVFO(skip);

	switch(scan) {
	case RIG_SCAN_STOP:
		param.p1 = 0;
		break;

	// The following three require proper VFO's
	case RIG_SCAN_VFO:
	case RIG_SCAN_MEM:
	case RIG_SCAN_PRIO:
		param.p1 = 1;
		break;

	case RIG_SCAN_SLCT:
		return -RIG_ENIMPL;
		break;

	case RIG_SCAN_PROG:
		return -RIG_ENIMPL;
		break;

	case RIG_SCAN_DELTA:
		return -RIG_ENIMPL;
		break;

	default:
		return -RIG_ENAVAIL;
	}

	retval = ts2k_s_sc(rig, &param);

	RESETVFO(skip);

	return retval;
}

/*
 *	status:	untested
 */
int ts2k_set_level(RIG * rig, vfo_t vfo, setting_t level, value_t value)
{
	STDPARAM;
	int ret, val;	// return value for set func
	int Main;
	union {
		TS2K_AG_T	ag;	// AF gain
		TS2K_AM_T	am;	// auto mode
		TS2K_BC_T	bc;	// BC on/off
		TS2K_GT_T	gt;	// AGC speed
		TS2K_LK_T	lk;	// Lock
		TS2K_ML_T	ml;	// Output Monitor
		TS2K_NB_T	nb;	// NB on/off
		TS2K_NR_T	nr;	// NR on/off
		TS2K_NT_T	nt;	// A.Notch on/off
		TS2K_SA_A_T	sa;	// SAT
		TS2K_SC_T	sc;	// band scan, sort-of
		TS2K_SD_T	sd;	// CW break-in
		TS2K_SQ_T	sq;	// Squelch
		TS2K_TN_T	tn;	// Tone sql
		TS2K_TO_T	to;	// Tone val
		TS2K_VX_T	vx;	// VOX on/off
		//TS2K_NB_T	nb;	// 
	} param;

	Main = (vfo & RIG_CTRL_MAIN)? 0: 1;	// Main=0, Sub=1
	val = value.i;

	TESTVFO(skip);

#define ON_OFF	(val > 0)? 1: 0
#define MAX(_v)	(val > _v)? _v: val

	switch(level) {

	case RIG_FUNC_FAGC:	/* Fast AGC */
		param.gt.p1 = MAX(20);
		ret = ts2k_s_gt(rig, &param.gt);
		break;

	case RIG_FUNC_NB:	/* Noise Blanker */
		param.nb.p1 = ON_OFF;
		ret = ts2k_s_nb(rig, &param.nb);
		break;

	case RIG_FUNC_COMP:	/* Compression */
		ret = -RIG_ENIMPL;
		break;

	case RIG_FUNC_VOX:	/* VOX */
		param.vx.p1 = ON_OFF;
		ret = ts2k_s_vx(rig, &param.vx);
		break;

	/* Duplicate!!! Use rig_set_tone() or move DCS/CTCSS here! */
	case RIG_FUNC_TONE:	/* Tone */
		param.tn.p1 = ts2k_uniq_tone2idx(val); 
		ret = ts2k_s_tn(rig, &param.tn);
		break;

	/* Duplicate!!! Use rig_set_tone_sql() */
	case RIG_FUNC_TSQL:	/* may require a tone field */
		param.to.p1 = ON_OFF;
		ret = ts2k_s_to(rig, &param.to);
		break;

	case RIG_FUNC_SBKIN:	/* Semi Break-in */
		param.sd.p1 = MAX(1000);
		ret = ts2k_s_sd(rig, &param.sd);
		break;

	case RIG_FUNC_FBKIN:	/* Full Break-in, for CW mode */
		param.sd.p1 = ON_OFF;
		ret = ts2k_s_sd(rig, &param.sd);
		break;

	case RIG_FUNC_ANF:	/* Automatic Notch Filter (DSP) */
		param.nt.p1 = ON_OFF;
		ret = ts2k_s_nt(rig, &param.nt);
		break;

	case RIG_FUNC_NR:	/* Noise Reduction (DSP) */
		param.nr.p1 = ON_OFF;
		ret = ts2k_s_nr(rig, &param.nr);
		break;

	case RIG_FUNC_AIP:	/* AIP (Kenwood) */
		ret = -RIG_ENIMPL;	// Huh?
		break;

	case RIG_FUNC_APF:	/* Auto Passband Filter */
		ret = -RIG_ENIMPL;
		break;

	case RIG_FUNC_MON:	/* Monitor transmitted signal, != rev */
		param.ml.p1 = ON_OFF;
		ret = ts2k_s_ml(rig, &param.ml);
		break;

	case RIG_FUNC_MN:	/* Manual Notch (Icom) */
		ret = -RIG_ENAVAIL;
		break;

	case RIG_FUNC_RNF:	/* RTTY Filter Notch (Icom) */
		ret = -RIG_ENAVAIL;	// menu(50A) on + "sl;", "sh;"?
		break;

	case RIG_FUNC_ARO:	/* Auto Repeater Offset */
		ret = -RIG_ENIMPL;
		break;

	case RIG_FUNC_LOCK:	/* Lock */
		param.lk.p1 = MAX(2);
		param.lk.p2 = 0;
		ret = ts2k_s_lk(rig, &param.lk);
		break;

	/* I mute both Main and Sub */
	case RIG_FUNC_MUTE:	/* Mute, could be emulated by LEVEL_AF */
		param.ag.p1 = 0;	// Main
		param.ag.p2 = MAX(50);	// Audio Gain
		ret = ts2k_s_ag(rig, &param.ag);
		param.ag.p1 = 1;	// Sub
		param.ag.p2 = MAX(50);	// (max=255, but 50 is loud!)
		ret = ts2k_s_ag(rig, &param.ag);
		break;

	case RIG_FUNC_VSC:	/* Voice Scan Control */
		ret = -RIG_ENAVAIL;
		break;

	case RIG_FUNC_SQL:	/* Turn Squelch on/off */
		param.sq.p1 = Main;	// Main=0, Sub=1
		param.sq.p2 = MAX(255);
		ret = ts2k_s_sq(rig, &param.sq);
		break;

	case RIG_FUNC_ABM:	/* Auto Band Mode */
		param.am.p1 = ON_OFF;
		ret = ts2k_s_am(rig, &param.am);
		break;

	case RIG_FUNC_BC:	/* Beat Canceller */
		param.bc.p1 = ON_OFF;
		ret = ts2k_s_bc(rig, &param.bc);
		break;

	case RIG_FUNC_MBC:	/* Manual Beat Canceller */
		param.bc.p1 = (val>0)? 2: 0;
		ret = ts2k_s_bc(rig, &param.bc);
		break;

	case RIG_FUNC_LMP:	/* LCD lamp ON/OFF */
		ret = -RIG_ENIMPL;
		break;

	case RIG_FUNC_AFC:	/* Auto Frequency Control ON/OFF */
		ret = -RIG_ENIMPL;
		break;

	/* Caution: The following depends up the TS2K_SA_S_T and
	 * TS2K_SA_A_T being aligned in the above union.  If SAT
	 * don't work using this function, this is the reason! */

	case RIG_FUNC_SATMODE:	/* Satellite mode ON/OFF (IC-910H) */
		ret = ts2k_g_sa(rig, &param.sa);	// Read current values
		param.sa.p1 = ON_OFF;
		ret = ts2k_s_sa(rig, (TS2K_SA_S_T *) &param.sa);	// Just toggle on/off
		break;

	/* No bandscope, but I turn on a similar function... */
	case RIG_FUNC_SCOPE:	/* Simple bandscope ON/OFF (IC-910H) */
		param.sc.p1 = (val>0)? 3: 0;
		ret = ts2k_s_sc(rig, &param.sc);
		break;

	case RIG_FUNC_RESUME:	/* Scan resume */
		ret = -RIG_ENAVAIL;
		break;

	case RIG_FUNC_TBURST:	/* 1750 Hz tone burst */
		ret = -RIG_ENIMPL;
		break;

	default:
		ret = -RIG_ENAVAIL;
		break;
	}

#undef ON_OFF
#undef MAX

	RESETVFO(skip);

	return ret;
}

/*
 *	status:	untested
 */
int ts2k_get_level(RIG * rig, vfo_t vfo, setting_t level, value_t * value)
{
	STDPARAM;
	int ret, val;
	int Main;
	union {
		TS2K_AG_T	ag;	// AF gain
		TS2K_AM_T	am;	// auto mode
		TS2K_BC_T	bc;	// BC on/off
		TS2K_GT_T	gt;	// AGC speed
		TS2K_LK_T	lk;	// Lock
		TS2K_ML_T	ml;	// Output Monitor
		TS2K_NB_T	nb;	// NB on/off
		TS2K_NR_T	nr;	// NR on/off
		TS2K_NT_T	nt;	// A.Notch on/off
		TS2K_SA_R_T	sa;	// SAT
		TS2K_SC_T	sc;	// band scan, sort-of
		TS2K_SD_T	sd;	// CW break-in
		TS2K_SQ_T	sq;	// Squelch
		TS2K_TN_T	tn;	// Tone sql
		TS2K_TO_T	to;	// Tone val
		TS2K_VX_T	vx;	// VOX on/off
		//TS2K_NB_T	nb;	// 
	} param;

	Main = (vfo & RIG_CTRL_MAIN)? 0: 1;	// Main=0, Sub=1
	val = 0;

	TESTVFO(skip);

#define TRUEVAL(_p, _v)	val = (_p == _v)? 1: 0
#define MAX(_v)	(val > _v)? _v: val

	switch(level) {

	case RIG_FUNC_FAGC:	/* Fast AGC */
		ret = ts2k_g_gt(rig, &param.gt);
		val = param.gt.p1;
		break;

	case RIG_FUNC_NB:	/* Noise Blanker */
		ret = ts2k_g_nb(rig, &param.nb);
		val = param.nb.p1;
		break;

	case RIG_FUNC_COMP:	/* Compression */
		ret = -RIG_ENIMPL;
		break;

	case RIG_FUNC_VOX:	/* VOX */
		ret = ts2k_g_vx(rig, &param.vx);
		val = param.vx.p1;
		break;

	/* Duplicate!!! Use rig_set_tone() or move DCS/CTCSS here! */
	case RIG_FUNC_TONE:	/* Tone */
		ret = ts2k_g_tn(rig, &param.tn);
		val = param.tn.p1;
		break;

	/* Duplicate!!! Use rig_set_tone_sql() */
	case RIG_FUNC_TSQL:	/* may require a tone field */
		ret = ts2k_g_to(rig, &param.to);
		val = param.to.p1;
		break;

	case RIG_FUNC_SBKIN:	/* Semi Break-in */
		ret = ts2k_g_sd(rig, &param.sd);
		val = param.sd.p1;
		break;

	case RIG_FUNC_FBKIN:	/* Full Break-in, for CW mode */
		ret = ts2k_g_sd(rig, &param.sd);
		val = param.sd.p1;
		break;

	case RIG_FUNC_ANF:	/* Automatic Notch Filter (DSP) */
		ret = ts2k_g_nt(rig, &param.nt);
		val = param.nt.p1;
		break;

	case RIG_FUNC_NR:	/* Noise Reduction (DSP) */
		ret = ts2k_g_nr(rig, &param.nr);
		val = param.nr.p1;
		break;

	case RIG_FUNC_AIP:	/* AIP (Kenwood) */
		ret = -RIG_ENIMPL;	// Huh?
		break;

	case RIG_FUNC_APF:	/* Auto Passband Filter */
		ret = -RIG_ENIMPL;
		break;

	case RIG_FUNC_MON:	/* Monitor transmitted signal, != rev */
		ret = ts2k_g_ml(rig, &param.ml);
		val = param.ml.p1;
		break;

	case RIG_FUNC_MN:	/* Manual Notch (Icom) */
		ret = -RIG_ENAVAIL;
		break;

	case RIG_FUNC_RNF:	/* RTTY Filter Notch (Icom) */
		ret = -RIG_ENAVAIL;	// menu(50A) on + "sl;", "sh;"?
		break;

	case RIG_FUNC_ARO:	/* Auto Repeater Offset */
		ret = -RIG_ENIMPL;
		break;

	case RIG_FUNC_LOCK:	/* Lock */
		ret = ts2k_g_lk(rig, &param.lk);
		val = param.lk.p1;
		break;

	/* I mute both Main and Sub */
	case RIG_FUNC_MUTE:	/* Mute, could be emulated by LEVEL_AF */
		param.ag.p1 = Main;	// Sub
		ret = ts2k_g_ag(rig, &param.ag);
		val = param.ag.p2;	// p2 is correct
		break;

	case RIG_FUNC_VSC:	/* Voice Scan Control */
		ret = -RIG_ENAVAIL;
		break;

	case RIG_FUNC_SQL:	/* Turn Squelch on/off */
		param.sq.p1 = Main;	// Main=0, Sub=1
		ret = ts2k_g_sq(rig, &param.sq);
		val = param.sq.p2;	// p2 is correct
		break;

	case RIG_FUNC_ABM:	/* Auto Band Mode */
		ret = ts2k_g_am(rig, &param.am);
		val = param.am.p1;	// p2 is correct
		break;

	case RIG_FUNC_BC:	/* Beat Canceller */
		ret = ts2k_g_bc(rig, &param.bc);
		val = TRUEVAL(param.bc.p1, 1);
		break;

	case RIG_FUNC_MBC:	/* Manual Beat Canceller */
		ret = ts2k_g_bc(rig, &param.bc);
		val = TRUEVAL(param.bc.p1, 2);
		break;

	case RIG_FUNC_LMP:	/* LCD lamp ON/OFF */
		ret = -RIG_ENIMPL;
		break;

	case RIG_FUNC_AFC:	/* Auto Frequency Control ON/OFF */
		ret = -RIG_ENIMPL;
		break;

	/* Caution: The following depends up the TS2K_SA_S_T and
	 * TS2K_SA_A_T being aligned in the above union.  If SAT
	 * don't work using this function, this is the reason! */

	case RIG_FUNC_SATMODE:	/* Satellite mode ON/OFF (IC-910H) */
		ret = ts2k_g_sa(rig, (TS2K_SA_A_T *) &param.sa);	// Read current values
		TRUEVAL(param.sa.p1, 1);
		break;

	/* No bandscope, but I turn on a similar function... */
	case RIG_FUNC_SCOPE:	/* Simple bandscope ON/OFF (IC-910H) */
		ret = ts2k_g_sc(rig, &param.sc);
		TRUEVAL(param.sc.p1, 3);
		break;

	case RIG_FUNC_RESUME:	/* Scan resume */
		ret = -RIG_ENAVAIL;
		break;

	case RIG_FUNC_TBURST:	/* 1750 Hz tone burst */
		ret = -RIG_ENIMPL;
		break;

	default:
		ret = -RIG_ENAVAIL;
		break;
	}

	value->i = val;	// to lazy to write casts on everything!!!

	RESETVFO(skip);

	return ret;
}

/*
 *	status:	untested
 */
int ts2k_set_func(RIG * rig, vfo_t vfo, setting_t func, int status)
{
	return -RIG_ENIMPL;
}

/*
 *	status:	untested
 */
int ts2k_get_func(RIG * rig, vfo_t vfo, setting_t func, int *status)
{
	return -RIG_ENIMPL;
}

/*
 *	status:	untested
 */
int ts2k_get_parm(RIG * rig, setting_t parm, value_t * val)
{
	return -RIG_ENIMPL;
}

/*
 *	status:	untested
 */
int ts2k_set_parm(RIG * rig, setting_t parm, value_t val)
{
	return -RIG_ENIMPL;
}

/*
 */
int ts2k_send_morse(RIG * rig, vfo_t vfo, const char *msg)
{
	return -RIG_ENIMPL;
}

/*
 */
int ts2k_vfo_op(RIG * rig, vfo_t vfo, vfo_op_t op)
{
	return -RIG_ENIMPL;
}

/*
 */
int ts2k_set_trn(RIG * rig, int trn)
{
	return -RIG_ENIMPL;
}

/*
 */
int ts2k_get_trn(RIG * rig, int *trn)
{
	return -RIG_ENIMPL;
}

/*
 */
int ts2k_set_powerstat(RIG * rig, powerstat_t status)
{
	return -RIG_ENIMPL;
}

/*
 */
int ts2k_get_powerstat(RIG * rig, powerstat_t * status)
{
	return -RIG_ENIMPL;
}

/*
 *	status:	untested
 */
rig_model_t probe_ts2k(port_t * port)
{
	return -RIG_ENIMPL;
}

/*
 *	status:	untested
 */
int ts2k_get_dcd(RIG * rig, vfo_t vfo, dcd_t * dcd)
{
	return -RIG_ENIMPL;
}

/*
 * ts2k_save_channel() (rig_save_channel() doesn't call us! don't write either)
 *
int ts2k_save_channel(RIG *rig, chan_t *ch)
{
	
}
int ts2k_restore_channel(){}
 This part awaiting Hamlib changes
 */

/* The following are "so-to-be" functions:

int ts2k_init_pm(){}	// done
int ts2k_init_menu(){}	// done
int ts2k_menu_build(){}
int ts2k_menu_parse(){}
int ts2k_menu_list(){}	// done

// The rest is anybody's guess...  --Dale

 * end TODO list
 */

/*
 * Uniq Functions for the TS-2000 are below.
 */

/*
 * PTT/CTRL are assumed to match.  CTRL defines VFO.
 *	Eventually we need to provide the user an option!
 *
 *	status: working
 */
int ts2k_uniq_GetSimpleVfo(RIG *rig, vfo_t *vfo)
{
	int retval, rx, t;
	vfo_t vtmp;
	union {
		TS2K_FR_T rx;
		TS2K_FT_T tx;
		TS2K_DC_T dc;
	} param;

	vtmp = 0;

	// Get Main/Sub
	retval = ts2k_uniq_GetTransceiver(rig, &vtmp);

	// Get Rx vfo
	retval = ts2k_g_fr(rig, &param.rx);
	rx = param.rx.p1;
	CHKERR(retval);

	t = (vtmp & RIG_CTRL_MAIN);

	switch(rx) {
	case 0:		// First vfo
		vtmp |= (t)? RIG_VFO_A: RIG_VFO_C;
		break;

	case 1:		// Second vfo
		vtmp |= (t)? RIG_VFO_B: 0;
		break;

	case 2:		// Mem
		vtmp |= (t)? RIG_MEM_A: RIG_MEM_C;
		break;

	case 3:		// Call
		vtmp |= (t)? RIG_CALL_A: RIG_CALL_C;
		break;

		default:
			rig_debug(RIG_DEBUG_ERR, __FUNCTION__ \
				": Rx not yet recognized.\n");
			break;
	}

	// Get Tx vfo
	retval = ts2k_g_ft(rig, &param.tx);
	CHKERR(retval);

	if( rx != param.tx.p1) {	// We're in Split
		vtmp |= RIG_CTRL_SPLIT;

		switch(param.tx.p1) {
		case 0:
			vtmp |= RIG_VFO_A;
			if(rx==1)
				vtmp |= RIG_CTRL_REV;	// RIG_VFO_BA
			// FIXME: else do mem/vfo split here
			break;

		case 1:
			vtmp |= RIG_VFO_B;
			break;

		case 2:
			vtmp |= RIG_CTRL_MEM;	// Guess this'll work...
			break;

		default:
			rig_debug(RIG_DEBUG_ERR, __FUNCTION__ \
				": Split not yet recognized.\n");
			break;
		}
	}
	*vfo = vtmp;

	return RIG_OK;
}

/*
 * Sets only RIG_VFO_[ABC],
 *		RIG_MEM_[ABC],
 *		RIG_CALL_[AC], no questions asked.
 *
 *	No error checking (e.g. testing for Complex VFOs)
 *
 *	Status:	Sets applicable VFOs correctly!
 */
int ts2k_uniq_SetSimpleVfo(RIG *rig, vfo_t vfo)
{
	vfo_t vtmp;
	int retval, skip;
	union {
		TS2K_FR_T rx;
		TS2K_FT_T tx;
	} param;

	vtmp = skip = 0;

	// Set Main/Sub
	switch(vfo) {
	case RIG_VFO_C:
	case RIG_MEM_C:
	case RIG_CALL_C:
	case RIG_CTRL_SUB:
		vtmp = RIG_CTRL_SUB;
		break;

	case RIG_VFO_A:
	case RIG_VFO_B:
	case RIG_MEM_A:
	case RIG_CALL_A:
	case RIG_VFO_AB:
	case RIG_VFO_BA:
	case RIG_CTRL_MAIN:
		vtmp = RIG_CTRL_MAIN;
		break;

	default:
		skip = 1;	// Force action to not be taken.
		break;
	}

	if(!skip) {
		retval = ts2k_uniq_SetTransceiver(rig, vtmp);
		CHKERR(retval);
		skip = 0;
	}

	// Set Rx (always first, since rig sets Tx to match)
	switch(vfo) {
	case RIG_VFO_A:
	case RIG_VFO_AB:
	case RIG_VFO_C:		// looks strange, but it's correct
		param.rx.p1 = 0;
		break;

	case RIG_VFO_B:
	case RIG_VFO_BA:
		param.rx.p1 = 1;
		break;

	case RIG_MEM_A:
	case RIG_MEM_C:
	//case RIG_VFO_MA:	// mem/vfo
	//case RIG_VFO_MB:	// mem/vfo
		param.rx.p1 = 2;
		break;

	case RIG_CALL_A:
	case RIG_CALL_C:
		param.rx.p1 = 3;
		break;

	default:
		skip = 1;	// Force action to not be taken.
		break;
	}

	if(!skip) {
		retval = ts2k_s_fr(rig, &param.rx);
		CHKERR(retval);
		skip = 0;
	}

	// Set Tx if different
	if(vfo & RIG_CTRL_SPLIT) {
		switch(vfo) {
		case RIG_VFO_AB:
		case RIG_VFO_MA:	// mem/vfo
			param.tx.p1 = 1;
			break;

		case RIG_VFO_BA:
		case RIG_VFO_MB:	// mem/vfo
			param.tx.p1 = 0;
			break;

		// Mem/Vfo split goes here if we ever do it
		// and Menu 6A is On.
		case RIG_VFO_AM:	// mem/vfo
		case RIG_VFO_BM:
			param.tx.p1 = 2;
			break;

		default:
			skip = 1;	// Force action to not be taken.
			break;
		}

		if(!skip) {
			retval = ts2k_s_ft(rig, &param.tx);
			CHKERR(retval);
			//skip = 0;	// not needed here
		}
	}

	return RIG_OK;
}

/*
 * Sets either Main or Sub.  PTT/CTRL match!
 *
 *	status:	working
 */
int ts2k_uniq_SetTransceiver(RIG *rig, vfo_t vfo)
{
	TS2K_DC_T param;

	param.p1 = param.p2 = (vfo & RIG_CTRL_MAIN)? 0: 1;

	return ts2k_s_dc(rig, &param);
}

/*
 * This routine determines whether the Main or Sub
 *	transceiver is selected (CTRL).
 *
 *	Status:	Working.
 */
int ts2k_uniq_GetTransceiver(RIG *rig, vfo_t *vfo)
{
	int retval;
	vfo_t vtmp;
	TS2K_DC_T param;

	vtmp = 0;

	// Get Main/Sub
	retval = ts2k_g_dc(rig, &param);
	CHKERR(retval);

	/*
	 * The following is inadequate.  But, since we have only one VFO passed
	 *	to us, we deal with it as best we can.
	 */
	if(param.p2 == 0) {
		vtmp = RIG_CTRL_MAIN;		// CTRL on Main
		//vtmp = RIG_CTRL_MAIN | RIG_VFO_CTRL;		// CTRL on Main
		//vtmp |= (param.p1 == 0)? RIG_VFO_PTT: 0;	// PTT on Main
	} else if(param.p2 == 1) {
		vtmp = RIG_CTRL_SUB;		// CTRL on Sub
		//vtmp = RIG_CTRL_SUB | RIG_VFO_CTRL;		// CTRL on Sub
		//vtmp |= (param.p1 == 1)? RIG_VFO_PTT: 0;	// PTT on Sub
	} else {
		rig_debug(RIG_DEBUG_ERR, __FUNCTION__ \
			": Transceiver %i not yet recognized.\n", param.p2);
		//return -RIG_EINTERNAL;	// Debugging required
	}
	*vfo = vtmp;

	return RIG_OK;
}

/*
 * FIXME: simple ascii to integer converter--expensive!
 *	Prevents trashing original string, otherwise
 *	just drop a (char)0 where you need it.
 */
//int ts2k_get_int(char *src, int cnt)
long int int_n(char *src, int cnt)
{
//	static char buf[20];
	char buf[20];	// enough for 1e20 - 1
	strncpy(buf, src, cnt);
	buf[cnt] = (char) 0;
	return atol(buf);
//	return atoi(buf);
}

/**
 * \brief Get Rx/Tx pair from memory.  Address must
 *	be set prior to call.  We are using the
 *	chan memory pointer for direct access so VFO is
 *	not used or required.  Other functions are
 *	expected to handle the details.  Linked channels
 *	are followed.  Split memories have the Tx
 *	channel inserted as the next.  The original next
 *	pointer is placed in the Tx chan.  The entire
 *	memory may thus be placed into this structure
 *	in one go.
 *
 *	Notes:	uses recursion
 *		bank=0 for Rx, bank=1 for Tx
 *
 *	status:
 */
int ts2k_uniq_GetMem(RIG *rig, channel_t *chan)
{
	TS2K_MR_T param[2];
	int retval, i, j, am_fm;
	channel_t *p[2], *next;
	vfo_t vfo;

	rig_debug(RIG_DEBUG_ERR,__FUNCTION__": chan = %i\n", chan->channel_num);

	if(chan == NULL)
		return -RIG_EINVAL;	// Come on!  Give me *something*!

	// FIXME: use caps to find memory bounds!
	if(chan->channel_num >= 300)
		return -RIG_EINVAL;	// Oops.

	next = chan->next;	// simplify check at end
	i = chan->channel_num;	// allow mem clear
	vfo = chan->vfo;

	/* Ensure we have Rx/Tx pairs */
	p[0] = chan;
	p[1] = calloc( 1, sizeof(channel_t));
	if(p[1] == NULL)
		return -RIG_ENOMEM;
	memset(p[1], 0, sizeof(channel_t));
	memset(chan, 0, sizeof(channel_t));
	chan->channel_num = i;	// restore
	chan->vfo = vfo;

	/* Get Rx and Tx memory */
	for(i=0; i<2; i++) {
		param[i].p1 = i;
		param[i].p2p3 = chan->channel_num;
		retval = ts2k_g_mr(rig, &param[i]);
		CHKERR(retval);

		p[i]->freq = param[i].p4;
		if(i==1) {
			if(param[0].p4 == param[1].p4) {
				p[0]->split = 0;
				break;
			} else {
				p[0]->split = 1;
				p[1]->split = 1;
				p[1]->width = RIG_PASSBAND_NORMAL;
				p[1]->ant = 0;
				p[1]->next = p[0]->next;
				p[0]->next = p[1];
			}
		}
		p[i]->bank_num = param[i].p1;
		p[i]->channel_num = param[i].p2p3;
		// p4 set first
		p[i]->mode = ts2k_mode_list[param[i].p5];
			am_fm = (p[i]->mode == RIG_MODE_AM);
			am_fm |= (p[i]->mode == RIG_MODE_FM);
			j = (am_fm)? 1: 0;	// index for p14
		p[i]->flags = (param[i].p6)? RIG_CHFLAG_SKIP: RIG_CHFLAG_NONE;

		p[i]->tone_sql = (param[i].p7 == 1)? 1: 0;
		p[i]->ctcss_sql = (param[i].p7 == 2)? 1: 0;
		p[i]->dcs_sql = (param[i].p7 == 3)? 1: 0;

		p[i]->tone = ts2k_ctcss_list[ param[i].p8 - 1 ];
		p[i]->ctcss = ts2k_ctcss_list[ param[i].p9 - 1];
		p[i]->dcs = ts2k_dcs_list[ param[i].p10 - 1 ];
		p[i]->vfo |= (param[i].p11)? RIG_CTRL_REV: 0;	// use VFO for now
		p[i]->rptr_shift = ts2k_shift_list[ param[i].p12 ];
		p[i]->rptr_offs = param[i].p13;
		p[i]->tuning_step = ts2k_steps[j][param[i].p14 - 1];
		p[i]->scan_group = param[i].p15;
		j = (MAXCHANDESC > 8)? 8: MAXCHANDESC;
		strncpy(&p[i]->channel_desc[0], &param[i].p16[0], j);

		p[i]->vfo |= RIG_VFO_MEM;
	}
	p[0]->width = RIG_PASSBAND_NORMAL;
	p[0]->ant = 0;


	if( ! p[0]->split ) {
		free(p[1]);
	}

	// debug
	ts2k_uniq_PrintChan(rig, p[0]);

	/* use up all the stack space. hi! */
	if(next != NULL)
		return ts2k_uniq_GetMem(rig, next);

	return RIG_OK;
}

/*
 * \brief ts2k_uniq_SetMem() reads a list of channel_t
 *	items.  If split is set the next pointer is
 *	a second channel_t which is sent to the Tx
 *	memory after the Rx memory is set.  The next
 *	pointer of the Rx/Tx channel is tested for NULL
 *	and we call this function recursively to process
 *	the remaining channel_t's in the list.
 *
 *	status:	new
 */
int ts2k_uniq_SetMem(RIG *rig, channel_t *chan)
{
	TS2K_MW_T	param[2];
	channel_t	*next, *p[2];
	int		mem[2], i, t, j, k, retval, am_fm;

	rig_debug(RIG_DEBUG_VERBOSE,__FUNCTION__": chan = %i\n", chan->channel_num);

	if(chan == NULL)
		return -RIG_EINVAL;	// Big Oops!

	p[0] = chan;
	p[1] = chan->next;

	mem[0] = p[0]->channel_num;

	// FIXME: use caps for limit
	if(mem[0] > 300)	// 300 is ts2k Call on current
		return -RIG_EINVAL;

	if(chan->split) {	// Rx/Tx split pair
		if(p[1] == NULL) {
			rig_debug(RIG_DEBUG_ERR, __FUNCTION__
				": Tx pointer is NULL but Split is set!\n");
			return -RIG_EINVAL;	// No mem-mem split, yet.
		}
		mem[1] = p[1]->channel_num;
		next = p[1]->next;
		if(mem[0] != mem[1]) {
			rig_debug(RIG_DEBUG_ERR, __FUNCTION__
				": Tx channel different than Rx = %i\n", mem[0]);
			return -RIG_EINVAL;	// No mem-mem split, yet.
		}
		t = 1;	// max index
	} else {		// Single Rx
		next = p[0]->next;
		t = 0;
	}

	/* Loop once or twice for each memory */
	for(i=0; i <= t; i++) {
		rig_debug(RIG_DEBUG_VERBOSE,__FUNCTION__
			": start loop p[%i]\n", i);

		param[i].p1 = i;	// Select Rx/Tx (Rx *always* first!)
		param[i].p2p3 = chan->channel_num;
		param[i].p4 = p[i]->freq;
		param[i].p5 = RIG_MODE_FM;	// kludge
		//ts2k_mode_list[param[i].p5] = p[i]->mode;	// FIXME:
		//	am_fm = (p[i]->mode == RIG_MODE_AM);
		//	am_fm |= (p[i]->mode == RIG_MODE_FM);
		//	j = (am_fm)? 1: 0;	// index for p14

		param[i].p6 = (p[i]->flags & RIG_CHFLAG_SKIP)? 1: 0;

		// FIXME:
		param[i].p7 = (p[i]->tone_sql == 1)? 1: \
					(p[i]->ctcss_sql == 1)? 2: \
						(p[i]->dcs_sql == 1)? 3: 0;
		param[i].p8 = ts2k_uniq_tone2idx(p[i]->tone);
		param[i].p9 = ts2k_uniq_tone2idx(p[i]->ctcss);
		param[i].p10 = ts2k_uniq_dcs2idx(p[i]->dcs);

		param[i].p11 = ((p[i]->vfo & RIG_CTRL_REV) && !p[i]->split)?
					1: 0;	// use VFO for now
		// FIXME:
		//param[i].p12 = p[i]->rptr_shift = ts2k_shift_list[ param[i].p12 ];
		param[i].p12 = 0;	// kludge
		param[i].p13 = p[i]->rptr_offs;
		// FIXME
		//param[i].p14 = p[i]->tuning_step = ts2k_steps[j][param[i].p14 - 1];
		param[i].p14 = 0;	// kludge
		param[i].p15 = p[i]->scan_group;
		j = (MAXCHANDESC > 8)? 8: MAXCHANDESC-1;
		strncpy(&param[i].p16[0], &p[i]->channel_desc[0], j);
		for(k=0; k<=j; k++) {
			if( !isprint( param[i].p16[k])) {
				break;
			}
		}
		param[i].p16[k] = 0;	// Terminate the bugger!

		rig_debug(RIG_DEBUG_VERBOSE,__FUNCTION__
			": writing p[%i]\n", i);

		retval = ts2k_s_mw(rig, &param[i]);
		CHKERR(retval);
	}

	// continue down the list...
	if(next != NULL)
		return ts2k_uniq_SetMem(rig, next);

	return RIG_OK;
}

/*
 * Print a channel_t
 */
int ts2k_uniq_PrintChan(RIG *rig, channel_t *chan)
{
	
	rig_debug(RIG_DEBUG_ERR, "\n");

#define MSG(_a)	rig_debug(RIG_DEBUG_ERR, #_a " = %li\n", (long int)chan-> ## _a )

	MSG(next);
	MSG(channel_num);
	MSG(bank_num);
	MSG(vfo);
	MSG(freq);
	MSG(mode);
	MSG(tx_vfo);
	MSG(tx_freq);
	MSG(tx_mode);
	MSG(tx_width);
	MSG(split);
	MSG(rptr_shift);
	MSG(rptr_offs);
	MSG(tuning_step);	// Value wrong on GetMem
	MSG(rit);
	MSG(xit);
	MSG(funcs);
	//MSG(levels[0]);
	MSG(tone);
	MSG(tone_sql);
	MSG(ctcss);
	MSG(ctcss_sql);
	MSG(dcs);
	MSG(dcs_sql);
	MSG(scan);
	MSG(scan_group);
	MSG(flags);
	MSG(ext_levels);
	rig_debug(RIG_DEBUG_ERR, "channel_desc = %s\n", &chan->channel_desc[0]);

#undef MSG

	rig_debug(RIG_DEBUG_ERR, "\n");

	return RIG_OK;
}

/*
 * return a index given a dcs code
 *
 *	status:	unknown
 */ 
int ts2k_uniq_dcs2idx(tone_t dcs)
{
	int i, cnt;

	cnt = sizeof(ts2k_dcs_list) / sizeof(tone_t) - 1;

	// FIXME:
	// Currently, I just grab the quickest available...
	for(i=0; i<cnt; i++) {
		if(dcs > ts2k_dcs_list[i]) {
			return i-1;
		} else if(dcs == ts2k_dcs_list[i]) {
			return i;
		}
	}

	return i;
}

/*
 * return a index given a tone/ctcss
 *
 *	status: originally from ts2k_set_tone()
 */ 
int ts2k_uniq_tone2idx(int tone)
{
//	STDPARAM;
	int i, low_idx, hi_idx, t;

	hi_idx = (sizeof(ts2k_ctcss_list) / sizeof(int)) - 1;
	low_idx = i = 0;

	rig_debug(RIG_DEBUG_VERBOSE, __FUNCTION__
		"\t: lo = %i, i = %i, hi = %i\n", low_idx, i, hi_idx);

	while( ts2k_ctcss_list[hi_idx] == 0 )
		hi_idx--;	// one or two zeros at end

	rig_debug(RIG_DEBUG_VERBOSE, __FUNCTION__
		"\t:B: lo = %i, i = %i, hi = %i\n", low_idx, i, hi_idx);

	// Here I kludge in a preference for 103.5 (13)
	i = hi_idx/3;

	// do a "fast" search through 38 variable. Hi!
	while( (hi_idx - low_idx) > 1 ) {
		if(ts2k_ctcss_list[i] > tone)
			hi_idx = i;
		else if(ts2k_ctcss_list[i] < tone) {
			low_idx = i;
		}
		if(ts2k_ctcss_list[i] == tone) {
			rig_debug(RIG_DEBUG_VERBOSE, __FUNCTION__
				"\t:exact: lo = %i, i = %i, hi = %i\n", low_idx, i, hi_idx);
			return i+1;
		}
		i = low_idx + (hi_idx - low_idx) / 2;	// mid index

		rig_debug(RIG_DEBUG_VERBOSE, __FUNCTION__
			"\t:M: lo = %i, i = %i, hi = %i\n", low_idx, i, hi_idx);
	}
	rig_debug(RIG_DEBUG_VERBOSE, __FUNCTION__
		"\t:E: lo = %i, i = %i, hi = %i\n", low_idx, i, hi_idx);

	if(ts2k_ctcss_list[hi_idx] == tone)
		return hi_idx+1;
	
	if(low_idx < hi_idx) {
		// midpoint between hi/low
		t = ts2k_ctcss_list[low_idx] +
			(ts2k_ctcss_list[hi_idx] - ts2k_ctcss_list[low_idx])/2;
		if(tone < t)
			i = low_idx;
		else
			i = hi_idx;
	}

	rig_debug(RIG_DEBUG_VERBOSE, __FUNCTION__
		"\t: Done: lo = %i, i = %i, hi = %i\n", low_idx, i, hi_idx);

	return i+1;	// only the index is returned
}

int ts2k_uniq_shift2idx(rptr_shift_t shift)
{
	int i;

	switch(shift) {

	case RIG_RPT_SHIFT_NONE:	i = 0; break;
	case RIG_RPT_SHIFT_PLUS:	i = 1; break;
	case RIG_RPT_SHIFT_MINUS:	i = 2; break;
	case RIG_RPT_SHIFT_1750:	i = 3; break;

	default:
		rig_debug(RIG_DEBUG_WARN, __FUNCTION__
			": Invalid Repeater Shift.\n");
		return 0;
	}

	return i;
}

/****************************************
	Emulation Routines
 ****************************************/

/*
 * Emulate direct access to Call freqs
 *	Side effects: original modes destroyed
 *
 *	status:	working	
 */
int ts2k_emul_SetCallFreq(RIG *rig, vfo_t vfo, freq_t freq)
{
	int retval;
	vfo_t vtmp, vorig, vorig2;
	freq_t ftmp;

	// Save current VFO
	retval = rig_get_vfo(rig, &vorig);
	CHKERR(retval);

	// Switch to correct Transceiver
	vtmp = (vfo & (RIG_CTRL_MAIN | RIG_CTRL_SUB));
	retval = ts2k_uniq_SetTransceiver(rig, vtmp);

	// Save current VFO in case its a different transceiver 
	retval = rig_get_vfo(rig, &vorig2);
	CHKERR(retval);

	vtmp = vfo & (RIG_VFO_A | RIG_VFO_C);

	// Set either VFOA or VFOC
	retval = rig_set_vfo(rig, vtmp);
	CHKERR(retval);

	// Save current frequency in PLL
	retval = ts2k_get_freq(rig, vtmp, &ftmp);
	CHKERR(retval);
	retval = ts2k_set_freq(rig, vtmp, freq); 
	CHKERR(retval);

	// Now, actually write to CALL_n
	retval = ts2k_s_ci(rig, NULL);	// Non-standard.
	CHKERR(retval);

	// restore orig freq to PLL
	retval = ts2k_set_freq(rig, vtmp, ftmp);
	CHKERR(retval);

	// restore this transceiver vfo
	retval = rig_set_vfo(rig, vorig2);
	CHKERR(retval);

	// restore starting transceiver vfo
	retval = rig_set_vfo(rig, vorig);
	CHKERR(retval);

	return RIG_OK;
}

// End