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
#include <sys/gld.h>

#include <inet/nd.h>
#include <inet/mi.h>

#include "sfxge.h"


typedef enum sfxge_prop_e {
	SFXGE_RX_COALESCE_MODE = 0,
	SFXGE_RX_SCALE_COUNT,
	SFXGE_FCNTL_RESPOND,
	SFXGE_FCNTL_GENERATE,
	SFXGE_INTR_MODERATION,
	SFXGE_ADV_AUTONEG,
	SFXGE_ADV_40GFDX,
	SFXGE_ADV_10GFDX,
	SFXGE_ADV_1000FDX,
	SFXGE_ADV_1000HDX,
	SFXGE_ADV_100FDX,
	SFXGE_ADV_100HDX,
	SFXGE_ADV_10FDX,
	SFXGE_ADV_10HDX,
	SFXGE_ADV_PAUSE,
	SFXGE_ADV_ASM_PAUSE,
	SFXGE_LP_AUTONEG,
	SFXGE_LP_40GFDX,
	SFXGE_LP_10GFDX,
	SFXGE_LP_1000FDX,
	SFXGE_LP_1000HDX,
	SFXGE_LP_100FDX,
	SFXGE_LP_100HDX,
	SFXGE_LP_10FDX,
	SFXGE_LP_10HDX,
	SFXGE_LP_PAUSE,
	SFXGE_LP_ASM_PAUSE,
	SFXGE_CAP_AUTONEG,
	SFXGE_CAP_40GFDX,
	SFXGE_CAP_10GFDX,
	SFXGE_CAP_1000FDX,
	SFXGE_CAP_1000HDX,
	SFXGE_CAP_100FDX,
	SFXGE_CAP_100HDX,
	SFXGE_CAP_10FDX,
	SFXGE_CAP_10HDX,
	SFXGE_CAP_PAUSE,
	SFXGE_CAP_ASM_PAUSE,
#if EFSYS_OPT_MCDI_LOGGING
	SFXGE_MCDI_LOGGING,
#endif
	SFXGE_NPROPS
} sfxge_prop_t;


static int
sfxge_gld_nd_get(sfxge_t *sp, unsigned int id, uint32_t *valp)
{
	efx_nic_t *enp = sp->s_enp;
	int rc;

	if (sp->s_mac.sm_state != SFXGE_MAC_STARTED) {
		rc = ENODEV;
		goto fail1;
	}

	ASSERT3U(id, <, SFXGE_NPROPS);

	switch (id) {
	case SFXGE_RX_COALESCE_MODE: {
		sfxge_rx_coalesce_mode_t mode;

		sfxge_rx_coalesce_mode_get(sp, &mode);

		*valp = mode;
		break;
	}
	case SFXGE_RX_SCALE_COUNT: {
		unsigned int count;

		if (sfxge_rx_scale_count_get(sp, &count) != 0)
			count = 0;

		*valp = count;
		break;
	}
	case SFXGE_FCNTL_RESPOND: {
		unsigned int fcntl;

		sfxge_mac_fcntl_get(sp, &fcntl);

		*valp = (fcntl & EFX_FCNTL_RESPOND) ? 1 : 0;
		break;
	}
	case SFXGE_FCNTL_GENERATE: {
		unsigned int fcntl;

		sfxge_mac_fcntl_get(sp, &fcntl);

		*valp = (fcntl & EFX_FCNTL_GENERATE) ? 1 : 0;
		break;
	}
	case SFXGE_INTR_MODERATION: {
		unsigned int us;

		sfxge_ev_moderation_get(sp, &us);

		*valp = (long)us;
		break;
	}
	case SFXGE_ADV_AUTONEG: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_AN)) ? 1 : 0;
		break;
	}
	case SFXGE_ADV_40GFDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_40000FDX)) ? 1 : 0;
		break;
	}
	case SFXGE_ADV_10GFDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_10000FDX)) ? 1 : 0;
		break;
	}
	case SFXGE_ADV_1000FDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_1000FDX)) ? 1 : 0;
		break;
	}
	case SFXGE_ADV_1000HDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_1000HDX)) ? 1 : 0;
		break;
	}
	case SFXGE_ADV_100FDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_100FDX)) ? 1 : 0;
		break;
	}
	case SFXGE_ADV_100HDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_100HDX)) ? 1 : 0;
		break;
	}
	case SFXGE_ADV_10FDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_10FDX)) ? 1 : 0;
		break;
	}
	case SFXGE_ADV_10HDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_10HDX)) ? 1 : 0;
		break;
	}
	case SFXGE_ADV_PAUSE: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_PAUSE)) ? 1 : 0;
		break;
	}
	case SFXGE_ADV_ASM_PAUSE: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_ASYM)) ? 1 : 0;
		break;
	}
	case SFXGE_LP_AUTONEG: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_AN)) ? 1 : 0;
		break;
	}
	case SFXGE_LP_40GFDX: {
		uint32_t mask;

		efx_phy_lp_cap_get(enp, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_40000FDX)) ? 1 : 0;
		break;
	}
	case SFXGE_LP_10GFDX: {
		uint32_t mask;

		efx_phy_lp_cap_get(enp, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_10000FDX)) ? 1 : 0;
		break;
	}
	case SFXGE_LP_1000FDX: {
		uint32_t mask;

		efx_phy_lp_cap_get(enp, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_1000FDX)) ? 1 : 0;
		break;
	}
	case SFXGE_LP_1000HDX: {
		uint32_t mask;

		efx_phy_lp_cap_get(enp, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_1000HDX)) ? 1 : 0;
		break;
	}
	case SFXGE_LP_100FDX: {
		uint32_t mask;

		efx_phy_lp_cap_get(enp, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_100FDX)) ? 1 : 0;
		break;
	}
	case SFXGE_LP_100HDX: {
		uint32_t mask;

		efx_phy_lp_cap_get(enp, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_100HDX)) ? 1 : 0;
		break;
	}
	case SFXGE_LP_10FDX: {
		uint32_t mask;

		efx_phy_lp_cap_get(enp, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_10FDX)) ? 1 : 0;
		break;
	}
	case SFXGE_LP_10HDX: {
		uint32_t mask;

		efx_phy_lp_cap_get(enp, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_10HDX)) ? 1 : 0;
		break;
	}
	case SFXGE_LP_PAUSE: {
		uint32_t mask;

		efx_phy_lp_cap_get(enp, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_PAUSE)) ? 1 : 0;
		break;
	}
	case SFXGE_LP_ASM_PAUSE: {
		uint32_t mask;

		efx_phy_lp_cap_get(enp, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_ASYM)) ? 1 : 0;
		break;
	}
	case SFXGE_CAP_AUTONEG: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_DEFAULT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_AN)) ? 1 : 0;
		break;
	}
	case SFXGE_CAP_40GFDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_DEFAULT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_40000FDX)) ? 1 : 0;
		break;
	}
	case SFXGE_CAP_10GFDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_DEFAULT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_10000FDX)) ? 1 : 0;
		break;
	}
	case SFXGE_CAP_1000FDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_DEFAULT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_1000FDX)) ? 1 : 0;
		break;
	}
	case SFXGE_CAP_1000HDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_DEFAULT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_1000HDX)) ? 1 : 0;
		break;
	}
	case SFXGE_CAP_100FDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_DEFAULT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_100FDX)) ? 1 : 0;
		break;
	}
	case SFXGE_CAP_100HDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_DEFAULT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_100HDX)) ? 1 : 0;
		break;
	}
	case SFXGE_CAP_10FDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_DEFAULT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_10FDX)) ? 1 : 0;
		break;
	}
	case SFXGE_CAP_10HDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_DEFAULT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_10HDX)) ? 1 : 0;
		break;
	}
	case SFXGE_CAP_PAUSE: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_DEFAULT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_PAUSE)) ? 1 : 0;
		break;
	}
	case SFXGE_CAP_ASM_PAUSE: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_DEFAULT, &mask);

		*valp = (mask & (1 << EFX_PHY_CAP_ASYM)) ? 1 : 0;
		break;
	}
#if EFSYS_OPT_MCDI_LOGGING
	case SFXGE_MCDI_LOGGING: {
		*valp = sp->s_mcdi_logging;
		break;
	}
#endif
	default:
		ASSERT(B_FALSE);
		break;
	}

	return (0);
fail1:
	DTRACE_PROBE1(fail1, int, rc);

	return (rc);
}

#ifdef _USE_NDD_PROPS
static int
sfxge_gld_nd_get_ioctl(queue_t *q, mblk_t *mp, caddr_t arg, cred_t *credp)
{
	sfxge_ndd_param_t *snpp = (sfxge_ndd_param_t *)arg;
	sfxge_t *sp = snpp->snp_sp;
	unsigned int id = snpp->snp_id;
	uint32_t val;
	int rc;

	_NOTE(ARGUNUSED(q, credp))

	if ((rc = sfxge_gld_nd_get(sp, id, &val)) != 0)
		goto fail1;

	(void) mi_mpprintf(mp, "%d", val);

	return (0);

fail1:
	DTRACE_PROBE1(fail1, int, rc);

	return (rc);
}
#else
static int
sfxge_gld_nd_get_ioctl(queue_t *q, mblk_t *mp, caddr_t arg, cred_t *credp)
{
	ASSERT(B_FALSE);
	return (-ENODEV);
}
#endif


static int
sfxge_gld_nd_set(sfxge_t *sp, unsigned int id, uint32_t val)
{
	efx_nic_t *enp = sp->s_enp;
	int rc;

	ASSERT3U(id, <, SFXGE_NPROPS);

	switch (id) {
	case SFXGE_RX_COALESCE_MODE: {
		sfxge_rx_coalesce_mode_t mode =
		    (sfxge_rx_coalesce_mode_t)val;

		if ((rc = sfxge_rx_coalesce_mode_set(sp, mode)) != 0)
			goto fail1;

		break;
	}
	case SFXGE_RX_SCALE_COUNT: {
		unsigned int count = (unsigned int)val;

		if ((rc = sfxge_rx_scale_count_set(sp, count)) != 0)
			goto fail1;

		break;
	}
	case SFXGE_FCNTL_RESPOND: {
		unsigned int fcntl;

		sfxge_mac_fcntl_get(sp, &fcntl);

		if (val != 0)
			fcntl |= EFX_FCNTL_RESPOND;
		else
			fcntl &= ~EFX_FCNTL_RESPOND;

		if ((rc = sfxge_mac_fcntl_set(sp, fcntl)) != 0)
			goto fail1;

		break;
	}
	case SFXGE_FCNTL_GENERATE: {
		unsigned int fcntl;

		sfxge_mac_fcntl_get(sp, &fcntl);

		if (val != 0)
			fcntl |= EFX_FCNTL_GENERATE;
		else
			fcntl &= ~EFX_FCNTL_GENERATE;

		if ((rc = sfxge_mac_fcntl_set(sp, fcntl)) != 0)
			goto fail1;

		break;
	}
	case SFXGE_INTR_MODERATION: {
		unsigned int us = (unsigned int)val;

		if ((rc = sfxge_ev_moderation_set(sp, us)) != 0)
			goto fail1;
		break;
	}
	case SFXGE_ADV_AUTONEG: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		if (val != 0)
			mask |= (1 << EFX_PHY_CAP_AN);
		else
			mask &= ~(1 << EFX_PHY_CAP_AN);

		if ((rc = efx_phy_adv_cap_set(enp, mask)) != 0)
			goto fail1;

		break;
	}
	case SFXGE_ADV_40GFDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		if (val != 0)
			mask |= (1 << EFX_PHY_CAP_40000FDX);
		else
			mask &= ~(1 << EFX_PHY_CAP_40000FDX);

		if ((rc = efx_phy_adv_cap_set(enp, mask)) != 0)
			goto fail1;

		break;
	}
	case SFXGE_ADV_10GFDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		if (val != 0)
			mask |= (1 << EFX_PHY_CAP_10000FDX);
		else
			mask &= ~(1 << EFX_PHY_CAP_10000FDX);

		if ((rc = efx_phy_adv_cap_set(enp, mask)) != 0)
			goto fail1;

		break;
	}
	case SFXGE_ADV_1000FDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		if (val != 0)
			mask |= (1 << EFX_PHY_CAP_1000FDX);
		else
			mask &= ~(1 << EFX_PHY_CAP_1000FDX);

		if ((rc = efx_phy_adv_cap_set(enp, mask)) != 0)
			goto fail1;

		break;
	}
	case SFXGE_ADV_1000HDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		if (val != 0)
			mask |= (1 << EFX_PHY_CAP_1000HDX);
		else
			mask &= ~(1 << EFX_PHY_CAP_1000HDX);

		if ((rc = efx_phy_adv_cap_set(enp, mask)) != 0)
			goto fail1;

		break;
	}
	case SFXGE_ADV_100FDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		if (val != 0)
			mask |= (1 << EFX_PHY_CAP_100FDX);
		else
			mask &= ~(1 << EFX_PHY_CAP_100FDX);

		if ((rc = efx_phy_adv_cap_set(enp, mask)) != 0)
			goto fail1;

		break;
	}
	case SFXGE_ADV_100HDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		if (val != 0)
			mask |= (1 << EFX_PHY_CAP_100HDX);
		else
			mask &= ~(1 << EFX_PHY_CAP_100HDX);

		if ((rc = efx_phy_adv_cap_set(enp, mask)) != 0)
			goto fail1;

		break;
	}
	case SFXGE_ADV_10FDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		if (val != 0)
			mask |= (1 << EFX_PHY_CAP_10FDX);
		else
			mask &= ~(1 << EFX_PHY_CAP_10FDX);

		if ((rc = efx_phy_adv_cap_set(enp, mask)) != 0)
			goto fail1;

		break;
	}
	case SFXGE_ADV_10HDX: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		if (val != 0)
			mask |= (1 << EFX_PHY_CAP_10HDX);
		else
			mask &= ~(1 << EFX_PHY_CAP_10HDX);

		if ((rc = efx_phy_adv_cap_set(enp, mask)) != 0)
			goto fail1;

		break;
	}
	case SFXGE_ADV_PAUSE: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		if (val != 0)
			mask |= (1 << EFX_PHY_CAP_PAUSE);
		else
			mask &= ~(1 << EFX_PHY_CAP_PAUSE);

		if ((rc = efx_phy_adv_cap_set(enp, mask)) != 0)
			goto fail1;

		break;
	}
	case SFXGE_ADV_ASM_PAUSE: {
		uint32_t mask;

		efx_phy_adv_cap_get(enp, EFX_PHY_CAP_CURRENT, &mask);

		if (val != 0)
			mask |= (1 << EFX_PHY_CAP_ASYM);
		else
			mask &= ~(1 << EFX_PHY_CAP_ASYM);

		if ((rc = efx_phy_adv_cap_set(enp, mask)) != 0)
			goto fail1;

		break;
	}
#if EFSYS_OPT_MCDI_LOGGING
	case SFXGE_MCDI_LOGGING: {
		sp->s_mcdi_logging = val;
		break;
	}
#endif
	/* Ignore other kstat writes. Might be for the link partner */
	default:
		DTRACE_PROBE1(ignore_kstat_write, int, id);
	}

	return (0);

fail1:
	DTRACE_PROBE1(fail1, int, rc);

	return (rc);
}


#ifdef _USE_NDD_PROPS
static int
sfxge_gld_nd_set_ioctl(queue_t *q, mblk_t *mp, char *valp, caddr_t arg,
    cred_t *credp)
{
	sfxge_ndd_param_t *snpp = (sfxge_ndd_param_t *)arg;
	sfxge_t *sp = snpp->snp_sp;
	unsigned int id = snpp->snp_id;
	long val;
	int rc;

	_NOTE(ARGUNUSED(q, mp, credp))

	(void) ddi_strtol(valp, (char **)NULL, 0, &val);

	if ((rc = sfxge_gld_nd_set(sp, id, (uint32_t)val)) != 0)
		goto fail1;

	return (0);

fail1:
	DTRACE_PROBE1(fail1, int, rc);

	return (rc);
}
#else
static int
sfxge_gld_nd_set_ioctl(queue_t *q, mblk_t *mp, char *valp, caddr_t arg,
    cred_t *credp)
{
	ASSERT(B_FALSE);
	return (-ENODEV);
}
#endif


static int
sfxge_gld_nd_update(kstat_t *ksp, int rw)
{
	sfxge_t *sp = ksp->ks_private;
	unsigned int id;
	int rc = 0;

	for (id = 0; id < SFXGE_NPROPS; id++) {
		kstat_named_t *knp = &(sp->s_nd_stat[id]);

		if (rw == KSTAT_READ)
			rc = sfxge_gld_nd_get(sp, id, &(knp->value.ui32));
		else if (rw == KSTAT_WRITE)
			rc = sfxge_gld_nd_set(sp, id, knp->value.ui32);
		else
			rc = EACCES;

		if (rc != 0)
			goto fail1;
	}

	return (0);

fail1:
	DTRACE_PROBE1(fail1, int, rc);
	return (rc);
}


static sfxge_ndd_param_t	sfxge_ndd_param[] = {
	{
		NULL,
		SFXGE_RX_COALESCE_MODE,
		"rx_coalesce_mode",
		sfxge_gld_nd_get_ioctl,
		sfxge_gld_nd_set_ioctl
	},
	{
		NULL,
		SFXGE_RX_SCALE_COUNT,
		"rx_scale_count",
		sfxge_gld_nd_get_ioctl,
		sfxge_gld_nd_set_ioctl
	},
	{
		NULL,
		SFXGE_FCNTL_RESPOND,
		"fcntl_respond",
		sfxge_gld_nd_get_ioctl,
		sfxge_gld_nd_set_ioctl
	},
	{
		NULL,
		SFXGE_FCNTL_GENERATE,
		"fcntl_generate",
		sfxge_gld_nd_get_ioctl,
		sfxge_gld_nd_set_ioctl
	},
	{
		NULL,
		SFXGE_INTR_MODERATION,
		"intr_moderation",
		sfxge_gld_nd_get_ioctl,
		sfxge_gld_nd_set_ioctl
	},
	{
		NULL,
		SFXGE_ADV_AUTONEG,
		"adv_cap_autoneg",
		sfxge_gld_nd_get_ioctl,
		sfxge_gld_nd_set_ioctl
	},
	{
		NULL,
		SFXGE_ADV_40GFDX,
		"adv_cap_40gfdx",
		sfxge_gld_nd_get_ioctl,
		sfxge_gld_nd_set_ioctl
	},
	{
		NULL,
		SFXGE_ADV_10GFDX,
		"adv_cap_10gfdx",
		sfxge_gld_nd_get_ioctl,
		sfxge_gld_nd_set_ioctl
	},
	{
		NULL,
		SFXGE_ADV_1000FDX,
		"adv_cap_1000fdx",
		sfxge_gld_nd_get_ioctl,
		sfxge_gld_nd_set_ioctl
	},
	{
		NULL,
		SFXGE_ADV_1000HDX,
		"adv_cap_1000hdx",
		sfxge_gld_nd_get_ioctl,
		sfxge_gld_nd_set_ioctl
	},
	{
		NULL,
		SFXGE_ADV_100FDX,
		"adv_cap_100fdx",
		sfxge_gld_nd_get_ioctl,
		sfxge_gld_nd_set_ioctl
	},
	{
		NULL,
		SFXGE_ADV_100HDX,
		"adv_cap_100hdx",
		sfxge_gld_nd_get_ioctl,
		sfxge_gld_nd_set_ioctl
	},
	{
		NULL,
		SFXGE_ADV_10FDX,
		"adv_cap_10fdx",
		sfxge_gld_nd_get_ioctl,
		sfxge_gld_nd_set_ioctl
	},
	{
		NULL,
		SFXGE_ADV_10HDX,
		"adv_cap_10hdx",
		sfxge_gld_nd_get_ioctl,
		sfxge_gld_nd_set_ioctl
	},
	{
		NULL,
		SFXGE_ADV_PAUSE,
		"adv_cap_pause",
		sfxge_gld_nd_get_ioctl,
		sfxge_gld_nd_set_ioctl
	},
	{
		NULL,
		SFXGE_ADV_ASM_PAUSE,
		"adv_cap_asm_pause",
		sfxge_gld_nd_get_ioctl,
		sfxge_gld_nd_set_ioctl
	},
	{
		NULL,
		SFXGE_LP_AUTONEG,
		"lp_cap_autoneg",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_LP_40GFDX,
		"lp_cap_40gfdx",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_LP_10GFDX,
		"lp_cap_10gfdx",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_LP_1000FDX,
		"lp_cap_1000fdx",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_LP_1000HDX,
		"lp_cap_1000hdx",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_LP_100FDX,
		"lp_cap_100fdx",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_LP_100HDX,
		"lp_cap_100hdx",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_LP_10FDX,
		"lp_cap_10fdx",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_LP_10HDX,
		"lp_cap_10hdx",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_LP_PAUSE,
		"lp_cap_pause",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_LP_ASM_PAUSE,
		"lp_cap_asm_pause",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_CAP_AUTONEG,
		"cap_autoneg",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_CAP_40GFDX,
		"cap_40gfdx",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_CAP_10GFDX,
		"cap_10gfdx",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_CAP_1000FDX,
		"cap_1000fdx",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_CAP_1000HDX,
		"cap_1000hdx",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_CAP_100FDX,
		"cap_100fdx",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_CAP_100HDX,
		"cap_100hdx",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_CAP_10FDX,
		"cap_10fdx",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_CAP_10HDX,
		"cap_10hdx",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_CAP_PAUSE,
		"cap_pause",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
	{
		NULL,
		SFXGE_CAP_ASM_PAUSE,
		"cap_asm_pause",
		sfxge_gld_nd_get_ioctl,
		NULL
	},
#if EFSYS_OPT_MCDI_LOGGING
	{
		NULL,
		SFXGE_MCDI_LOGGING,
		"mcdi_logging",
		sfxge_gld_nd_get_ioctl,
		sfxge_gld_nd_set_ioctl,
	}
#endif
};


int
sfxge_gld_nd_register(sfxge_t *sp)
{
#ifdef _USE_NDD_PROPS
	caddr_t *ndhp = &(sp->s_ndh);
#endif
	unsigned int id;
	char name[MAXNAMELEN];
	kstat_t *ksp;
	int rc;

	ASSERT3P(sp->s_ndh, ==, NULL);

#ifdef _USE_NDD_PROPS
	/* Register with the NDD framework */
	if ((sp->s_ndp = kmem_zalloc(sizeof (sfxge_ndd_param_t) * SFXGE_NPROPS,
	    KM_NOSLEEP)) == NULL) {
		rc = ENOMEM;
		goto fail1;
	}

	for (id = 0; id < SFXGE_NPROPS; id++) {
		sfxge_ndd_param_t *snpp = &(sp->s_ndp[id]);

		*snpp = sfxge_ndd_param[id];
		ASSERT3U(snpp->snp_id, ==, id);

		snpp->snp_sp = sp;

		(void) nd_load(ndhp, (char *)(snpp->snp_name),
		    snpp->snp_get, snpp->snp_set, (caddr_t)snpp);
	}
#endif

	/* Also create a kstat set */
	(void) snprintf(name, MAXNAMELEN - 1, "%s_ndd",
	    ddi_driver_name(sp->s_dip));

	if ((ksp = kstat_create((char *)ddi_driver_name(sp->s_dip),
	    ddi_get_instance(sp->s_dip), name, "ndd", KSTAT_TYPE_NAMED,
	    SFXGE_NPROPS, KSTAT_FLAG_WRITABLE)) == NULL) {
		rc = ENOMEM;
		goto fail2;
	}

	sp->s_nd_ksp = ksp;

	ksp->ks_update = sfxge_gld_nd_update;
	ksp->ks_private = sp;

	sp->s_nd_stat = ksp->ks_data;

	for (id = 0; id < SFXGE_NPROPS; id++) {
		kstat_named_t *knp = &(sp->s_nd_stat[id]);
		sfxge_ndd_param_t *snpp = &sfxge_ndd_param[id];

		kstat_named_init(knp, (char *)(snpp->snp_name),
		    KSTAT_DATA_UINT32);
	}

	kstat_install(ksp);

	return (0);

fail2:
	DTRACE_PROBE(fail2);

#ifdef _USE_NDD_PROPS
	nd_free(ndhp);
	sp->s_ndh = NULL;

	kmem_free(sp->s_ndp, sizeof (sfxge_ndd_param_t) * SFXGE_NPROPS);
	sp->s_ndp = NULL;

fail1:
	DTRACE_PROBE1(fail1, int, rc);
#endif

	return (rc);
}


void
sfxge_gld_nd_unregister(sfxge_t *sp)
{
#ifdef _USE_NDD_PROPS
	caddr_t *ndhp = &(sp->s_ndh);
#endif

	/* Destroy the kstat set */
	kstat_delete(sp->s_nd_ksp);
	sp->s_nd_ksp = NULL;
	sp->s_nd_stat = NULL;

	/* Unregister from the NDD framework */
#ifdef _USE_NDD_PROPS
	nd_free(ndhp);

	sp->s_ndh = NULL;

	kmem_free(sp->s_ndp, sizeof (sfxge_ndd_param_t) * SFXGE_NPROPS);
	sp->s_ndp = NULL;
#endif
}
