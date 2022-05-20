#ifndef AP_SQLITE_H
#define AP_SQLITE_H

/**
 * Callback function for sqlite3
 *
 * @see ap_sqlite_default_cb
 */
typedef int (*ap_sqlite_callback_func_t) (
        void *reserve, int argc, char **argv, char **column_name
);

int ap_sqlite_init();

int ap_sqlite_execute(const char *sql, ap_sqlite_callback_func_t cb);

int ap_sqlite_free();

#endif // AP_SQLITE_H