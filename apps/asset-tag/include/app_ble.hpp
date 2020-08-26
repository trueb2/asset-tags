#ifndef APP_INCLUDE_APP_BLE_HPP
#define APP_INCLUDE_APP_BLE_HPP

#include <app_log.hpp>
#include <app_smp.hpp>

#include <zephyr.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <bluetooth/services/bas.h>

#include <stdexcept>

namespace app_ble {
    static constexpr bt_data advertisement_data[] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
        BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x0f, 0x18),
        BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_SMP_DATA_BYTES)
    };

    static bt_le_adv_param adv_params[] = {
        BT_LE_ADV_PARAM_INIT(
            BT_LE_ADV_OPT_CONNECTABLE |  BT_LE_ADV_OPT_USE_NAME,
            BT_GAP_ADV_FAST_INT_MIN_2,
            BT_GAP_ADV_FAST_INT_MAX_2,
            NULL)
        };

    struct static_manager_t {
        static void bt_adv() {
            bt_le_adv_stop();
            const auto err = bt_le_adv_start(adv_params, advertisement_data, ARRAY_SIZE(advertisement_data), NULL, 0);
            if (err) {
                LOG_ERR("Advertising failed to start (err %d)", err);
                throw std::runtime_error("Failed to start advertising");
            }

            LOG_DBG("Advertising successfully started");
        }

        static void connected(bt_conn *conn, uint8_t err) {
            bt_le_adv_stop();

            if (err) {
                LOG_INF("Connection failed (err 0x%02x)", err);
                bt_adv();
            } else {
                LOG_INF("Connected");
            }
        }

        static void disconnected(bt_conn *conn, uint8_t reason) {
            LOG_INF("Disconnected (reason 0x%02x)", reason);
            bt_adv();
        }
    };

    static bt_conn_cb conn_callbacks = {
        .connected = static_manager_t::connected,
        .disconnected = static_manager_t::disconnected,
    };

    struct manager_t {
        manager_t() {
            int ret;

            // Register the built-in mcumgr command handlers before advertising */
            #ifdef CONFIG_MCUMGR_CMD_FS_MGMT
                ret = fs_mount(&littlefs_mnt);
                if (ret < 0) {
                    LOG_ERR("Error mounting littlefs: %d", ret);
                    throw std::runtime_error("Failed to mount littlefs");
                }

                fs_mgmt_register_group();
            #endif
            #ifdef CONFIG_MCUMGR_CMD_OS_MGMT
                os_mgmt_register_group();
            #endif
            #ifdef CONFIG_MCUMGR_CMD_IMG_MGMT
                img_mgmt_register_group();
            #endif
            #ifdef CONFIG_MCUMGR_CMD_STAT_MGMT
                stat_mgmt_register_group();
            #endif

            // Prepare kernel structures
            ret = bt_enable(NULL);
            if(ret) {
                LOG_ERR("Failed to enable bluetooth: %d", ret);
                throw std::runtime_error("Failed to enable bluetooth");
            }
            LOG_DBG("Bluetooth initialized");
            
            // Register advertisement and callback configurations
            bt_conn_cb_register(&conn_callbacks);
            smp_bt_register();
            static_manager_t::bt_adv();
        }
    };
}


#endif