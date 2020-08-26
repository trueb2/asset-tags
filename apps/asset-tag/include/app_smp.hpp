#ifndef APP_INCLUDE_APP_SMP_HPP
#define APP_INCLUDE_APP_SMP_HPP

#include <zephyr.h>
#include <string.h>
#include <stdlib.h>
#include <stats/stats.h>
#include <mgmt/buf.h>

#ifdef CONFIG_MCUMGR_CMD_FS_MGMT
#include <device.h>
#include <fs/fs.h>
#include "fs_mgmt/fs_mgmt.h"
#include <fs/littlefs.h>
#endif
#ifdef CONFIG_MCUMGR_CMD_OS_MGMT
#include "os_mgmt/os_mgmt.h"
#endif
#ifdef CONFIG_MCUMGR_CMD_IMG_MGMT
#include "img_mgmt/img_mgmt.h"
#endif
#ifdef CONFIG_MCUMGR_CMD_STAT_MGMT
#include "stat_mgmt/stat_mgmt.h"
#endif

#ifdef CONFIG_MCUMGR_SMP_BT
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/gatt.h>
#include <mgmt/smp_bt.h>
#endif

#ifdef CONFIG_MCUMGR_CMD_FS_MGMT
FS_LITTLEFS_DECLARE_DEFAULT_CONFIG(cstorage);
static fs_mount_t littlefs_mnt = {
	.type = FS_LITTLEFS,
	.mnt_point = "/lfs",
	.fs_data = &cstorage,
	.storage_dev = (void *)FLASH_AREA_ID(storage),
};
#endif

#ifdef CONFIG_MCUMGR_SMP_BT
#define BT_UUID_SMP_DATA_BYTES 0x84, 0xaa, 0x60, 0x74, 0x52, 0x8a, 0x8b, 0x86, 0xd3, 0x4c, 0xb7, 0x1d, 0x1d, 0xdc, 0x53, 0x8d
#endif



#endif