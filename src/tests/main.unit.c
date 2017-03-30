#include "../lib/meta/meta.h"
#include "../lib/unit/unit.h"

void compiler_parse(unit_S *s);

void unit_main(unit_G *g) {
    puts(META_VERSION " (" META_REVISION_HASH ")");

    unit_suite(g, compiler_parse);
}

