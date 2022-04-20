#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

#include "ripper_plugin.h"
#include "ripperx.h"
#include "main.h"
#include "config_handler.h"
#include "plugin_handling.h"

#define XML_ROOT     "ripperx"
#define XML_ENCODERS "encoders"
#define XML_NAME     "name"
#define XML_OPTION   "option"
#define XML_OPTIONS  "options"
#define XML_PLUGIN   "plugin"
#define XML_RIPPERS  "rippers"
#define XML_VALUE    "value"


/**
 * Main configuration structure. 
 */
rx_cfg *config = NULL;

static void load_config(void);
static void set_option(gchar * name, gchar * value);
void parse_plugins(rx_cfg * parent, xmlNodePtr node, gboolean is_ripper);
void parse_options(xmlNodePtr node, rx_cfg * cfg);
void unparse_options(xmlNodePtr node, const rx_cfg * cfg);
void unparse_plugin_1(gpointer key, gpointer value, gpointer user_data);
void get_config_file_name(char *buf, gint size);

// ===[ GUI ]===

void show_config()
{
//    gtk_widget_show(config_dialog);
}


void hide_config()
{
//    gtk_widget_hide(config_dialog);
}


// ===[ Loading/initializing configuration ]===

void init_config()
{
    dlog("init_config.\n");
    load_config();
}

static void load_config()
{
    xmlDocPtr doc;
    xmlNodePtr node, root;
    gchar *file;
    rx_cfg *main;

    file = g_malloc(MAX_FILE_LENGTH);
    get_config_file_name(file, MAX_FILE_LENGTH);
    dlog("load_config: '%s'.\n", file);
    if (!g_file_test(file, G_FILE_TEST_EXISTS)) {
        // File does not exist, just return
        dlog("load_config: '%s' does not exist.\n");
        g_free(file);
        return;
    };
    // TODO: more sanity checks on file (is regular, is readable)?

    doc = xmlParseFile(file);
    if (!doc) {
        perror("couldn't load config");
        g_free(file);
        exit(1);
    }
    root = xmlDocGetRootElement(doc);
    if (!root || !root->name) {
        perror("couldn't load config");
        g_free(file);
        exit(1);
    }
    for (node = root->children; node != NULL; node = node->next) {
        if (node->type != XML_ELEMENT_NODE)
            continue;
        if (g_ascii_strcasecmp(node->name, XML_OPTIONS) == 0) { /*general options */

            dlog("  found general options.\n");
            main = get_configuration();
            parse_options(node, main);
        } else if (g_ascii_strcasecmp(node->name, XML_RIPPERS) == 0) {
            parse_plugins(main, node, TRUE);
        } else if (g_ascii_strcasecmp(node->name, XML_ENCODERS) == 0) {
            parse_plugins(main, node, FALSE);
        }
    }
    g_free(doc);
    g_free(file);
}

rx_cfg *get_configuration(void)
{
    dlog("get_configuration.\n");
    // This method will be called during RipperX start-up, so no
    // synchronization necessary to initialize the configuration
    // structure.
    if (!config) {
        config = rx_cfg_new(NULL);
        rx_cfg_add_str(config, CFG_MAIN_PLUGIN_DIR, "Plugins Dir", PLUGINSDIR);
        rx_cfg_add_str(config, CFG_MAIN_SRC_PATH, "Source path", "/tmp");
        rx_cfg_add_str(config, CFG_MAIN_ENC_PATH, "Encoded path", "/tmp");
        rx_cfg_add_str(config, CFG_MAIN_DIR_TEMPLATE, "Directory template", "%a - %v");
        rx_cfg_add_str(config, CFG_MAIN_FILE_TEMPLATE, "File template", "%# %t");
        rx_cfg_add_str(config, CFG_MAIN_CDDB_PATH, "CDDB path", "~/.cddbslave");
        rx_cfg_add_bool(config, CFG_MAIN_XLT_SPC, "Convert spaces", FALSE);
        rx_cfg_add_bool(config, CFG_MAIN_CREATE_TAG, "Create ID3 tags", TRUE);
        rx_cfg_add_int(config, CFG_MAIN_BIT_RATE, "Bit rate", 128);
        rx_cfg_add_bool(config, CFG_MAIN_DIR_ALBUM, "Album directories", TRUE);
        rx_cfg_add_bool(config, CFG_MAIN_KEEP_WAV, "Keep WAV files After ripping", FALSE);
        rx_cfg_add_str(config, CFG_MAIN_ENABLED_RIP, "Enable ripping", "");
        rx_cfg_add_str(config, CFG_MAIN_ENABLED_ENC, "Enable encoding", "");
        rx_cfg_add_bool(config, CFG_MAIN_ENCODE, "Enable encoding", TRUE);
        rx_cfg_add_float(config, CFG_MAIN_RIP_RATIO, "Ripping Ratio", 0.1);
        rx_cfg_add_float(config, CFG_MAIN_ENC_RATIO, "Encoding Ratio", 0.1);
        rx_cfg_add_str(config, CFG_MAIN_RIPPER, "Ripper plugin", "cdparanoia");
        rx_cfg_add_str(config, CFG_MAIN_ENCODER, "Encoder plugin", "lame");
        rx_cfg_add_str(config, CFG_MAIN_CDDB_SERVER, "CDDB Server", "freedb.freedb.org");
        rx_cfg_add_str(config, CFG_MAIN_CDDB_URL, "CDDB query URL", "~cddbd/cddb.cgi");
        rx_cfg_add_str(config, CFG_MAIN_CDDB_URL_SUBMIT, "CDDB submit URL", "~cddbd/submit.cgi");
        rx_cfg_add_int(config, CFG_MAIN_CDDB_PORT, "CDDB Port", 80);
        rx_cfg_add_bool(config, CFG_MAIN_CDDB_USE_HTTP, "Use HTTP for CDDB", TRUE);
        rx_cfg_add_bool(config, CFG_MAIN_CDDB_USE_PROXY, "Use HTTP Proxy", FALSE);
        rx_cfg_add_str(config, CFG_MAIN_CDDB_PROXY, "Proxy Server", "proxy");
        rx_cfg_add_int(config, CFG_MAIN_CDDB_PROXY_PORT, "Proxy Port", 8080);
    }
    return config;
}

void parse_plugins(rx_cfg * parent, xmlNodePtr node, gboolean is_ripper)
{
    xmlNodePtr plugin;
    gchar *name;
    rx_cfg *plugin_cfg;
    general_plugin *p;

    dlog("parse_plugins: %s.\n", (is_ripper ? "rippers" : "encoders"));
    for (plugin = node->children; plugin != NULL; plugin = plugin->next) {
        if ((plugin->type != XML_ELEMENT_NODE) ||
            (g_ascii_strcasecmp(plugin->name, XML_PLUGIN) != 0))
            continue;
        name = xmlGetProp(plugin, "name");
        if (!name) {
            dlog("  plugin: name missing.\n");
            continue;
        }
        dlog("  plugin: '%s'.\n", name);
        if (is_ripper) {
            p = (general_plugin *) g_hash_table_lookup(rippers, name);
        } else {
            p = (general_plugin *) g_hash_table_lookup(encoders, name);
        }
        if (!p) {
            dlog("  plugin: '%s' unknown.\n", name);
            continue;
        }
        if (!p->get_configuration) {
            dlog("  plugin: '%s' does not want to be configured.\n", name);
            continue;
        }
        plugin_cfg = p->get_configuration();
        rx_cfg_set_parent(plugin_cfg, parent);
        parse_options(plugin, plugin_cfg);
    }
}

/**
 * Expects a node, containing zero or more 'OPTION' nodes.
 */
void parse_options(xmlNodePtr node, rx_cfg * cfg)
{
    xmlNodePtr option;
    gchar *name, *value;
    rx_cfg_error err;

    dlog("parse_options.\n");
    for (option = node->children; option != NULL; option = option->next) {
        if ((option->type != XML_ELEMENT_NODE) ||
            (g_ascii_strcasecmp(option->name, XML_OPTION) != 0))
            continue;
        name = xmlGetProp(option, "name");
        value = xmlGetProp(option, "value");
        dlog("  option: '%s' -> '%s'.\n", name, value);
        err = rx_cfg_upd_str(cfg, name, value);
        switch (err) {
            case CFG_ERR_ABSENT:
                dlog("  option: '%s' unknown.\n", name);
                break;
            case CFG_ERR_CONVERT:
                dlog("  option: '%s' conversion failed.\n", name);
                break;
        }
    }
}

// ===[ Saving configuration ]===

void save_config(const rx_cfg * config)
{
    xmlDocPtr doc;
    xmlNodePtr root, node;
    gchar file[MAX_FILE_LENGTH];

    dlog("save_config.\n");
    doc = xmlNewDoc("1.0");
    root = xmlNewDocNode(doc, NULL, XML_ROOT, NULL);
    xmlDocSetRootElement(doc, root);
    dlog("\tgeneral configuration.\n");
    node = xmlNewChild(root, NULL, XML_OPTIONS, NULL);
    unparse_options(node, config);
    dlog("\tadd ripper plugins.\n");
    node = xmlNewChild(root, NULL, XML_RIPPERS, NULL);
    g_hash_table_foreach(rippers, unparse_plugin_1, node);
    dlog("\tadd encoder plugins.\n");
    node = xmlNewChild(root, NULL, XML_ENCODERS, NULL);
    g_hash_table_foreach(encoders, unparse_plugin_1, node);
    get_config_file_name(file, MAX_FILE_LENGTH);
    dlog("\tsave file: '%s'.\n", file);
    if (xmlSaveFormatFile(file, doc, TRUE) == -1) {
        perror("could not save configuration.");
    }
    xmlFreeDoc(doc);
}

void unparse_plugin_1(gpointer key, gpointer value, gpointer user_data)
{
    general_plugin *plugin;
    rx_cfg *cfg;
    xmlNodePtr parent, node;

    plugin = (general_plugin *) value;
    dlog("unparse_plugin_1: plugin name '%s'.\n", plugin->name);
    parent = (xmlNodePtr) user_data;
    node = xmlNewChild(parent, NULL, XML_PLUGIN, NULL);
    xmlNewProp(node, XML_NAME, plugin->name);
    cfg = plugin->get_configuration();
    unparse_options(node, cfg);
}

void unparse_options(xmlNodePtr node, const rx_cfg * cfg)
{
    xmlNodePtr option;
    GPtrArray *names;
    gchar *name, *value;
    guint i;

    dlog("unparse_options.\n");
    names = rx_cfg_get_names(cfg);
    i = 0;
    name = (gchar *) g_ptr_array_index(names, i);
    while (name) {
        value = rx_cfg_get_to_str(cfg, name);
        dlog("unparse_options: [%d] '%s' -> '%s'.\n", i, name, value);
        option = xmlNewChild(node, NULL, XML_OPTION, NULL);
        xmlNewProp(option, XML_NAME, name);
        xmlNewProp(option, XML_VALUE, value);
        g_free(value);
        name = (gchar *) g_ptr_array_index(names, ++i);
    }
    g_ptr_array_free(names, TRUE);
}

// ===[ Miscelleaneous ]===

void get_config_file_name(char *buf, gint size)
{
    g_snprintf(buf, size, "%s/.ripperXrc.xml", getenv("HOME"));
}
