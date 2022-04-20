#ifndef CONFIG_HANDLER_H
#define CONFIG_HANDLER_H 1

#include "config_defines.h"
#include "rx_cfg.h"

void show_config(void);
void init_config(void);
void hide_config(void);
gboolean parse_dialog(void);
void save_config(const rx_cfg * config);
rx_cfg *get_configuration(void);

#endif
