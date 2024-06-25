#pragma once

#include "defs.h"

extern int light_shaft;
extern ObjectP btie;
extern ObjectP binf;
extern int beach_dig;

bool balloon_burn();
void fweep(int num, int slp = 0);

RAPPLIC(blast);
RAPPLIC(breathe);
RAPPLIC(burnup);
RAPPLIC(digger);
RAPPLIC(geronimo);
RAPPLIC(ledge_mung);
RAPPLIC(safe_mung);
RAPPLIC(shaker);
RAPPLIC(swimmer);
RAPPLIC(volgnome);

