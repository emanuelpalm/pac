#include "../lib/meta/version.h"
#include "../lib/unit/unit.h"

void mem_string(unit_S *s);

void unit_main(unit_G *g) {
    puts(META_VERSION " (" META_VERSION_HASH ")");

    unit_suite(g, mem_string);
}

