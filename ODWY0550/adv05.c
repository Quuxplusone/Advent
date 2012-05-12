#include "adv0.h"
#include "adv1.h"
#include "adv3.h"

void initial()
{
    puts("");
    /* Move all items to LIMBO. */
    for (int j = FOBJ; j <= LOBJ; ++j)
        apport(j, LIMBO);
    initialize_portability();
    set_freebie_objects();
    set_rooms_lit();
    set_nodwarf_rooms();
    set_notincave_rooms();
    set_noback_rooms();
    set_inmaze_rooms();
    set_oneexit_rooms();
    set_openable_objects();
    set_invisible_objects();
    set_hintable_rooms();
    set_water_rooms();
    set_thrower_rooms();
    set_valued_objects();
    set_unstable_objects();
    set_mortal_objects();
    move_objects_to_starting_locations();
    set_schizoid_objects();
    wake_up_and_greet_user();
    return;
}

void p26()
{
    p27();
    p28();
    return;
}

void p31()
{
    p32();
    p33();
    return;
}

void p34()
{
    p35();
    p36();
    return;
}

void p49()
{
    p50();
    p51();
    return;
}

void p52()
{
    p53();
    p54();
    return;
}

void p55()
{
    p56();
    p57();
    p58();
    return;
}

void p62()
{
    action_BREAK_VASE();
    action_BREAK_VIAL();
    return;
}

void p66()
{
    p67();
    p68();
    cross_wheatstone_bridge();
    return;
}

void p72()
{
    p73();
    p74();
    return;
}

void p75()
{
    p76();
    p77();
    return;
}

void p78()
{
    p79();
    p80();
    return;
}

void p81()
{
    at_BUILDING();
    at_BUILDING_downstream();
    return;
}

void p105()
{
    action_CAVE_outdoors();
    action_CAVE();
    return;
}

void p108()
{
    p109();
    jumping_into_whirlpool();
    return;
}

void p114()
{
    action_CLOSE_grate();
    action_CLOSE_CHAIN();
    action_CLOSE_DOOR();
    action_CLOSE_SAFE();
    action_CLOSE_FLASK();
    action_CLOSE_CAVE();
    return;
}

void p130()
{
    p131();
    p132();
    return;
}

void p144()
{
    p145();
    p146();
    return;
}

void p147()
{
    p148();
    p149();
    return;
}

void p155()
{
    action_DROP_special();
    action_DROP();
    return;
}

void p168()
{
    p169();
    p170();
    p171();
    return;
}

void p173()
{
    action_EAT_nearmushroom();
    action_EAT_nearfood();
    action_EAT_mortal();
    action_EAT();
    return;
}

void p179()
{
    p180();
    p181();
    return;
}

void p183()
{
    p184();
    p185();
    return;
}

void p186()
{
    p187();
    p188();
    return;
}

void p190()
{
    action_FEE_fiefoefoo();
    action_FEE();
    return;
}

void p193()
{
    action_FEED_mortal();
    action_FEED();
    return;
}

void p197()
{
    action_FILL_VASE();
    action_FILL_BOTTLE();
    action_FILL();
    return;
}

void p201()
{
    action_FIND_CAVE();
    action_FIND();
    return;
}

void p208()
{
    p209();
    p210();
    return;
}

void p218()
{
    action_GET_special();
    action_GET();
    return;
}

void p233()
{
    p234();
    p235();
    return;
}

void p283()
{
    p284();
    p285();
    return;
}

void p288()
{
    p289();
    p290();
    return;
}

void p293()
{
    p294();
    p295();
    return;
}

void p300()
{
    action_KILL_mortal();
    action_KILL();
    return;
}

void p323()
{
    action_LOOK_scenery();
    action_LOOK();
    return;
}

void p367()
{
    p368();
    p369();
    return;
}

void p371()
{
    p372();
    p373();
    return;
}

void p375()
{
    p376();
    p377();
    return;
}

void p378()
{
    p379();
    p380();
    return;
}

void p382()
{
    p383();
    p384();
    return;
}

void p385()
{
    p386();
    p387();
    return;
}

void p388()
{
    p389();
    p390();
    p391();
    p392();
    p393();
    return;
}

void p396()
{
    p397();
    p398();
    return;
}

void p402()
{
    action_NOSIDE_SAMOHT();
    action_NOSIDE();
    return;
}

void p407()
{
    action_OIL_DOOR();
    action_OIL_PLANT();
    action_OIL();
    return;
}

void p413()
{
    action_OPEN_grate();
    action_OPEN_CHAIN();
    action_OPEN_DOOR();
    action_OPEN_KEYS();
    action_OPEN_SAFE();
    action_OPEN_CLAM();
    action_OPEN_OYSTER();
    action_OPEN_VIAL();
    action_OPEN_FLASK();
    action_OPEN_PENTAGRAM();
    return;
}

void p427()
{
    p428();
    p429();
    return;
}

void p430()
{
    p431();
    p432();
    return;
}

void p435()
{
    p436();
    p437();
    p438();
    p439();
    return;
}

void p443()
{
    p444();
    p445();
    return;
}

void p446()
{
    p447();
    p448();
    return;
}

void p449()
{
    p450();
    p451();
    p452();
    return;
}

void p455()
{
    action_POUR_WATER();
    action_POUR_OIL();
    return;
}

void p471()
{
    action_RUB_LAMP();
    action_RUB();
    return;
}

void p478()
{
    action_SAY_magicword();
    action_SAY();
    return;
}

void p482()
{
    p483();
    p484();
    p485();
    return;
}

void p486()
{
    p487();
    p488();
    p489();
    return;
}

void p490()
{
    p491();
    p492();
    return;
}

void p498()
{
    p499();
    p500();
    return;
}

void p503()
{
    p504();
    p505();
    return;
}

void p506()
{
    p507();
    p508();
    return;
}

void p518()
{
    p519();
    p520();
    return;
}

void p523()
{
    p524();
    p525();
    return;
}

void p528()
{
    p529();
    p530();
    p531();
    p532();
    return;
}

void p534()
{
    action_THROW_weaponry();
    action_THROW_FOOD();
    action_THROW_TEETH();
    action_THROW_VIAL();
    action_THROW();
    return;
}

void p551()
{
    p552();
    p553();
    return;
}

void p557()
{
    action_WATER_DOOR();
    action_WATER_PLANT();
    action_WATER();
    return;
}

void p564()
{
    p565();
    p566();
    return;
}

void p567()
{
    p568();
    p569();
    return;
}

void p571()
{
    p572();
    p573();
    p574();
    p575();
    return;
}

void p576()
{
    p577();
    p578();
    p579();
    return;
}

void p581()
{
    p582();
    p583();
    return;
}

void p584()
{
    p585();
    p586();
    return;
}

void p587()
{
    p588();
    p589();
    return;
}

void p592()
{
    p593();
    p594();
    return;
}

void p0(void)
{
    return;
}
