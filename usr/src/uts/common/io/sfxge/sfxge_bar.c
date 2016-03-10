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

#include <sys/types.h>
#include <sys/ddi.h>
#include <sys/sunddi.h>
#include <sys/stream.h>
#include <sys/dlpi.h>

#include "sfxge.h"

int
sfxge_bar_init(sfxge_t *sp)
{
	efsys_bar_t *esbp = &(sp->s_bar);
	ddi_device_acc_attr_t devacc;
	int rc;

	devacc.devacc_attr_version = DDI_DEVICE_ATTR_V0;
	devacc.devacc_attr_endian_flags = DDI_NEVERSWAP_ACC;
	devacc.devacc_attr_dataorder = DDI_STRICTORDER_ACC;

	if (ddi_regs_map_setup(sp->s_dip, EFX_MEM_BAR, &(esbp->esb_base), 0, 0,
	    &devacc, &(esbp->esb_handle)) != DDI_SUCCESS) {
		rc = ENODEV;
		goto fail1;
	}

	mutex_init(&(esbp->esb_lock), NULL, MUTEX_DRIVER, NULL);

	return (0);

fail1:
	DTRACE_PROBE1(fail1, int, rc);

	return (rc);
}

int
sfxge_bar_ioctl(sfxge_t *sp, sfxge_bar_ioc_t *sbip)
{
	efsys_bar_t *esbp = &(sp->s_bar);
	efx_oword_t oword;
	int rc;

	if (!IS_P2ALIGNED(sbip->sbi_addr, sizeof (efx_oword_t))) {
		rc = EINVAL;
		goto fail1;
	}

	switch (sbip->sbi_op) {
	case SFXGE_BAR_OP_READ:
		EFSYS_BAR_READO(esbp, sbip->sbi_addr, &oword, B_TRUE);

		sbip->sbi_data[0] = EFX_OWORD_FIELD(oword, EFX_DWORD_0);
		sbip->sbi_data[1] = EFX_OWORD_FIELD(oword, EFX_DWORD_1);
		sbip->sbi_data[2] = EFX_OWORD_FIELD(oword, EFX_DWORD_2);
		sbip->sbi_data[3] = EFX_OWORD_FIELD(oword, EFX_DWORD_3);

		break;

	case SFXGE_BAR_OP_WRITE:
		EFX_POPULATE_OWORD_4(oword,
		    EFX_DWORD_0, sbip->sbi_data[0],
		    EFX_DWORD_1, sbip->sbi_data[1],
		    EFX_DWORD_2, sbip->sbi_data[2],
		    EFX_DWORD_3, sbip->sbi_data[3]);

		EFSYS_BAR_WRITEO(esbp, sbip->sbi_addr, &oword, B_TRUE);
		break;

	default:
		rc = ENOTSUP;
		goto fail2;
	}

	return (0);

fail2:
	DTRACE_PROBE(fail2);
fail1:
	DTRACE_PROBE1(fail1, int, rc);

	return (rc);
}

void
sfxge_bar_fini(sfxge_t *sp)
{
	efsys_bar_t *esbp = &(sp->s_bar);

	ddi_regs_map_free(&(esbp->esb_handle));

	mutex_destroy(&(esbp->esb_lock));

	esbp->esb_base = NULL;
	esbp->esb_handle = NULL;
}
