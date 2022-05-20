#include "ap_utils.h"
#include "ap_sqlite.h"

#if !AP_PLATFORM_ANDROID

#include <sqlite3.h>
static sqlite3 *database = NULL;

#endif

#ifndef AP_DEFAULT_DB_FILE
#define AP_DEFAULT_DB_FILE "ap_data.db"
#endif

static int ap_sqlite_default_cb(
        void *reserve, int argc, char **argv, char **col_name)
{
        LOGD("default callback from sqlite3:");
        for(int i = 0; i < argc; i++) {
                LOGD("\t%s: %s", col_name[i], argv[i] ? argv[i] : "NULL");
        }

        return 0;
}

int ap_sqlite_init()
{
#if !AP_PLATFORM_ANDROID
        if (database != NULL) {
                return 0;
        }

        int ret = 0;
        ret = sqlite3_open(AP_DEFAULT_DB_FILE, &database);
        if (ret) {
                LOGE("failed to init sqlite: %s", sqlite3_errmsg(database));
                sqlite3_close(database);
        }
#endif

        return 0;
}

int ap_sqlite_execute(const char *sql, ap_sqlite_callback_func_t cb)
{
#if !AP_PLATFORM_ANDROID
        if (database == NULL) {
                return AP_ERROR_INIT_FAILED;
        }

        int rc = 0;

        char *err_msg = NULL;
        if (cb == NULL) {
                cb = ap_sqlite_default_cb;
        }

        rc = sqlite3_exec(database, sql, cb, 0, &err_msg);
        if(rc != SQLITE_OK) {
                LOGE("SQL error: %s", err_msg);
                sqlite3_free(err_msg);
        }
#endif

        return 0;
}

int ap_sqlite_free()
{
#if !AP_PLATFORM_ANDROID
        if (database) {
                sqlite3_close(database);
        }
#endif

        return 0;
}
