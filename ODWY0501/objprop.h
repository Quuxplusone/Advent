
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
bool is_liquid(ObjectWord t);
int weight(ObjectWord t);

bool here(ObjectWord t, Location loc);
bool is_at_loc(ObjectWord t, Location loc);
bool at_hand(ObjectWord t, Location loc);
bool toting(ObjectWord t);
bool holding(ObjectWord t);
bool enclosed(ObjectWord t);
bool is_ajar(ObjectWord t);
bool is_broken(ObjectWord t);
int burden(ObjectWord t);

#define move(t,l) do { carry(t); drop((t),l); } while (0)
#define juggle(t) do { Location l = objs(t).place; move(t, l); } while (0)
#define destroy(t) move((t), R_LIMBO)

void drop(ObjectWord t, Location l);
void insert(ObjectWord t, ObjectWord container);
void remove_from_containers(ObjectWord t);
void carry(ObjectWord t);
