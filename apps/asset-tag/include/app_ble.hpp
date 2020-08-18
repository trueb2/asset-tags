#ifndef APP_INCLUDE_APP_BLE_HPP
#define APP_INCLUDE_APP_BLE_HPP

#include <app_log.hpp>

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
    };

    static constexpr const auto adv_conn_name = BT_LE_ADV_CONN_NAME;

    struct static_manager_t {
        static void bt_adv() {
            bt_le_adv_stop();
            const auto err = bt_le_adv_start(adv_conn_name, advertisement_data, ARRAY_SIZE(advertisement_data), NULL, 0);
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

        static void auth_cancel(bt_conn *conn) {
            char addr[BT_ADDR_LE_STR_LEN];

            bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

            LOG_INF("Pairing cancelled: %s", addr);
        }    
    };

    static bt_conn_cb conn_callbacks = {
        .connected = static_manager_t::connected,
        .disconnected = static_manager_t::disconnected,
    };

    static bt_conn_auth_cb auth_callbacks = {
        .cancel = static_manager_t::auth_cancel,
    };

    struct manager_t {
        manager_t() {
            // Prepare kernel structures
            int ret = bt_enable(NULL);
            if(ret) {
                LOG_ERR("Failed to enable bluetooth: %d", ret);
                throw std::runtime_error("Failed to enable bluetooth");
            }
            LOG_DBG("Bluetooth initialized");
            
            // Register advertisement and callback configurations
            bt_conn_cb_register(&conn_callbacks);
            bt_conn_auth_cb_register(&auth_callbacks);
            static_manager_t::bt_adv();
        }
    };
}


#endif