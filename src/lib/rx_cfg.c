
#include <stdlib.h>
#include <glib.h>
#include "rx_cfg.h"

#define RX_CFG_PARENT "__RX_CFG_PARENT__"
#define RX_CFG_MUTEX  "__RX_CFG_MUTEX__"

// ===[ Forward declarations (private prototypes) ]===

/**
 * Locks the configuration (mutex).
 */
void rx_cfg_lock(const rx_cfg * cfg);

/**
 * Unlocks the configuration (mutex).
 */
void rx_cfg_unlock(const rx_cfg * cfg);

/**
 * Destroy a config item value, freeing any resources used by it.
 */
void rx_cfg_destroy_value(gpointer data);

/**
 * Remove one item from the configuration.  To be used with
 * ::rx_cfg_foreach.
 */
gboolean rx_cfg_remove_1(gpointer key, gpointer value, gpointer user_data);

/**
 * Create a configuration item without a value or type.
 */
rx_cfg_item *rx_cfg_create_no_val(const gchar * name, const gchar * desc);


// ===[ Construction/destruction ]===

rx_cfg *rx_cfg_new(const rx_cfg * parent)
{
    GHashTable *h;
    GStaticMutex *mutex;
    rx_cfg_item *item;

    dlog("rx_cfg_new: %s parent cfg given.\n", (parent ? "" : "no"));
    h = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, rx_cfg_destroy_value);
    // Create and add mutex.
    dlog("rx_cfg_new: adding mutex.\n");
    mutex = g_malloc(sizeof(GStaticMutex));
    g_static_mutex_init(mutex);
    item = rx_cfg_create_no_val(RX_CFG_MUTEX, NULL);
    item->type = CFG_TYPE_MUTEX;
    item->val.complexVal = (void *) mutex;
    g_hash_table_insert(h, RX_CFG_MUTEX, item);
    // Add parent if given.
    rx_cfg_set_parent((rx_cfg *) h, parent);
    return (rx_cfg *) h;
}

void rx_cfg_free(rx_cfg * cfg)
{
    GStaticMutex *mutex;

    dlog("rx_cfg_free.\n");
    rx_cfg_lock(cfg);
    mutex = g_hash_table_lookup((GHashTable *) cfg, RX_CFG_MUTEX);
    // Remove all items (automatically freeing them, except the mutex).
    g_hash_table_foreach_remove((GHashTable *) cfg, rx_cfg_remove_1, NULL);
    // Removethe mutex.
    g_hash_table_remove((GHashTable *) cfg, RX_CFG_MUTEX);
    // FIXME
    // Destroy it.
    //g_static_mutex_unlock(mutex);
    //g_static_mutex_free(mutex);
    //g_free(mutex);
    // Free the configuration structure.
    g_free(cfg);
}

gboolean rx_cfg_remove_1(gpointer key, gpointer value, gpointer user_data)
{
    rx_cfg_item *item;

    item = (rx_cfg_item *) value;
    if (item->type == CFG_TYPE_MUTEX) {
        return FALSE;           /* Don't remove mutex (yet). */
    } else {
        return TRUE;            /* Remove all. */
    }
}

void rx_cfg_destroy_value(gpointer data)
{
    rx_cfg_item *item;

    item = (rx_cfg_item *) data;
    dlog("rx_cfg_destroy_value: '%s'.\n", item->name);
    switch (item->type) {
        case CFG_TYPE_MUTEX:
            // no-op (the mutex itself will be destroyed by ::rx_cfg_free)
            break;
        case CFG_TYPE_PARENT:
            // no-op (we shouldn't destroy our parents)
            break;
        case CFG_TYPE_STR:
            g_free(item->val.strVal);
            break;
        default:
            // no free-ing necessary
    }
    g_free(item->name);
    g_free(item->description);
    g_free(item);
}

rx_cfg_error rx_cfg_remove(rx_cfg * cfg, const gchar * name)
{
    rx_cfg_item *item;
    rx_cfg_error err = CFG_ERR_NONE;

    dlog("rx_cfg_remove: '%s'.\n", name);
    rx_cfg_lock(cfg);           /* LOCK */
    item = (rx_cfg_item *) g_hash_table_lookup((GHashTable *) cfg, name);
    if (!item) {
        dlog("rx_cfg_remove: '%s' not present.\n", name);
        err = CFG_ERR_ABSENT;
    } else {
        g_hash_table_remove((GHashTable *) cfg, name);
    }
    rx_cfg_unlock(cfg);         /* UNLOCK */
    return err;
}

// ===[ Setters ]===

/**
 * Generic routine that adds a ::rx_cfg_item to a ::rx_cfg
 * structure
 */
void rx_cfg_add_item(rx_cfg * cfg, const gchar * name, rx_cfg_item * item)
{
    rx_cfg_remove(cfg, name);
    dlog("rx_cfg_add_item: adding '%s'.\n", name);
    rx_cfg_lock(cfg);           /* LOCK */
    g_hash_table_insert((GHashTable *) cfg, (gpointer) name, item);
    rx_cfg_unlock(cfg);         /* UNLOCK */
}

rx_cfg_item *rx_cfg_create_no_val(const gchar * name, const gchar * desc)
{
    rx_cfg_item *item;

    item = g_malloc(sizeof(rx_cfg_item));
    item->name = g_strdup(name);
    item->description = g_strdup(desc);
    return item;
}

rx_cfg_error rx_cfg_set_parent(rx_cfg * cfg, const rx_cfg * parent)
{
    dlog("rx_cfg_set_parent.\n");
    if (parent) {
        rx_cfg_item *item;

        item = rx_cfg_create_no_val(RX_CFG_PARENT, NULL);
        item->type = CFG_TYPE_PARENT;
        item->val.complexVal = (void *) parent;
        rx_cfg_add_item(cfg, RX_CFG_PARENT, item);
    }
    return CFG_ERR_NONE;
}

void rx_cfg_add_str(rx_cfg * cfg, const gchar * name, const gchar * desc, const gchar * val)
{
    rx_cfg_item *item;

    dlog("rx_cfg_add_str: '%s' -> '%s'.\n", name, val);
    item = rx_cfg_create_no_val(name, desc);
    item->type = CFG_TYPE_STR;
    item->val.strVal = g_strdup(val);
    rx_cfg_add_item(cfg, name, item);
}

void rx_cfg_add_int(rx_cfg * cfg, const gchar * name, const gchar * desc, const gint val)
{
    rx_cfg_item *item;

    dlog("rx_cfg_add_int: '%s' -> '%d'.\n", name, val);
    item = rx_cfg_create_no_val(name, desc);
    item->type = CFG_TYPE_INT;
    item->val.intVal = val;
    rx_cfg_add_item(cfg, name, item);
}

void rx_cfg_add_bool(rx_cfg * cfg, const gchar * name, const gchar * desc, const gboolean val)
{
    rx_cfg_item *item;

    dlog("rx_cfg_add_bool: '%s' -> '%d'.\n", name, val);
    item = rx_cfg_create_no_val(name, desc);
    item->type = CFG_TYPE_BOOL;
    item->val.boolVal = val;
    rx_cfg_add_item(cfg, name, item);
}

void rx_cfg_add_float(rx_cfg * cfg, const gchar * name, const gchar * desc, const gfloat val)
{
    rx_cfg_item *item;

    dlog("rx_cfg_add_float: '%s' -> '%f'.\n", name, val);
    item = rx_cfg_create_no_val(name, desc);
    item->type = CFG_TYPE_FLOAT;
    item->val.floatVal = val;
    rx_cfg_add_item(cfg, name, item);
}


// ===[ Getters ]===

rx_cfg *rx_cfg_get_parent(const rx_cfg * cfg)
{
    rx_cfg_item *item;
    rx_cfg *parent = NULL;

    dlog("rx_cfg_get_parent.\n");
    rx_cfg_lock(cfg);           /* LOCK */
    item = g_hash_table_lookup((GHashTable *) cfg, RX_CFG_PARENT);
    if (item) {
        parent = (rx_cfg *) item->val.complexVal;
    }
    rx_cfg_unlock(cfg);         /* UNLOCK */
    return parent;
}

void rx_cfg_get_names_1(gpointer key, gpointer value, gpointer user_data)
{
    rx_cfg_item *item;
    GPtrArray *names;

    item = (rx_cfg_item *) value;
    names = (GPtrArray *) user_data;
    if (item->type > CFG_TYPE_LOW) {
        g_ptr_array_add(names, g_strdup((gchar *) key));
    }
}

GPtrArray *rx_cfg_get_names(const rx_cfg * cfg)
{
    GPtrArray *names;

    dlog("rx_cfg_get_names.\n");
    names = g_ptr_array_new();
    g_hash_table_foreach((GHashTable *) cfg, rx_cfg_get_names_1, names);
    g_ptr_array_add(names, NULL);
    dlog("rx_cfg_get_names: %d items (terminating NULL incl.).\n", names->len);
    return names;
}

/**
 * Returns the configuration item with given name, or NULL if it does
 * not exist.
 */
rx_cfg_item *rx_cfg_get(const rx_cfg * cfg, const gchar * name)
{
    rx_cfg_item *item;

    dlog("rx_cfg_get: '%s'.\n", name);
    rx_cfg_lock(cfg);           /* LOCK */
    item = g_hash_table_lookup((GHashTable *) cfg, name);
    rx_cfg_unlock(cfg);         /* UNLOCK */
    if (!item) {
        rx_cfg *parent;

        parent = rx_cfg_get_parent(cfg);
        if (parent) {
            dlog("rx_cfg_get: checking parent config.\n");
            item = rx_cfg_get(parent, name);
        }
    }
    return item;
}

gchar *rx_cfg_get_str(const rx_cfg * cfg, const gchar * name)
{
    rx_cfg_item *item;

    dlog("rx_cfg_get_str: '%s'.\n", name);
    item = rx_cfg_get(cfg, name);
    if (!item) {
        dlog("rx_cfg_get_str: '%s' not present.\n", name);
        return NULL;
    }
    if (item->type != CFG_TYPE_STR) {
        dlog("rx_cfg_get_str: '%s' not string.\n", name);
        return NULL;
    }
    dlog("rx_cfg_get_str: '%s' -> '%s'.\n", name, item->val.strVal);
    return item->val.strVal;
}

gint rx_cfg_get_int(const rx_cfg * cfg, const gchar * name)
{
    rx_cfg_item *item;

    dlog("rx_cfg_get_int: '%s'.\n", name);
    item = rx_cfg_get(cfg, name);
    if (!item) {
        dlog("rx_cfg_get_int: '%s' not present.\n", name);
        return 0;
    }
    if (item->type != CFG_TYPE_INT) {
        dlog("rx_cfg_get_int: '%s' not integer.\n", name);
        return 0;
    }
    dlog("rx_cfg_get_int: '%s' -> '%d'.\n", name, item->val.intVal);
    return item->val.intVal;
}

gboolean rx_cfg_get_bool(const rx_cfg * cfg, const gchar * name)
{
    rx_cfg_item *item;

    dlog("rx_cfg_get_bool: '%s'.\n", name);
    item = rx_cfg_get(cfg, name);
    if (!item) {
        dlog("rx_cfg_get_bool: '%s' not present.\n", name);
        return FALSE;
    }
    if (item->type != CFG_TYPE_BOOL) {
        dlog("rx_cfg_get_bool: '%s' not boolean.\n", name);
        return FALSE;
    }
    dlog("rx_cfg_get_bool: '%s' -> '%s'.\n", name, (item->val.boolVal ? "true" : "false"));
    return item->val.boolVal;
}

gfloat rx_cfg_get_float(const rx_cfg * cfg, const gchar * name)
{
    rx_cfg_item *item;

    dlog("rx_cfg_get_float: '%s'.\n", name);
    item = rx_cfg_get(cfg, name);
    if (!item) {
        dlog("rx_cfg_get_float: '%s' not present.\n", name);
        return 0.0;
    }
    if (item->type != CFG_TYPE_FLOAT) {
        dlog("rx_cfg_get_float: '%s' not float.\n", name);
        return 0.0;
    }
    dlog("rx_cfg_get_float: '%s' -> '%f'.\n", name, item->val.floatVal);
    return item->val.floatVal;
}

gchar *rx_cfg_get_to_str(const rx_cfg * cfg, const gchar * name)
{
    rx_cfg_item *item;
    gchar *str = NULL, buf[32];

    dlog("rx_cfg_get_to_str: '%s'.\n", name);
    item = rx_cfg_get(cfg, name);
    if (!item) {
        dlog("rx_cfg_get_to_str: '%s' not present.\n", name);
        return NULL;
    }
    switch (item->type) {
        case CFG_TYPE_STR:
            str = g_strdup(item->val.strVal);
            break;
        case CFG_TYPE_INT:
            g_snprintf(buf, sizeof(buf), CFG_INT_FORMAT, item->val.intVal);
            str = g_strdup(buf);
            break;
        case CFG_TYPE_BOOL:
            str = (item->val.boolVal ? g_strdup(CFG_VAL_TRUE) : g_strdup(CFG_VAL_FALSE));
            break;
        case CFG_TYPE_FLOAT:
            g_snprintf(buf, sizeof(buf), CFG_FLOAT_FORMAT, item->val.floatVal);
            str = g_strdup(buf);
            break;
        default:
            // No-op if we don't know it.
            // This catch-all statement is here to avoid compiler warnings.
    }
    dlog("rx_cfg_get_to_str: '%s' -> '%s'.\n", name, str);
    return str;
}


// ===[ Mutators ]===

rx_cfg_error rx_cfg_upd_str(rx_cfg * cfg, const gchar * name, const gchar * val)
{
    rx_cfg_item *item;

    dlog("rx_cfg_upd_str: '%s' -> '%s'.\n", name, val);
    item = rx_cfg_get(cfg, name);
    if (!item) {
        dlog("rx_cfg_upd_str: '%s' not present.\n", name);
        return CFG_ERR_ABSENT;
    }
    switch (item->type) {
        case CFG_TYPE_STR:
            g_free(item->val.strVal);
            item->val.strVal = g_strdup(val);
            break;
        case CFG_TYPE_INT:
            {
                gint i;
                gchar *aux;

                i = strtol(val, &aux, 10);
                if (*aux != '\0') {
                    dlog("rx_cfg_upd_str: '%s' is invalid integer.\n", val);
                    return CFG_ERR_CONVERT;
                }
                item->val.intVal = i;
            }
            break;
        case CFG_TYPE_BOOL:
            if (g_ascii_strcasecmp(val, CFG_VAL_TRUE) == 0) {
                item->val.boolVal = TRUE;
            } else if (g_ascii_strcasecmp(val, CFG_VAL_FALSE) == 0) {
                item->val.boolVal = FALSE;
            } else {
                dlog("rx_cfg_upd_str: '%s' is invalid boolean.\n", val);
                return CFG_ERR_CONVERT;
            }
            break;
        case CFG_TYPE_FLOAT:
            {
                gfloat f;
                gchar *aux;

                f = g_ascii_strtod(val, &aux);
                if (*aux != '\0') {
                    dlog("rx_cfg_upd_str: '%s' is invalid float.\n", val);
                    return CFG_ERR_CONVERT;
                }
                item->val.floatVal = f;
            }
            break;
        default:
            // No-op if we don't know it.
            // This catch-all statement is here to avoid compiler warnings.
    }
    return CFG_ERR_NONE;
}

rx_cfg_error rx_cfg_upd_int(rx_cfg * cfg, const gchar * name, const gint val)
{
    rx_cfg_item *item;

    dlog("rx_cfg_upd_int: '%s' -> '%d'.\n", name, val);
    item = rx_cfg_get(cfg, name);
    if (!item) {
        dlog("rx_cfg_upd_int: '%s' not present.\n", name);
        return CFG_ERR_ABSENT;
    }
    if (item->type != CFG_TYPE_INT) {
        dlog("rx_cfg_upd_int: '%s' not integer.\n", name);
        return CFG_ERR_TYPE;
    }
    item->val.intVal = val;
    return CFG_ERR_NONE;
}

rx_cfg_error rx_cfg_upd_bool(rx_cfg * cfg, const gchar * name, const gboolean val)
{
    rx_cfg_item *item;

    dlog("rx_cfg_upd_bool: '%s' -> '%s'.\n", name, (val ? "TRUE" : "FALSE"));
    item = rx_cfg_get(cfg, name);
    if (!item) {
        dlog("rx_cfg_upd_bool: '%s' not present.\n", name);
        return CFG_ERR_ABSENT;
    }
    if (item->type != CFG_TYPE_BOOL) {
        dlog("rx_cfg_upd_bool: '%s' not boolean.\n", name);
        return CFG_ERR_TYPE;
    }
    item->val.boolVal = val;
    return CFG_ERR_NONE;
}

rx_cfg_error rx_cfg_upd_float(rx_cfg * cfg, const gchar * name, const gfloat val)
{
    rx_cfg_item *item;

    dlog("rx_cfg_upd_float: '%s' -> '%f'.\n", name, val);
    item = rx_cfg_get(cfg, name);
    if (!item) {
        dlog("rx_cfg_upd_float: '%s' not present.\n", name);
        return CFG_ERR_ABSENT;
    }
    if (item->type != CFG_TYPE_FLOAT) {
        dlog("rx_cfg_upd_float: '%s' not float.\n", name);
        return CFG_ERR_TYPE;
    }
    item->val.floatVal = val;
    return CFG_ERR_NONE;
}


// ===[ Miscellaneous ]===

void rx_cfg_print_1(gpointer key, gpointer value, gpointer user_data)
{
    rx_cfg_item *item;

    item = (rx_cfg_item *) value;
    dlog("\t'%s' -> '", (gchar *) key);
    switch (item->type) {
        case CFG_TYPE_STR:
            dlog("%s", item->val.strVal);
            break;
        case CFG_TYPE_INT:
            dlog("%d", item->val.intVal);
            break;
        case CFG_TYPE_BOOL:
            dlog("%s", (item->val.boolVal ? "true" : "false"));
            break;
        case CFG_TYPE_FLOAT:
            dlog("%f", item->val.floatVal);
            break;
        default:
            // No-op if we don't know it.
            // This catch-all statement is here to avoid compiler warnings.
    }
    dlog("'.\n");
}

void rx_cfg_print(const rx_cfg * cfg)
{
    dlog("rx_cfg_print.\n");
    rx_cfg_lock(cfg);
    g_hash_table_foreach((GHashTable *) cfg, rx_cfg_print_1, NULL);
    rx_cfg_unlock(cfg);
}

void rx_cfg_lock(const rx_cfg * cfg)
{
    rx_cfg_item *item;
    GStaticMutex *mutex;

    item = g_hash_table_lookup((GHashTable *) cfg, RX_CFG_MUTEX);
    mutex = (GStaticMutex *) item->val.complexVal;
    g_static_mutex_lock(mutex);
}

void rx_cfg_unlock(const rx_cfg * cfg)
{
    rx_cfg_item *item;
    GStaticMutex *mutex;

    item = g_hash_table_lookup((GHashTable *) cfg, RX_CFG_MUTEX);
    mutex = (GStaticMutex *) item->val.complexVal;
    g_static_mutex_unlock(mutex);
}
