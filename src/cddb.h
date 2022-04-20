#ifndef RX_CDDB_H
#define RX_CDDB_H 1

#include <cddb/cddb.h>

#include "ripperx.h"

#define CDDB_NOT_FOUND 0
#define CDDB_SUCCESS 1
#define CDDB_CONNECTION_REFUSED -1
#define CDDB_RESOLV_ERROR = -2

void init_cddb(void);
int cddb_lookup(cddb_disc_t * disc);
int cddb_submit(cddb_disc_t * disc);

#endif
