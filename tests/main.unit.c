#include "../src/util/meta/version.h"
#include "../src/util/unit/unit.h"

void mem_string(unit_S *s);
void rvm_error(unit_S *s);

void unit_main(unit_G *g) {
    puts(META_VERSION " (" META_VERSION_HASH ")");

    unit_suite(g, mem_string);
    unit_suite(g, rvm_error);
}
