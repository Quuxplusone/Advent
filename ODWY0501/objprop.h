
#include <stdbool.h>

bool is_hinged(ObjectWord t);
bool has_lock(ObjectWord t);
bool is_edible(ObjectWord t);
bool is_readable(ObjectWord t);
bool is_living(ObjectWord t);
bool is_wearable(ObjectWord t);
bool is_plural(ObjectWord t);
bool is_treasure(ObjectWord t);
bool is_vessel(ObjectWord t);
bool is_small(ObjectWord t);
bool is_opaque(ObjectWord t);

bool here(ObjectWord t, Location loc);
bool at_hand(ObjectWord t, Location loc);
bool toting(ObjectWord t);
bool holding(ObjectWord t);
bool enclosed(ObjectWord t);
bool is_ajar(ObjectWord t);