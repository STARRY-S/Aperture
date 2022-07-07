/**
 * @file ap_sqlite.h
 * @author Starry Wang (hxstarrys@gmail.com)
 * @brief SQLite functions definition
 */
#ifndef AP_SQLITE_H
#define AP_SQLITE_H

/**
 * @brief Callback function type definition for sqlite3
 *
 * @see ap_sqlite_default_cb
 */
typedef int (*ap_sqlite_callback_func_t) (
        void *reserve, int argc, char **argv, char **column_name
);

/**
 * @brief Initialize SQLite3
 *
 * @return int
 */
int ap_sqlite_init();

/**
 * @brief Execute SQL
 *
 * @param sql
 * @param cb
 * @return int
 */
int ap_sqlite_execute(const char *sql, ap_sqlite_callback_func_t cb);

/**
 * @brief Release SQLite resources
 *
 * @return int
 */
int ap_sqlite_free_all();

#endif // AP_SQLITE_H