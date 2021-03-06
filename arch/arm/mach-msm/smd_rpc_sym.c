/*                                            */



#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/module.h>

struct sym {
	const char *str;
};

const char *smd_rpc_syms[] = {
	"CB CM_FUSION",				/*          */
	"CB DB",				/*          */
	"CB SND",				/*          */
	"CB WMS_FUSION",			/*          */
	"CB PDSM",				/*          */
	"CB MISC_MODEM_APIS",			/*          */
	"CB MISC_APPS_APIS",			/*          */
	"CB JOYST",				/*          */
	"CB UNDEFINED",
	"CB UNDEFINED",
	"CB ADSPRTOSATOM",			/*          */
	"CB ADSPRTOSMTOA",			/*          */
	"CB I2C",				/*          */
	"CB TIME_REMOTE",			/*          */
	"CB NV_FUSION",				/*          */
	"CB CLKRGM_SEC_FUSION",			/*          */
	"CB RDEVMAP",				/*          */
	"CB UNDEFINED",
	"CB PBMLIB_FUSION",			/*          */
	"CB AUDMGR",				/*          */
	"CB MVS",				/*          */
	"CB DOG_KEEPALIVE",			/*          */
	"CB GSDI_EXP_FUSION",			/*          */
	"CB AUTH",				/*          */
	"CB NVRUIMI",				/*          */
	"CB MMGSDILIB_FUSION",			/*          */
	"CB CHARGER",				/*          */
	"CB UIM_FUSION",			/*          */
	"CB UNDEFINED",
	"CB PDSM_ATL",				/*          */
	"CB FS_XMOUNT",				/*          */
	"CB SECUTIL",				/*          */
	"CB MCCMEID",				/*          */
	"CB PM_STROBE_FLASH",			/*          */
	"CB UNDEFINED",
	"CB SMD_BRIDGE",			/*          */
	"CB SMD_PORT_MGR_FUSION",		/*          */
	"CB BUS_PERF",				/*          */
	"CB BUS_MON_REMOTE",			/*          */
	"CB MC",				/*          */
	"CB MCCAP",				/*          */
	"CB MCCDMA",				/*          */
	"CB MCCDS",				/*          */
	"CB MCCSCH",				/*          */
	"CB MCCSRID",				/*          */
	"CB SNM",				/*          */
	"CB MCCSYOBJ",				/*          */
	"CB UNDEFINED",
	"CB UNDEFINED",
	"CB DSRLP_APIS",			/*          */
	"CB RLP_APIS",				/*          */
	"CB DS_MP_SHIM_MODEM",			/*          */
	"CB UNDEFINED",
	"CB DSHDR_MDM_APIS",			/*          */
	"CB DS_MP_SHIM_APPS",			/*          */
	"CB HDRMC_APIS",			/*          */
	"CB UNDEFINED",
	"CB UNDEFINED",
	"CB PMAPP_OTG",				/*          */
	"CB DIAG",				/*          */
	"CB GSTK_EXP_FUSION",			/*          */
	"CB DSBC_MDM_APIS",			/*          */
	"CB HDRMRLP_MDM_APIS",			/*          */
	"CB UNDEFINED",
	"CB HDRMC_MRLP_APIS",			/*          */
	"CB PDCOMM_APP_API",			/*          */
	"CB DSAT_APIS",				/*          */
	"CB RFM",				/*          */
	"CB CMIPAPP",				/*          */
	"CB DSMP_UMTS_MODEM_APIS",		/*          */
	"CB UNDEFINED",
	"CB DSUCSDMPSHIM",			/*          */
	"CB TIME_REMOTE_ATOM",			/*          */
	"CB UNDEFINED",
	"CB SD",				/*          */
	"CB MMOC",				/*          */
	"CB UNDEFINED",
	"CB WLAN_CP_CM",			/*          */
	"CB FTM_WLAN",				/*          */
	"CB UNDEFINED",
	"CB CPRMINTERFACE",			/*          */
	"CB DATA_ON_MODEM_MTOA_APIS",		/*          */
	"CB UNDEFINED",
	"CB MISC_MODEM_APIS_NONWINMOB",		/*          */
	"CB MISC_APPS_APIS_NONWINMOB",		/*          */
	"CB PMEM_REMOTE",			/*          */
	"CB TCXOMGR",				/*          */
	"CB UNDEFINED",
	"CB BT",				/*          */
	"CB PD_COMMS_API",			/*          */
	"CB PD_COMMS_CLIENT_API",		/*          */
	"CB PDAPI",				/*          */
	"CB UNDEFINED",
	"CB TIME_REMOTE_MTOA",			/*          */
	"CB FTM_BT",				/*          */
	"CB DSUCSDAPPIF_APIS",			/*          */
	"CB PMAPP_GEN",				/*          */
	"CB PM_LIB_FUSION",			/*          */
	"CB UNDEFINED",
	"CB HSU_APP_APIS",			/*          */
	"CB HSU_MDM_APIS",			/*          */
	"CB ADIE_ADC_REMOTE_ATOM",		/*          */
	"CB TLMM_REMOTE_ATOM",			/*          */
	"CB UI_CALLCTRL",			/*          */
	"CB UIUTILS",				/*          */
	"CB PRL",				/*          */
	"CB HW",				/*          */
	"CB OEM_RAPI_FUSION",			/*          */
	"CB WMSPM",				/*          */
	"CB BTPF",				/*          */
	"CB UNDEFINED",
	"CB USB_APPS_RPC",			/*          */
	"CB USB_MODEM_RPC",			/*          */
	"CB ADC",				/*          */
	"CB CAMERAREMOTED",			/*          */
	"CB SECAPIREMOTED",			/*          */
	"CB DSATAPI",				/*          */
	"CB CLKCTL_RPC",			/*          */
	"CB BREWAPPCOORD",			/*          */
	"CB UNDEFINED",
	"CB WLAN_TRP_UTILS",			/*          */
	"CB GPIO_RPC",				/*          */
	"CB UNDEFINED",
	"CB UNDEFINED",
	"CB L1_DS",				/*          */
	"CB UNDEFINED",
	"CB UNDEFINED",
	"CB OSS_RRCASN_REMOTE",			/*          */
	"CB PMAPP_OTG_REMOTE",			/*          */
	"CB PING_LTE_RPC",			/*          */
	"CB UNDEFINED",
	"CB UNDEFINED",
	"CB UNDEFINED",
	"CB UNDEFINED",
	"CB UNDEFINED",
	"CB UKCC_IPC_APIS",			/*          */
	"CB UNDEFINED",
	"CB VBATT_REMOTE",			/*          */
	"CB MFPAL_FPS",				/*          */
	"CB DSUMTSPDPREG",			/*          */
	"CB LOC_API",				/*          */
	"CB UNDEFINED",
	"CB CMGAN",				/*          */
	"CB ISENSE",				/*          */
	"CB TIME_SECURE",			/*          */
	"CB HS_REM",				/*          */
	"CB ACDB",				/*          */
	"CB NET",				/*          */
	"CB LED",				/*          */
	"CB DSPAE",				/*          */
	"CB MFKAL",				/*          */
	"CB UNDEFINED",
	"CB UNDEFINED",
	"CB UNDEFINED",
	"CB UNDEFINED",
	"CB TEST_API",				/*          */
	"CB REMOTEFS_SRV_API_FUSION",		/*          */
	"CB ISI_TRANSPORT",			/*          */
	"CB OEM_FTM",				/*          */
	"CB TOUCH_SCREEN_ADC",			/*          */
	"CB SMD_BRIDGE_APPS_FUSION",		/*          */
	"CB SMD_BRIDGE_MODEM_FUSION",		/*          */
	"CB DOG_KEEPALIVE_MODEM",		/*          */
	"CB VOEM_IF",				/*          */
	"CB NPA_REMOTE",			/*          */
	"CB MMGSDISESSIONLIB_FUSION",		/*          */
	"CB IFTA_REMOTE",			/*          */
	"CB REMOTE_STORAGE",			/*          */
	"CB MF_REMOTE_FILE",			/*          */
	"CB MFSC_CHUNKED_TRANSPORT",		/*          */
	"CB MFIM3",				/*          */
	"CB FM_WAN_API",			/*          */
	"CB WLAN_RAPI",				/*          */
	"CB DSMGR_APIS",			/*          */
	"CB CM_MM_FUSION",			/*          */
};

static struct sym_tbl {
	const char **data;
	int size;
} tbl = { smd_rpc_syms, ARRAY_SIZE(smd_rpc_syms)};

const char *smd_rpc_get_sym(uint32_t val)
{
	int idx = val & 0xFFFF;
	if (idx < tbl.size) {
		if (val & 0x01000000)
			return tbl.data[idx];
		else
			return tbl.data[idx] + 3;
	}
	return 0;
}
EXPORT_SYMBOL(smd_rpc_get_sym);

