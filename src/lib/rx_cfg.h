#ifndef RX_CFG_H
#define RX_CFG_H 1

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <glib.h>
#include "debug.h"


#define CFG_VAL_TRUE  "true"
#define CFG_VAL_FALSE "false"

#define CFG_INT_FORMAT "%d"
#define CFG_FLOAT_FORMAT "%.2f"


/**
 * This enumeration defines all possible types of configuration items.
 */
typedef enum {
    CFG_TYPE_MUTEX = 1,
    CFG_TYPE_PARENT,
    CFG_TYPE_LOW = 100,
    CFG_TYPE_STR,
    CFG_TYPE_INT,
    CFG_TYPE_FLOAT,
    CFG_TYPE_BOOL
} rx_cfg_type;

/**
 * This enumeration defines all possible error codes for the
 * configuration routines.
 */
typedef enum {
    CFG_ERR_NONE = 0,
    CFG_ERR_ABSENT,
    CFG_ERR_CONVERT,
    CFG_ERR_TYPE
} rx_cfg_error;

/**
 * Structure used to specify a configurable option of a plug-in.
 */
struct rx_cfg_item_struct {

    gchar *name;                /**< Name of the item. */

    gchar *description;         /**< Short description of the item. */

    rx_cfg_type type;           /**< Type of the value. */
    union {

        gchar *strVal;          /**< String value of the item. */

        gint intVal;            /**< Integer value of the item. */

        gfloat floatVal;        /**< Floating point value of the item. */

        gboolean boolVal;       /**< Boolean value of the item. */

        void *complexVal;       /**< Pointer to a more complex item. */
    } val;
};

/**
 * One item of the configuration of a plug-in.
 */
typedef struct rx_cfg_item_struct rx_cfg_item;

/**
 * Complete configuration of a plug-in.  The hashtable will contain a
 * set of ::rx_cfg_item structures each of them referenced by their
 * name.
 */
typedef struct GHashTable rx_cfg;

/**
 * Create a new (empty) configuration structure.
 */
rx_cfg *rx_cfg_new(const rx_cfg * parent);

/**
 * Destroys the configuration and any items in it.  All resources will
 * be freed.
 */
void rx_cfg_free(rx_cfg * cfg);

/**
 * Remove a named item from the configuration.  The memory used by the
 * ::rx_cfg_item will be freed automatically.  If the item does not
 * exist, an error code will be returned.
 */
rx_cfg_error rx_cfg_remove(rx_cfg * cfg, const gchar * name);


// ===[ Setters ]===

/**
 * Set the parent configuration.
 */
rx_cfg_error rx_cfg_set_parent(rx_cfg * cfg, const rx_cfg * parent);

/**
 * Add string item to the configuration.
 */
void rx_cfg_add_str(rx_cfg * cfg, const gchar * name, const gchar * desc, const gchar * val);

/**
 * Add string item to the configuration.
 */
void rx_cfg_add_int(rx_cfg * cfg, const gchar * name, const gchar * desc, const gint val);

/**
 * Add string item to the configuration.
 */
void rx_cfg_add_bool(rx_cfg * cfg, const gchar * name, const gchar * desc, const gboolean val);

/**
 * Add float item to the configuration.
 */
void rx_cfg_add_float(rx_cfg * cfg, const gchar * name, const gchar * desc, const gfloat val);


// ===[ Getters ]===

/**
 * Get the parent configuration.
 */
rx_cfg *rx_cfg_get_parent(const rx_cfg * cfg);

/**
 * Returns an GPtrArray with the names of all the configuration items.
 * The list is terminated with a NULL pointer.  The caller should free
 * the array AND all the items in the array with a call to
 * ::g_ptr_array_free.
 */
GPtrArray *rx_cfg_get_names(const rx_cfg * cfg);

/**
 * Retrieves a string value from the configuration.  If the item is
 * not present or when it is not of type string, NULL is returned.
 */
gchar *rx_cfg_get_str(const rx_cfg * cfg, const gchar * name);

/**
 * Retrieves an integer value from the configuration.  If the item is
 * not present or when it is not of type integer, 0 is returned.
 */
gint rx_cfg_get_int(const rx_cfg * cfg, const gchar * name);

/**
 * Retrieves a boolean value from the configuration.  If the item is
 * not present or if it is not of type boolean, FALSE is returned.
 */
gboolean rx_cfg_get_bool(const rx_cfg * cfg, const gchar * name);

/**
 * Retrieves a float value from the configuration.  If the item is not
 * present or when it is not of type float, 0.0 is returned.
 */
gfloat rx_cfg_get_float(const rx_cfg * cfg, const gchar * name);

/**
 * Retrieves an item from the configuration and converts its value to
 * a string.  If the item is not present, NULL is returned.  The
 * caller should free the memory used by the string when it is no
 * longer needed.
 */
gchar *rx_cfg_get_to_str(const rx_cfg * cfg, const gchar * name);


// ===[ Mutators ]===

/**
 * Updates the value of the configuration item with the given name.
 * The string value is automatically converted to the correct type for
 * the configuration item.  If all goes well, 0 is returned.  Errors
 * can occur when the conifguration item does not exist or when the
 * string can not be converted into the correct type.
 */
rx_cfg_error rx_cfg_upd_str(rx_cfg * cfg, const gchar * name, const gchar * val);

/**
 * Updates the value of the configuration item with the given name.
 * If the configuration item is not present or when it is not of type
 * integer, an error is returned.
 */
rx_cfg_error rx_cfg_upd_int(rx_cfg * cfg, const gchar * name, const gint val);

/**
 * Updates the value of the configuration item with the given name.
 * If the configuration item is not present or when it is not of type
 * boolean, an error is returned.
 */
rx_cfg_error rx_cfg_upd_bool(rx_cfg * cfg, const gchar * name, const gboolean val);

/**
 * Updates the value of the configuration item with the given name.
 * If the configuration item is not present or when it is not of type
 * float, an error is returned.
 */
rx_cfg_error rx_cfg_upd_float(rx_cfg * cfg, const gchar * name, const gfloat val);


// ===[ Miscellaneous ]===

/**
 * Print all key/value pairs in the given configuration.  Uses dlog().
 */
void rx_cfg_print(const rx_cfg * cfg);

#endif
