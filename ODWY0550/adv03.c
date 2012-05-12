#include <assert.h>
#include "adv0.h"
#include "adv3.h"

void p56(void)
{
    move(148, 0, -367);
    move(150, 0, -368);
    move(151, 0, -151);
}

void p57(void)
{
    if (!anyof(369, 370, 357, 358, -1))
        return;
    if (pct(65)) {
        say(0, 624, 0);
        longjmp(loop_back, 1);
    } else {
        move(185, 0, -358);
        if (pct(75)) {
            move(155, 0, -369);
            move(125, 0, -357);
            move(151, 0, -370);
        } else {
            move(154, 0, -369);
            move(153, 0, -357);
            move(160, 0, -370);
        }
    }
}

void p58(void)
{
    if (!keyword(374, -1))
        return;
    if (pct(50)) {
        say(0, 552, 0);
    } else {
        move(176, -1);
    }
    longjmp(loop_back, 1);
}

void p527(void)
{
    move(149, 0, -371);
    move(112, 0, -368);
    move(160, 0, -103);
    move(175, 0, -367);
    move(176, 0, -176);
    move(175, 0, -175);
    if (!anyof(374, 370, -1))
        return;
    if (pct(65)) {
        move(176, 0, -374);
        say(0, 552, 0);
    } else {
        move(160, 0, -370);
        say(0, 552, 0);
    }
    longjmp(loop_back, 1);
}

void p178(void)
{
    move(150, 0, -367);
    move(111, 0, -368);
    move(111, 0, -366);
    move(113, 0, -358);
    move(113, 0, -105);
}

void p501(void)
{
    move(111, 0, -370);
    move(152, 0, -357);
    move(152, 0, -376);
    move(149, 0, -369);
    move(149, 0, -149);
}

void p495(void)
{
    move(151, 0, -358);
    move(151, 0, -151);
    move(188, 0, -369);
    move(188, 0, -9);
    move(191, 0, -191);
    move(202, 0, -370);
}

void p494(void)
{
    move(154, 0, -369);
    move(149, 0, -358);
    move(149, 0, -353);
    move(189, 0, -370);
}

void p493(void)
{
    move(149, 0, -372);
    move(153, 0, -370);
    move(121, 0, -369);
}

void p327(void)
{
    move(149, 0, -149);
    move(201, 0, -373);
    move(156, 0, -369);
    move(176, 0, -372);
    move(176, 0, -176);
}

void p142(void)
{
    move(155, 0, -370);
    move(155, 0, -349);
    move(155, 0, -343);
}

void p491(void)
{
    move(110, 0, -367);
    move(158, 0, -358);
}

void p492(void)
{
    if (!keyword(368, -1))
        return;
    if (value[19] == 0) {
        move(203, -1);
    } else {
        move(202, -1);
    }
    longjmp(loop_back, 1);
}

void p405(void)
{
    move(159, 0, -370);
    move(160, 0, -369);
}

void p542(void)
{
    move(158, 0, -369);
}

void p533(void)
{
    move(158, 0, -367);
    move(161, 0, -368);
    move(150, 0, -369);
    move(150, 0, -349);
}

void p143(void)
{
    move(160, 0, -370);
}

void p348(void)
{
    move(127, 0, -369);
    move(162, 0, -368);
    move(162, 0, -370);
    move(163, 0, -367);
}

void p349(void)
{
    move(162, 0, -368);
    move(162, 0, -343);
}

void p350(void)
{
    move(129, 0, -370);
    move(129, 0, -343);
}

void p351(void)
{
    move(142, 0, -370);
    move(166, 0, -367);
    move(167, 0, -368);
}

void p352(void)
{
    move(142, 0, -369);
    move(165, 0, -368);
    move(198, 0, -374);
}

void p353(void)
{
    move(165, 0, -367);
    move(165, 0, -343);
}

void p354(void)
{
    move(137, 0, -357);
    move(137, 0, -343);
}

void p355(void)
{
    move(130, 0, -357);
    move(130, 0, -358);
}

void p386(void)
{
    move(114, 0, -358);
    move(114, 0, -376);
    move(114, 0, -367);
    move(172, 0, -368);
    move(172, 0, -172);
}

void p387(void)
{
    if (!keyword(365, -1))
        return;
    move(114, -1);
    splatter();
}

void p291(void)
{
    move(174, 0, -369);
    move(174, 0, -174);
    move(174, 0, -353);
    move(155, 803, -358);
    move(155, 803, -376);
}

void p232(void)
{
    move(170, 0, -370);
    move(170, 0, -204);
    move(276, 0, -367);
    move(173, 0, -369);
}

void p284(void)
{
    move(172, 0, -370);
    move(172, 0, -172);
    move(172, 0, -353);
}

void p285(void)
{
    if (!anyof(369, 350, 174, -1))
        return;
    if (value[5] == 0) {
        say(0, 609, 0);
    } else {
        move(174, -1);
    }
    longjmp(loop_back, 1);
}

void p109(void)
{
    move(173, 0, -370);
    move(173, 0, -343);
    move(172, 0, -172);
    move(171, 0, -368);
    move(171, 0, -171);
}

void jumping_into_whirlpool(void)
{
    if (!anyof(358, 365, -1))
        return;
    if (query(857)) {
        value[449] = 69;
        *bitword(449) = -1;
        set('V', 450, 'c', 0, NULL, NULL);
        *bitword(448) = -1;
        value[448] = FOBJ - 1;
        while (++value[448] <= LOBJ) {
            if ((have(value[448], -1, -1)) && value[448] != value[449]) {
                apport(value[448], 334);
                value[450] += 1;
            }
        }
        if (have(69, -1, -1)) {
            set('V', 452, 'c', 1, NULL, NULL);
            if (value[450] == 0) {
                say(0, 858, 0);
            } else {
                say(0, 859, 0);
            }
        } else {
            set('V', 452, 'c', 0, NULL, NULL);
            if (value[450] == 0) {
                say(0, 860, 0);
            } else {
                say(0, 861, 0);
            }
        }
        move(192, -1);
        say(0, 862, 0);
    } else {
        say(2, 465, 0);
    }
    longjmp(loop_back, 1);
}

void p511(void)
{
    move(150, 0, -368);
    move(150, 0, -343);
    move(150, 0, -150);
}

void p424(void)
{
    move(150, 0, -372);
    move(155, 0, -368);
    move(155, 0, -349);
    move(177, 0, -357);
    move(177, 0, -369);
    move(177, 0, -174);
}

void p379(void)
{
    move(176, 0, -370);
    move(176, 0, -176);
    move(178, 0, -368);
}

void p380(void)
{
    if (!keyword(365, -1))
        return;
    move(174, -1);
    splatter();
    longjmp(loop_back, 1);
}

void p27(void)
{
    if (!anyof(367, 353, 179, -1))
        return;
    if (value[452] == 0) {
        move(179, -1);
    } else {
        if (value[452] == 1) {
            if (have(54, -1, -1)) {
                move(179, -1);
            } else {
                say(0, 615, 0);
            }
        } else {
            say(0, 615, 0);
        }
    }
    longjmp(loop_back, 1);
}

void p28(void)
{
    move(177, 0, -374);
    move(177, 0, -174);
}

void p450(void)
{
    if (value[HERE] != 179)
        return;
    if (!keyword(179, -1))
        return;
    if ((value[STATUS] == 1) || KEY(336)) {
        if (have(54, -1, -1)) {
            apport(54, HERE);
        }
        say(2, 479, 0);
        move(118, -1);
        longjmp(loop_back, 1);
    }
}

void p451(void)
{
    if (!anyof(368, 353, 178, -1))
        return;
    if (value[452] == 0) {
        move(178, -1);
    } else {
        if (value[452] == 1) {
            if (have(54, -1, -1)) {
                move(178, -1);
            } else {
                say(0, 615, 0);
            }
        } else {
            say(0, 615, 0);
        }
    }
    longjmp(loop_back, 1);
}

void p452(void)
{
    move(180, 0, -371);
    move(180, 0, -180);
}

void p140(void)
{
    move(179, 0, -370);
    move(179, 0, -179);
    move(179, 0, -343);
}

void p38(void)
{
    move(182, 0, -358);
    move(182, 0, -182);
    move(324, 0, -357);
    move(324, 0, -367);
}

void p499(void)
{
    move(181, 0, -357);
    move(181, 0, -364);
    move(183, 0, -358);
    move(183, 0, -204);
}

void p500(void)
{
    if (!anyof(370, 148, -1))
        return;
    if (have(76, -1, -1)) {
        say(0, 616, 0);
    } else {
        if (have(77, -1, -1)) {
            say(0, 617, 0);
        } else {
            move(148, -1);
        }
    }
    longjmp(loop_back, 1);
}

void p460(void)
{
    move(182, 0, -357);
    move(182, 0, -182);
    move(184, 0, -358);
}

void p137(void)
{
    move(183, 0, -357);
    move(183, 0, -343);
    move(183, 0, -204);
    move(182, 0, -182);
}

void p29(void)
{
    move(148, 0, -357);
    move(149, 0, -368);
    move(187, 0, -367);
}

void p356(void)
{
    move(255, 0, -370);
    move(256, 0, -373);
    move(257, 0, -371);
    move(258, 0, -372);
    move(259, 0, -357);
    move(260, 0, -374);
    move(261, 0, -367);
    move(262, 0, -368);
    move(263, 0, -369);
    move(145, 0, -358);
}

void p588(void)
{
    if (!anyof(369, 370, 367, 357, 358, 371, 374, 372, 373, -1))
        return;
    if (pct(95)) {
        move(187, -1);
    } else {
        move(185, -1);
    }
    longjmp(loop_back, 1);
}

void p589(void)
{
    if (!keyword(368, -1))
        return;
    move(187, -1);
    longjmp(loop_back, 1);
}

void p372(void)
{
    if (!keyword(9, -1))
        return;
    say(0, 643, 0);
    longjmp(loop_back, 1);
}

void p373(void)
{
    move(152, 0, -370);
    move(191, 0, -369);
    move(191, 0, -191);
}

void p585(void)
{
    move(154, 0, -368);
}

void p586(void)
{
    if (!keyword(365, -1))
        return;
    say(0, 705, 0);
    move(188, -1);
    coroner();
}

void p519(void)
{
    move(153, 0, -369);
}

void p520(void)
{
    if (!anyof(358, 365, 376, -1))
        return;
    if (pct(40)) {
        move(135, -1);
    } else {
        if (pct(50)) {
            move(138, -1);
        } else {
            move(130, -1);
        }
    }
    longjmp(loop_back, 1);
}

void p357(void)
{
    move(255, 0, -373);
    move(256, 0, -369);
    move(257, 0, -367);
    move(258, 0, -374);
    move(259, 0, -372);
    move(260, 0, -371);
    move(261, 0, -368);
    move(262, 0, -358);
    move(263, 0, -357);
    move(264, 0, -370);
}

void p466(void)
{
    move(188, 0, -370);
    move(188, 0, -343);
    move(188, 0, -9);
    if (!anyof(369, 366, -1))
        return;
    say(0, 853, 0);
    longjmp(loop_back, 1);
}

void p467(void)
{
    move(193, 0, -369);
    move(193, 0, -353);
    move(193, 0, -193);
    move(194, 0, -194);
    if (!anyof(370, 366, -1))
        return;
    if (isnear(27, -1, -1)) {
        say(0, 854, 0);
        apport(27, LIMBO);
        move(191, -1);
    } else {
        say(0, 853, 0);
    }
    longjmp(loop_back, 1);
}

void p555(void)
{
    move(192, 0, -370);
    move(192, 0, -191);
    move(194, 0, -371);
    move(194, 0, -194);
}

void p43(void)
{
    move(193, 0, -368);
    move(193, 0, -343);
    move(193, 0, -193);
    move(192, 0, -191);
    if (!keyword(365, -1))
        return;
    move(334, -1);
    splatter();
}

void p330(void)
{
    move(166, 0, -372);
}

void p529(void)
{
    move(201, 0, -373);
    move(201, 0, -201);
    move(201, 0, -204);
}

void p530(void)
{
    if (!keyword(392, -1))
        return;
    if ((value[STATUS] == 1) || isnear(22, -1, -1)) {
        return;
    } else {
        if ((KEY(85)) || KEY(86)) {
            if (have(value[ARG2], -1, -1)) {
                say(12, 660, 445);
                apport(value[ARG2], HERE);
            } else {
                return;
            }
        } else {
            if (bitest(evar(445), 15)) {
                if (bitest(evar(445), 2)) {
                    if (have(value[ARG2], -1, -1)) {
                        if (KEY(49)) {
                            if (bitest(21, 10)) {
                                set('E', 21, 'c', 6, NULL, NULL);
                                say(0, 21, 0);
                                set('E', 21, 'c', 0, NULL, NULL);
                                apport(49, 334);
                                longjmp(loop_back, 1);
                            }
                        }
                        say(12, 661, 445);
                        set('E', 21, 'c', 1, NULL, NULL);
                        apport(21, LIMBO);
                        apport(22, 199);
                        apport(value[ARG2], LIMBO);
                    } else {
                        return;
                    }
                } else {
                    return;
                }
            } else {
                return;
            }
        }
    }
    longjmp(loop_back, 1);
}

void p531(void)
{
    if (!anyof(366, 371, -1))
        return;
    if (value[20] > 0) {
        say(0, 663, 0);
    } else {
        if (value[21] == 0) {
            say(0, 662, 0);
        } else {
            if (value[21] == 2) {
                set('E', 21, 'c', 3, NULL, NULL);
                say(0, 21, 0);
                set('E', 21, 'c', 0, NULL, NULL);
                apport(21, 199);
                apport(22, LIMBO);
            } else {
                if (value[21] == 1) {
                    set('E', 21, 'c', 2, NULL, NULL);
                }
                move(200, -1);
            }
        }
    }
    longjmp(loop_back, 1);
}

void p532(void)
{
    if (!keyword(365, -1))
        return;
    if (value[20] == 0) {
        say(0, 594, 0);
    } else {
        move(334, -1);
        splatter();
    }
    longjmp(loop_back, 1);
}

void p509(void)
{
    move(155, 0, -358);
    move(199, 0, -357);
    move(199, 0, -20);
    move(155, 0, -155);
    move(155, 0, -343);
}

void p483(void)
{
    if (!keyword(59, -1))
        return;
    if (value[19] == 0) {
        say(0, 722, 0);
        longjmp(loop_back, 1);
    }
}

void p484(void)
{
    move(152, 0, -369);
    move(152, 0, -343);
}

void p485(void)
{
    if (!anyof(341, 367, -1))
        return;
    if (value[19] == 0) {
        say(0, 649, 0);
    } else {
        move(157, -1);
    }
    longjmp(loop_back, 1);
}

void p487(void)
{
    if (!keyword(59, -1))
        return;
    if (value[19] == 0) {
        say(0, 722, 0);
        longjmp(loop_back, 1);
    }
}

void p488(void)
{
    move(157, 0, -367);
    move(157, 0, -343);
}

void p489(void)
{
    if (!anyof(341, 369, -1))
        return;
    if (value[19] == 0) {
        say(0, 649, 0);
    } else {
        move(152, -1);
    }
    longjmp(loop_back, 1);
}

void p389(void)
{
    if (!keyword(379, 37, -1))
        return;
    if (!have(37, -1, -1))
        return;
    if (!isnear(21, -1, -1))
        return;
    set('E', 21, 'c', 4, NULL, NULL);
    say(0, 665, 0);
    apport(21, LIMBO);
    apport(22, 199);
    apport(37, HERE);
    longjmp(loop_back, 1);
}

void p390(void)
{
    if (!keyword(392, -1))
        return;
    if ((value[STATUS] == 1) || isnear(22, -1, -1)) {
        return;
    } else {
        if ((KEY(85)) || KEY(86)) {
            if (have(value[ARG2], -1, -1)) {
                say(12, 660, 445);
                apport(value[ARG2], HERE);
            } else {
                return;
            }
        } else {
            if (bitest(evar(445), 15)) {
                if (bitest(evar(445), 2)) {
                    if (have(value[ARG2], -1, -1)) {
                        if (KEY(49)) {
                            if (bitest(21, 10)) {
                                set('E', 21, 'c', 6, NULL, NULL);
                                say(0, 21, 0);
                                set('E', 21, 'c', 0, NULL, NULL);
                                apport(49, 334);
                                longjmp(loop_back, 1);
                            }
                        }
                        say(12, 661, 445);
                        set('E', 21, 'c', 1, NULL, NULL);
                        apport(21, LIMBO);
                        apport(22, 199);
                        apport(value[ARG2], LIMBO);
                    } else {
                        return;
                    }
                } else {
                    return;
                }
            } else {
                return;
            }
        }
    }
    longjmp(loop_back, 1);
}

void p391(void)
{
    if (!anyof(366, 373, -1))
        return;
    if (value[20] > 0) {
        say(0, 663, 0);
    } else {
        if (value[21] == 0) {
            say(0, 662, 0);
        } else {
            if (value[21] == 2) {
                set('E', 21, 'c', 3, NULL, NULL);
                say(0, 21, 0);
                set('E', 21, 'c', 0, NULL, NULL);
                apport(21, 199);
                apport(22, LIMBO);
            } else {
                if (value[21] == 1) {
                    set('E', 21, 'c', 2, NULL, NULL);
                }
                if (have(37, -1, -1)) {
                    say(0, 664, 0);
                    set('E', 20, 'c', 1, NULL, NULL);
                    apport(22, LIMBO);
                    move(334, -1);
                    coroner();
                } else {
                    if ((value[21] == 4) && bitest(21, 10)) {
                        if (isnear(37, -1, -1)) {
                            apport(37, INHAND);
                            set('E', 20, 'c', 1, NULL, NULL);
                            say(0, 791, 0);
                        } else {
                            set('E', 20, 'c', 2, NULL, NULL);
                            say(0, 792, 0);
                        }
                        apport(22, LIMBO);
                        move(334, -1);
                        coroner();
                    } else {
                        move(199, -1);
                    }
                }
            }
        }
    }
    longjmp(loop_back, 1);
}

void p392(void)
{
    if (!keyword(365, -1))
        return;
    if (value[20] == 0) {
        say(0, 594, 0);
    } else {
        move(334, -1);
        splatter();
    }
    longjmp(loop_back, 1);
}

void p393(void)
{
    move(204, 0, -371);
    move(204, 0, -204);
    move(205, 0, -205);
    move(207, 0, -354);
    move(253, 0, -253);
}

void p128(void)
{
    move(200, 0, -20);
    move(200, 0, -368);
    move(205, 0, -367);
    move(205, 0, -205);
    move(207, 0, -354);
    move(253, 0, -253);
}

void p212(void)
{
    move(200, 0, -20);
    move(204, 0, -368);
    move(204, 0, -204);
    move(206, 0, -371);
    move(206, 0, -362);
    move(252, 0, -372);
    move(252, 0, -363);
    move(252, 0, -358);
    move(207, 0, -354);
    move(253, 0, -253);
}

void p556(void)
{
    move(205, 0, -370);
    move(205, 0, -205);
    move(207, 0, -369);
    move(207, 0, -354);
    move(251, 0, -367);
    move(251, 0, -349);
}

void p67(void)
{
    move(206, 0, -370);
    move(206, 0, -353);
    move(206, 0, -343);
    move(205, 0, -205);
}

void p68(void)
{
    if (!anyof(365, 358, 376, -1))
        return;
    move(334, -1);
    splatter();
}

void cross_wheatstone_bridge(void)
{
    if (!anyof(95, 366, 29, 369, -1))
        return;
    if (value[29] == 0) {
        say(0, 811, 0);
    } else {
        if (have(63, -1, -1)) {
            if (have(37, -1, -1)) {
                say(0, 815, 0);
                apport(37, 334);
            } else {
                if (!(bitest(208, 1))) {
                    say(0, 813, 0);
                }
            }
            move(208, -1);
        } else {
            if (have(37, -1, -1)) {
                say(0, 814, 0);
            } else {
                say(0, 812, 0);
            }
            move(334, -1);
            coroner();
        }
    }
    longjmp(loop_back, 1);
}

void p61(void)
{
    move(206, 0, -368);
    move(206, 0, -343);
    move(206, 0, -349);
    move(205, 0, -205);
    move(207, 0, -354);
}

void p320(void)
{
    move(205, 0, -369);
    move(205, 0, -357);
    move(205, 0, -205);
    move(253, 0, -370);
    move(253, 0, -358);
    move(253, 0, -253);
    move(207, 0, -354);
}

void p45(void)
{
    move(252, 0, -368);
    move(252, 0, -357);
    move(205, 0, -205);
    move(254, 0, -367);
    move(254, 0, -350);
    move(254, 0, -253);
    move(254, 0, -350);
    move(207, 0, -354);
}

void p53(void)
{
    if (!keyword(378, 85, -1))
        return;
    if (!isnear(85, -1, -1))
        return;
    if ((have(85, -1, -1)) || value[85] == 0) {
        return;
    } else {
        say(12, 721, 445);
        longjmp(loop_back, 1);
    }
}

void p54(void)
{
    move(253, 0, -368);
    move(253, 0, -343);
    move(205, 0, -205);
    move(207, 0, -354);
}

void p358(void)
{
    move(186, 0, -368);
    move(256, 0, -372);
    move(257, 0, -374);
    move(258, 0, -373);
    move(259, 0, -371);
    move(260, 0, -357);
    move(261, 0, -358);
    move(262, 0, -369);
    move(263, 0, -370);
    move(190, 0, -367);
}

void p359(void)
{
    move(186, 0, -374);
    move(255, 0, -357);
    move(257, 0, -369);
    move(258, 0, -370);
    move(259, 0, -368);
    move(260, 0, -373);
    move(261, 0, -371);
    move(262, 0, -367);
    move(263, 0, -358);
    move(190, 0, -372);
}

void p360(void)
{
    move(186, 0, -357);
    move(255, 0, -358);
    move(256, 0, -368);
    move(258, 0, -371);
    move(259, 0, -373);
    move(260, 0, -367);
    move(261, 0, -369);
    move(262, 0, -374);
    move(263, 0, -372);
    move(190, 0, -370);
}

void p361(void)
{
    move(186, 0, -371);
    move(255, 0, -369);
    move(256, 0, -374);
    move(257, 0, -372);
    move(259, 0, -367);
    move(260, 0, -358);
    move(261, 0, -370);
    move(262, 0, -357);
    move(263, 0, -368);
    move(190, 0, -373);
}

void p362(void)
{
    move(186, 0, -369);
    move(255, 0, -372);
    move(256, 0, -358);
    move(257, 0, -370);
    move(258, 0, -367);
    move(260, 0, -368);
    move(261, 0, -373);
    move(262, 0, -371);
    move(263, 0, -374);
    move(190, 0, -357);
}

void p363(void)
{
    move(186, 0, -367);
    move(255, 0, -368);
    move(256, 0, -357);
    move(257, 0, -373);
    move(258, 0, -358);
    move(259, 0, -370);
    move(261, 0, -374);
    move(262, 0, -372);
    move(263, 0, -371);
    move(190, 0, -369);
}

void p364(void)
{
    move(186, 0, -372);
    move(255, 0, -371);
    move(256, 0, -370);
    move(257, 0, -358);
    move(258, 0, -357);
    move(259, 0, -374);
    move(260, 0, -369);
    move(262, 0, -373);
    move(263, 0, -367);
    move(190, 0, -368);
}

void p365(void)
{
    move(186, 0, -358);
    move(255, 0, -367);
    move(256, 0, -371);
    move(257, 0, -357);
    move(258, 0, -368);
    move(259, 0, -369);
    move(260, 0, -370);
    move(261, 0, -372);
    move(263, 0, -373);
    move(190, 0, -374);
}

void p366(void)
{
    move(186, 0, -373);
    move(255, 0, -374);
    move(256, 0, -367);
    move(257, 0, -368);
    move(258, 0, -369);
    move(259, 0, -358);
    move(260, 0, -372);
    move(261, 0, -357);
    move(262, 0, -370);
    move(190, 0, -371);
}

void p368(void)
{
    move(190, 0, -369);
    move(190, 0, -343);
}

void p369(void)
{
    if (!keyword(379, 47, -1))
        return;
    if (!have(47, -1, -1))
        return;
    apport(47, LIMBO);
    apport(38, value[HERE]);
    say(2, 465, 0);
    say(0, 38, 0);
    longjmp(loop_back, 1);
}

void p319(void)
{
    move(94, 0, -343);
}

void p476(void)
{
    move(106, 0, -368);
    move(106, 0, -343);
    move(106, 0, -106);
}

void p381(void)
{
    move(110, 0, -370);
    move(110, 0, -343);
}

void p552(void)
{
    if (!anyof(357, 343, 369, -1))
        return;
    if (value[34] == 1) {
        say(0, 826, 0);
    } else {
        move(110, -1);
    }
    longjmp(loop_back, 1);
}

void p553(void)
{
    if (!keyword(350, -1))
        return;
    if (value[34] == 1) {
        set('V', 473, 'v', 442, NULL, NULL);
        move(269, -1);
    } else {
        say(0, 851, 0);
    }
    longjmp(loop_back, 1);
}

void p294(void)
{
    if (!keyword(343, -1))
        return;
    move(value[473], -1);
    longjmp(loop_back, 1);
}

void p295(void)
{
    if (!keyword(34, -1))
        return;
    if (value[STATUS] == 1) {
        say(12, 714, 445);
    } else {
        if (KEY(381)) {
            say(0, 852, 0);
        } else {
            if (KEY(380)) {
                say(12, 818, 445);
            } else {
                say(0, 608, 0);
            }
        }
    }
    longjmp(loop_back, 1);
}

void p125(void)
{
    move(110, 0, -370);
    move(271, 0, -369);
}

void p126(void)
{
    move(270, 0, -370);
    move(270, 0, -368);
    move(272, 0, -369);
    move(272, 0, -367);
}

void p543(void)
{
    move(271, 0, -343);
    move(271, 0, -370);
}

void p127(void)
{
    move(110, 0, -370);
    move(275, 0, -369);
    move(274, 0, -367);
}

void p136(void)
{
    move(273, 0, -343);
    move(273, 0, -204);
    move(273, 0, -370);
}

void p516(void)
{
    move(273, 0, -343);
    move(273, 0, -369);
    move(273, 0, -204);
}

void p547(void)
{
    move(172, 0, -370);
    move(277, 0, -369);
}

void p234(void)
{
    move(276, 0, -370);
}

void p235(void)
{
    if (!anyof(369, 278, -1))
        return;
    if (isnear(23, -1, -1)) {
        say(0, 805, 0);
    } else {
        move(278, -1);
    }
    longjmp(loop_back, 1);
}

void p317(void)
{
    move(279, 0, -367);
    move(277, 0, -368);
}

void p73(void)
{
    move(278, 0, -369);
    move(280, 0, -368);
    move(319, 0, -367);
    move(278, 0, -369);
}

void p74(void)
{
    if (!keyword(365, -1))
        return;
    plunge();
}

void p76(void)
{
    move(279, 0, -369);
    move(281, 0, -372);
}

void p77(void)
{
    if (!keyword(365, -1))
        return;
    plunge();
}

void p245(void)
{
    move(280, 0, -374);
    move(282, 803, -358);
    move(282, 803, -367);
    move(282, 803, -282);
}

void p504(void)
{
    if (!anyof(282, 357, 369, 376, -1))
        return;
    say(0, 804, 0);
    longjmp(loop_back, 1);
}

void p505(void)
{
    move(286, 0, -370);
    move(289, 0, -374);
}

void p246(void)
{
    move(285, 0, -368);
    move(284, 0, -369);
}

void p259(void)
{
    move(283, 0, -370);
}

void p260(void)
{
    move(283, 0, -367);
    move(287, 0, -368);
    move(286, 0, -369);
}

void p272(void)
{
    move(282, 0, -369);
    move(285, 0, -370);
}

void p273(void)
{
    move(285, 0, -367);
    move(288, 0, -369);
}

void p275(void)
{
    move(287, 0, -370);
}

void p276(void)
{
    move(282, 0, -367);
    move(290, 0, -368);
}

void p277(void)
{
    move(289, 0, -371);
    move(291, 0, -370);
}

void p278(void)
{
    move(290, 0, -369);
    move(292, 0, -370);
    move(294, 0, -368);
}

void p279(void)
{
    move(291, 0, -369);
}

void p280(void)
{
    move(294, 0, -369);
}

void p281(void)
{
    move(291, 0, -367);
    move(293, 0, -370);
    move(295, 0, -369);
}

void p247(void)
{
    move(294, 0, -370);
    move(296, 0, -374);
}

void p248(void)
{
    move(295, 0, -367);
    move(297, 0, -368);
}

void p249(void)
{
    move(296, 0, -371);
    move(298, 0, -370);
    move(301, 0, -368);
}

void p250(void)
{
    move(297, 0, -369);
    move(299, 0, -370);
}

void p251(void)
{
    move(298, 0, -369);
}

void p252(void)
{
    move(302, 0, -369);
}

void p253(void)
{
    move(297, 0, -367);
    move(302, 0, -370);
    move(303, 0, -374);
}

void p254(void)
{
    move(300, 0, -370);
    move(301, 0, -369);
}

void p255(void)
{
    move(301, 0, -367);
    move(304, 0, -368);
}

void p256(void)
{
    move(303, 0, -371);
    move(305, 0, -370);
}

void p257(void)
{
    move(304, 0, -369);
    move(306, 0, -370);
    move(308, 0, -368);
    move(309, 0, -374);
}

void p258(void)
{
    move(305, 0, -369);
    move(307, 0, -368);
}

void p261(void)
{
    move(306, 0, -367);
    move(308, 0, -369);
}

void p262(void)
{
    move(305, 0, -367);
    move(307, 0, -370);
}

void p263(void)
{
    move(305, 0, -372);
    move(310, 0, -374);
}

void p264(void)
{
    move(309, 0, -367);
    move(311, 0, -368);
}

void p265(void)
{
    move(310, 0, -371);
    move(312, 0, -370);
    move(317, 0, -368);
}

void p266(void)
{
    move(311, 0, -369);
    move(313, 0, -370);
    move(315, 0, -368);
    move(316, 0, -374);
}

void p267(void)
{
    move(312, 0, -369);
    move(314, 0, -374);
}

void p268(void)
{
    move(313, 0, -372);
    move(315, 0, -369);
}

void p269(void)
{
    move(312, 0, -367);
    move(314, 0, -370);
}

void p270(void)
{
    move(312, 0, -372);
    move(317, 0, -369);
}

void p271(void)
{
    move(311, 0, -367);
    move(316, 0, -370);
    move(318, 0, -374);
}

void p274(void)
{
    move(317, 0, -367);
    move(281, value[479], -422);
}

void p79(void)
{
    move(279, 0, -369);
    move(320, 0, -371);
    move(320, 0, -360);
}

void p80(void)
{
    if (!keyword(365, -1))
        return;
    plunge();
}

void p129(void)
{
    move(319, 0, -373);
    move(321, 0, -372);
}

void p131(void)
{
    move(320, 0, -368);
}

void p132(void)
{
    if (!keyword(370, -1))
        return;
    if (isnear(32, -1, -1)) {
        say(0, 775, 0);
        coroner();
    } else {
        move(322, -1);
    }
    longjmp(loop_back, 1);
}

void p133(void)
{
    move(321, 0, -369);
    move(323, 0, -370);
    move(323, 0, -349);
}

void p134(void)
{
    move(322, 0, -369);
    move(322, 0, -343);
    move(322, 0, -349);
}

void p32(void)
{
    move(181, 0, -368);
}

void p33(void)
{
    if (!keyword(367, -1))
        return;
    if (((value[31] == 0) || have(76, -1, -1)) || have(77, -1, -1)) {
        set('E', 31, 'c', 0, NULL, NULL);
        if (bitest(31, 10)) {
            say(0, 774, 0);
        } else {
            say(0, 773, 0);
            bitmod('s', 31, 10);
        }
        move(334, -1);
        coroner();
    } else {
        move(325, -1);
        set('E', 31, 'c', 0, NULL, NULL);
    }
    longjmp(loop_back, 1);
}

void p35(void)
{
    move(326, 0, -369);
}

void p36(void)
{
    if (!keyword(368, -1))
        return;
    if (value[31] == 0) {
        if (bitest(31, 10)) {
            say(0, 774, 0);
        } else {
            say(0, 773, 0);
            bitmod('s', 31, 10);
        }
        move(334, -1);
        coroner();
    } else {
        move(324, -1);
        set('E', 31, 'c', 0, NULL, NULL);
    }
    longjmp(loop_back, 1);
}

void p37(void)
{
    move(325, 0, -370);
    move(329, 0, -369);
    move(327, 0, -367);
    move(327, 0, -327);
    move(329, 0, -329);
}

void p213(void)
{
    move(326, 0, -374);
    move(330, 0, -373);
}

void p497(void)
{
    move(329, 0, -368);
    move(331, 0, -358);
    move(331, 0, -4);
}

void p50(void)
{
    if (!anyof(357, 330, 4, 368, -1))
        return;
    set('E', 40, 'c', 1, NULL, NULL);
    move(330, -1);
    longjmp(loop_back, 1);
}

void p51(void)
{
    if (!anyof(82, 397, 390, -1))
        return;
    say(0, 570, 0);
    longjmp(loop_back, 1);
}

void p297(void)
{
    move(328, 0, -370);
    move(326, 0, -368);
}

void p287(void)
{
    move(327, 0, -369);
    move(327, 0, -343);
}

void p180(void)
{
    move(209, 0, -369);
}

void p181(void)
{
    if (!anyof(366, 29, 370, -1))
        return;
    if (value[29] == 0) {
        say(0, 811, 0);
    } else {
        if (have(63, -1, -1)) {
            if (have(55, -1, -1)) {
                say(0, 816, 0);
                move(334, -1);
                set('E', 29, 'c', 0, NULL, NULL);
                coroner();
            } else {
                move(207, -1);
            }
        } else {
            say(0, 812, 0);
            move(334, -1);
            coroner();
        }
    }
    longjmp(loop_back, 1);
}

void p84(void)
{
    if (value[30] == 1) {
        move(210, 0, -374);
        move(218, 0, -369);
        move(247, 0, -371);
    }
    move(208, 0, -370);
}

void p441(void)
{
    move(209, 0, -370);
    if (KEY(369)) {
        set('E', 13, 'c', 0, NULL, NULL);
        apport(13, 211);
        move(211, -1);
        longjmp(loop_back, 1);
    }
}

void p442(void)
{
    if (!anyof(369, 367, 370, 368, 371, 374, 372, 373, -1))
        return;
    *bitword(448) = -1;
    value[448] = FOBJ - 1;
    while (++value[448] <= LOBJ) {
        if (((isnear(value[448], -1, -1)) &&
             !have(value[448], -1, -1)) && bitest(evar(448), 0)) {
            apport(value[448], 334);
        }
    }
    value[448] = 367;
    *bitword(448) = -1;
    value[448] += value[14];
    if ((((value[448] == value[ARG2]) &&
          value[STATUS] == 2) ||
         value[448] == value[ARG1]) && isnear(14, -1, -1)) {
        move(212, -1);
        set('E', 13, 'c', 8, NULL, NULL);
    } else {
        move(211, -1);
    }
    longjmp(loop_back, 1);
}

void p444(void)
{
    if (KEY(358)) {
        move(213, -1);
        apport(13, 210);
        longjmp(loop_back, 1);
    }
}

void p445(void)
{
    if (!anyof(369, 367, 370, 368, 371, 374, 372, 373, -1))
        return;
    move(211, -1);
    set('E', 13, 'c', 0, NULL, NULL);
    longjmp(loop_back, 1);
}

void p397(void)
{
    move(214, 0, -369);
    move(214, 0, -214);
}

void p398(void)
{
    if (!anyof(357, 370, -1))
        return;
    apport(13, 211);
    move(212, -1);
    longjmp(loop_back, 1);
}

void p431(void)
{
    if (!keyword(84, -1))
        return;
    if (!anyof(379, 392, -1))
        return;
    if (!have(84, -1, -1))
        return;
    if (value[84] == 1) {
        apport(84, HERE);
        say(0, 880, 0);
        set('E', 84, 'c', 0, NULL, NULL);
        longjmp(loop_back, 1);
    }
}

void p432(void)
{
    move(213, 0, -368);
    move(213, 0, -343);
    move(213, 0, -213);
    move(215, 0, -369);
    move(215, 0, -360);
    move(215, 0, -215);
}

void p111(void)
{
    move(216, 0, -357);
    move(216, 0, -376);
    move(216, 0, -216);
    move(214, 0, -370);
    move(214, 0, -214);
    move(214, 0, -360);
}

void p545(void)
{
    move(215, 0, -358);
    move(215, 0, -215);
    move(215, 0, -376);
    move(217, 0, -216);
    move(217, 0, -282);
    move(217, 0, -370);
}

void p546(void)
{
    move(210, 803, -370);
    move(210, 803, -358);
    move(210, 803, -282);
    move(216, 0, -369);
    move(215, 0, -215);
    move(215, 0, -216);
}

void p46(void)
{
    move(209, 0, -370);
    if (KEY(369)) {
        move(219, -1);
        value[24] += 1;
        if (value[24] == 1) {
            say(0, 833, 0);
        }
        longjmp(loop_back, 1);
    }
}

void p47(void)
{
    move(220, 0, -369);
    if (KEY(370)) {
        value[24] -= 1;
        if (value[24] == 0) {
            if (have(73, -1, -1)) {
                say(0, 834, 0);
                set('E', 24, 'c', 2, NULL, NULL);
            } else {
                say(0, 835, 0);
                coroner();
            }
        }
        move(218, -1);
        longjmp(loop_back, 1);
    }
}

void p48(void)
{
    move(268, 0, -369);
    move(268, 0, -268);
    move(219, 0, -370);
    move(221, 0, -358);
    move(221, 0, -4);
}

void p428(void)
{
    if (!anyof(370, 343, 205, -1))
        return;
    if (value[34] == 1) {
        say(0, 826, 0);
    } else {
        move(220, -1);
    }
    longjmp(loop_back, 1);
}

void p429(void)
{
    if (!keyword(350, -1))
        return;
    if (value[34] == 1) {
        set('V', 473, 'v', 442, NULL, NULL);
        move(269, -1);
    } else {
        say(0, 851, 0);
    }
    longjmp(loop_back, 1);
}

void p412(void)
{
    move(220, 0, -357);
    move(222, 0, -358);
    move(222, 0, -4);
}

void p521(void)
{
    move(221, 0, -357);
    move(223, 0, -358);
    move(224, 0, -369);
    move(223, 0, -4);
    move(224, 0, -343);
}

void p522(void)
{
    move(222, 0, -357);
    move(222, 0, -4);
}

void p187(void)
{
    move(222, 0, -370);
    move(226, 0, -368);
    move(225, 0, -367);
}

void p188(void)
{
    if (!anyof(425, 179, -1))
        return;
    if (bitest(evar(458), 3)) {
        say(0, 538, 0);
    } else {
        say(2, 479, 0);
        move(332, -1);
    }
    longjmp(loop_back, 1);
}

void p182(void)
{
    move(224, 0, -358);
    move(224, 0, -368);
    move(226, 0, -357);
}

void p39(void)
{
    move(246, 0, -367);
    move(236, 0, -368);
}

void p40(void)
{
    move(245, 0, -368);
}

void p86(void)
{
    move(227, 0, -370);
    if (!anyof(369, 374, 368, 373, 372, 367, 371, 357, 358, -1))
        return;
    move(226, -1);
    longjmp(loop_back, 1);
}

void p97(void)
{
    move(228, 0, -373);
    if (!anyof(369, 374, 368, 370, 372, 367, 371, 357, 358, -1))
        return;
    move(226, -1);
    longjmp(loop_back, 1);
}

void p98(void)
{
    move(229, 0, -374);
    if (!anyof(369, 368, 373, 370, 372, 367, 371, 357, 358, -1))
        return;
    move(227, -1);
    longjmp(loop_back, 1);
}

void p99(void)
{
    move(230, 0, -370);
    if (!anyof(369, 374, 368, 373, 372, 367, 371, 357, 358, -1))
        return;
    move(228, -1);
    longjmp(loop_back, 1);
}

void p100(void)
{
    move(231, 0, -358);
    if (!anyof(369, 374, 368, 373, 370, 372, 367, 371, 357, -1))
        return;
    move(229, -1);
    longjmp(loop_back, 1);
}

void p101(void)
{
    move(232, 0, -368);
    if (!anyof(369, 374, 373, 370, 372, 367, 371, 357, 358, -1))
        return;
    move(230, -1);
    longjmp(loop_back, 1);
}

void p102(void)
{
    move(233, 0, -374);
    if (!anyof(369, 368, 373, 370, 372, 367, 371, 357, 358, -1))
        return;
    move(231, -1);
    longjmp(loop_back, 1);
}

void p103(void)
{
    move(234, 0, -369);
    if (!anyof(374, 368, 373, 370, 372, 367, 371, 357, 358, -1))
        return;
    move(232, -1);
    longjmp(loop_back, 1);
}

void p104(void)
{
    move(235, 0, -370);
    if (!anyof(369, 374, 368, 373, 372, 367, 371, 357, 358, -1))
        return;
    move(233, -1);
    longjmp(loop_back, 1);
}

void p87(void)
{
    move(236, 0, -369);
    if (!anyof(374, 368, 373, 370, 372, 367, 371, 357, 358, -1))
        return;
    move(234, -1);
    longjmp(loop_back, 1);
}

void p88(void)
{
    move(237, 0, -373);
    move(245, 0, -367);
    if (!anyof(369, 374, 368, 370, 372, 371, 357, 358, -1))
        return;
    move(235, -1);
    longjmp(loop_back, 1);
}

void p89(void)
{
    move(238, 0, -367);
    if (!anyof(369, 374, 368, 373, 370, 372, 371, 357, 358, -1))
        return;
    move(236, -1);
    longjmp(loop_back, 1);
}

void p90(void)
{
    move(239, 0, -372);
    if (!anyof(369, 374, 368, 373, 370, 367, 371, 357, 358, -1))
        return;
    move(237, -1);
    longjmp(loop_back, 1);
}

void p91(void)
{
    move(240, 0, -371);
    if (!anyof(369, 374, 368, 373, 370, 372, 367, 357, 358, -1))
        return;
    move(238, -1);
    longjmp(loop_back, 1);
}

void p92(void)
{
    move(241, 0, -367);
    if (!anyof(369, 374, 368, 373, 370, 372, 371, 357, 358, -1))
        return;
    move(239, -1);
    longjmp(loop_back, 1);
}

void p93(void)
{
    move(242, 0, -372);
    if (!anyof(369, 374, 368, 373, 370, 367, 371, 357, 358, -1))
        return;
    move(240, -1);
    longjmp(loop_back, 1);
}

void p94(void)
{
    move(243, 0, -358);
    if (!anyof(369, 374, 368, 373, 370, 372, 367, 371, 357, -1))
        return;
    move(241, -1);
    longjmp(loop_back, 1);
}

void p95(void)
{
    move(244, 0, -370);
    if (!anyof(369, 374, 368, 373, 372, 367, 371, 357, 358, -1))
        return;
    move(242, -1);
    longjmp(loop_back, 1);
}

void p96(void)
{
    move(224, 0, -369);
    if (!anyof(374, 368, 373, 370, 372, 367, 371, 357, 358, -1))
        return;
    move(243, -1);
    longjmp(loop_back, 1);
}

void p44(void)
{
    move(209, 0, -374);
    move(248, 0, -369);
    move(248, 0, -248);
}

void p236(void)
{
    move(249, 0, -371);
    move(250, 0, -374);
    move(249, 0, -249);
    move(250, 0, -250);
    move(247, 0, -370);
}

void p30(void)
{
    move(248, 0, -370);
    move(248, 0, -343);
    move(248, 0, -248);
}

void p544(void)
{
    if (!anyof(343, 367, 248, -1))
        return;
    if (!(bitest(90, 10))) {
        bitmod('s', 90, 10);
        apport(90, value[HERE]);
        set('E', 90, 'c', -1, NULL, NULL);
    }
    move(248, -1);
    longjmp(loop_back, 1);
}

void p447(void)
{
    move(224, value[479], -425);
    move(224, value[479], -179);
}

void p448(void)
{
    if (!anyof(358, 376, 369, 370, 367, 368, 371, 374, 372, 373, 365, -1))
        return;
    say(0, 827, 0);
    move(334, -1);
    coroner();
}

void at_CYLINDRICAL_magicword(void)
{
    if (!anyof(420, 421, 422, 423, 424, 425, 427, 428,
               429, 430, 431, 432, 433, 434, 435, 426, -1))
        return;
    if (KEY(336)) {
        set('V', 448, 'v', 445, NULL, NULL);
    } else {
        if (value[STATUS] == 1) {
            set('V', 448, 'v', 444, NULL, NULL);
        } else {
            set('V', 448, 'c', 0, NULL, NULL);
        }
    }
    value[449] = 420;
    *bitword(449) = -1;
    value[448] -= value[449];
    if (value[448] == value[474]) {
        if (KEY(435)) {
            set('V', 463, 'c', 4, NULL, NULL);
            say(2, 479, 0);
            move(92, -1);
        } else {
            say(0, 484, 0);
            value[474] += 1;
        }
    } else {
        set('V', 474, 'c', 0, NULL, NULL);
        say(0, 538, 0);
    }
    longjmp(loop_back, 1);
}

void action_GET_special(void)
{
    if (((value[69] == 0) || !isnear(69, -1, -1)) && !(bitest(value[HERE], 0))) {
        grope_for_it();
    }
    default_to(0, value[HERE], 0);
    if (!anyof(71, 70, 81, 83, 82, 62, 37, 395, 76, 86, 55, 79, -1))
        return;
    if (KEY(71)) {
        getbird();
    } else {
        if (KEY(70)) {
            getcage();
        } else {
            if (KEY(81)) {
                getbottle();
            } else {
                if (KEY(83)) {
                    getoil();
                } else {
                    if (KEY(82)) {
                        getwater();
                    } else {
                        if (KEY(62)) {
                            getchain();
                        } else {
                            if (KEY(37)) {
                                getbear();
                            } else {
                                if (KEY(395)) {
                                    (*procs[395]) ();
                                } else {
                                    if (KEY(86)) {
                                        getsword();
                                    } else {
                                        if (KEY(55)) {
                                            getsceptre();
                                        } else {
                                            if (KEY(79)) {
                                                getknife();
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void action_GET(void)
{
    if (value[STATUS] == 1) {
        return;
    }
    if (!(bitest(evar(445), 15))) {
        say(0, 608, 0);
        longjmp(loop_back, 1);
    }
    if (have(value[ARG2], -1, -1)) {
        say(0, 517, 0);
        longjmp(loop_back, 1);
    }
    if (!(isnear(value[ARG2], -1, -1))) {
        say(12, 522, 445);
        longjmp(loop_back, 1);
    }
    if (!(bitest(evar(445), 0))) {
        say(0, 608, 0);
        longjmp(loop_back, 1);
    }
    if (value[452] < value[468]) {
        apport(value[ARG2], INHAND);
        say(2, 465, 0);
        if (bitest(evar(445), 4)) {
            value[449] = value[value[ARG2]];
            *bitword(449) = 0;
            if (value[449] == 0) {
                value[value[ARG2]] = 1;
            }
        }
    } else {
        say(0, 590, 0);
    }
    longjmp(loop_back, 1);
}

void action_DROP_special(void)
{
    if (!anyof(71, 70, 81, 83, 82, 52, 88, 26, 37, -1))
        return;
    if (KEY(71)) {
        dropbird();
    } else {
        if (KEY(70)) {
            dropcage();
        } else {
            if (KEY(81)) {
                dropbottle();
            } else {
                if ((KEY(83)) || KEY(82)) {
                    dropliquid();
                } else {
                    if (KEY(52)) {
                        dropvase();
                    } else {
                        if (KEY(88)) {
                            dropvial();
                        } else {
                            if (KEY(26)) {
                                freedjinn();
                            } else {
                                if (KEY(37)) {
                                    dropbear();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return;
}

void action_DROP(void)
{
    if (value[STATUS] == 1) {
        return;
    }
    if (!(bitest(evar(445), 15))) {
        say(0, 608, 0);
        longjmp(loop_back, 1);
    }
    if (have(value[ARG2], -1, -1)) {
        apport(value[ARG2], HERE);
        say(2, 465, 0);
    } else {
        say(0, 521, 0);
    }
    longjmp(loop_back, 1);
}

void action_WALK(void)
{
    if (value[STATUS] == 1) {
        say(12, 579, 444);
    } else {
        if ((((((((((((((KEY(369)) ||
                        KEY(370)) ||
                       KEY(367)) ||
                      KEY(368)) ||
                     KEY(371)) ||
                    KEY(374)) ||
                   KEY(372)) ||
                  KEY(373)) ||
                 KEY(357)) ||
                KEY(358)) ||
               KEY(342)) || KEY(417)) || KEY(343)) || KEY(350)) {
            assert(value[ARG2] < LPROC && value[ARG2] >= 0);
            (*procs[value[ARG2]]) ();
        } else {
            if (bitest(evar(445), 13)) {
                if ((value[HERE] == value[ARG2])) {
                    say(0, 554, 0);
                } else {
                    say(0, 634, 0);
                }
            } else {
                say(0, 506, 0);
            }
        }
    }
    longjmp(loop_back, 1);
}

void action_DIG(void)
{
    say(0, 562, 0);
    longjmp(loop_back, 1);
}

void action_CAVE_outdoors(void)
{
    if (value[HERE] != 95 && value[HERE] != 96 && value[HERE] != 97 &&
        value[HERE] != 92 && value[HERE] != 98 && value[HERE] != 99 &&
        value[HERE] != 94)
        return;
    say(0, 553, 0);
    longjmp(loop_back, 1);
}

void action_CAVE(void)
{
    say(0, 554, 0);
    longjmp(loop_back, 1);
}

void action_WATER_DOOR(void)
{
    if (!KEY(5))
        return;
    if (isnear(5, -1, -1)) {
        if (have(82, -1, -1)) {
            apport(82, LIMBO);
            set('E', 81, 'c', 1, NULL, NULL);
            say(0, 611, 0);
            set('E', 5, 'c', 0, NULL, NULL);
        } else {
            say(12, 717, 444);
        }
    } else {
        say(12, 522, 445);
    }
    longjmp(loop_back, 1);
}

void action_OIL_DOOR(void)
{
    if (!KEY(5))
        return;
    if (isnear(5, -1, -1)) {
        if (have(83, -1, -1)) {
            apport(83, LIMBO);
            set('E', 81, 'c', 1, NULL, NULL);
            set('E', 5, 'c', 1, NULL, NULL);
            say(0, 612, 0);
        } else {
            say(12, 717, 444);
        }
    } else {
        say(12, 522, 445);
    }
    longjmp(loop_back, 1);
}

void action_WATER_PLANT(void)
{
    if (!KEY(10))
        return;
    if (isnear(10, -1, -1)) {
        if (have(82, -1, -1)) {
            apport(82, LIMBO);
            set('E', 81, 'c', 1, NULL, NULL);
            value[10] += 1;
            say(0, 10, 0);
            value[10] += 1;
            if (value[10] == 6) {
                set('E', 10, 'c', 0, NULL, NULL);
                bitmod('s', 11, 7);
            } else {
                bitmod('c', 11, 7);
            }
            move(value[HERE], -1);
            set('E', 11, 'e', 10, NULL, NULL);
        } else {
            say(12, 717, 444);
        }
    } else {
        say(12, 522, 445);
    }
    longjmp(loop_back, 1);
}

void action_OIL_PLANT(void)
{
    if (!KEY(10))
        return;
    if (isnear(10, -1, -1)) {
        if (have(83, -1, -1)) {
            apport(83, LIMBO);
            set('E', 81, 'c', 1, NULL, NULL);
            say(0, 610, 0);
        } else {
            say(12, 717, 444);
        }
    } else {
        say(12, 522, 445);
    }
    longjmp(loop_back, 1);
}

void action_OIL(void)
{
    if (value[STATUS] == 1) {
        if ((((isnear(81, -1, -1)) &&
              value[81] == 2) || have(83, -1, -1)) || (value[HERE] == 113)) {
            say(12, 714, 444);
        } else {
            say(12, 522, 444);
        }
    } else {
        say(0, 608, 0);
    }
    longjmp(loop_back, 1);
}

void action_WATER(void)
{
    if (value[STATUS] == 1) {
        if ((((isnear(81, -1, -1)) &&
              value[81] == 0) || have(82, -1, -1)) || bitest(value[HERE], 6)) {
            say(12, 714, 444);
        } else {
            say(12, 522, 444);
        }
    } else {
        say(0, 608, 0);
    }
    longjmp(loop_back, 1);
}

void action_THROW_weaponry(void)
{
    if (!anyof(85, 86, -1))
        return;
    weaponry();
}

void action_THROW_FOOD(void)
{
    if (!KEY(80))
        return;
    if (!have(80, -1, -1))
        return;
    if (isnear(21, -1, -1)) {
        say(0, 685, 0);
    } else {
        if (isnear(91, -1, -1)) {
            say(0, 601, 0);
            apport(80, HERE);
            bitmod('s', 91, 11);
        } else {
            if (isnear(37, -1, -1)) {
                say(0, 670, 0);
                apport(80, LIMBO);
                set('E', 37, 'c', 1, NULL, NULL);
                if (value[85] == 1) {
                    set('E', 85, 'c', 0, NULL, NULL);
                }
            } else {
                return;
            }
        }
    }
    longjmp(loop_back, 1);
}

void action_THROW_TEETH(void)
{
    if (!KEY(87))
        return;
    if (!have(87, -1, -1))
        return;
    set('E', 87, 'c', 0, NULL, NULL);
    if (!isnear(90, -1, -1))
        return;
    say(0, 842, 0);
    apport(87, LIMBO);
    apport(90, LIMBO);
    longjmp(loop_back, 1);
}

void action_THROW_VIAL(void)
{
    if (!KEY(88))
        return;
    if (!have(88, -1, -1))
        return;
    break_vial();
}

void action_WAVE(void)
{
    if (value[STATUS] > 1) {
        if (!(bitest(evar(445), 15))) {
            say(0, 608, 0);
            longjmp(loop_back, 1);
        }
        if (!(have(value[ARG2], -1, -1))) {
            say(0, 521, 0);
            longjmp(loop_back, 1);
        }
    }
    if (KEY(72)) {
        if (isnear(7, -1, -1)) {
            if (value[463] < 2) {
                value[7] += 1;
                say(0, 7, 0);
                if (value[7] == 2) {
                    set('E', 7, 'c', 0, NULL, NULL);
                    bitmod('s', 7, 7);
                } else {
                    bitmod('c', 7, 7);
                }
            } else {
                say(0, 538, 0);
            }
        } else {
            if (isnear(31, -1, -1)) {
                say(0, 772, 0);
                set('E', 31, 'c', 1, NULL, NULL);
            } else {
                if ((isnear(29, -1, -1)) && value[463] < 2) {
                    value[29] += 1;
                    say(0, 29, 0);
                    value[29] += 1;
                    if (value[29] == 4) {
                        set('E', 29, 'c', 0, NULL, NULL);
                        bitmod('s', 29, 7);
                    } else {
                        bitmod('c', 29, 7);
                    }
                } else {
                    say(0, 538, 0);
                }
            }
        }
    } else {
        if ((KEY(85)) || KEY(86)) {
            weaponry();
        } else {
            say(0, 538, 0);
        }
    }
    longjmp(loop_back, 1);
}

void action_THROW(void)
{
    if (value[STATUS] == 2) {
        if (bitest(evar(445), 15)) {
            if ((have(value[ARG2], -1, -1)) && bitest(value[HERE], 9)) {
                upchuck();
            }
        }
    }
    (*procs[379]) ();
}

void action_NORTH(void)
{
    no_move_possible();
    longjmp(loop_back, 1);
}

void action_SOUTH(void)
{
    no_move_possible();
    longjmp(loop_back, 1);
}

void action_UP(void)
{
    no_move_possible();
    longjmp(loop_back, 1);
}

void action_DOWN(void)
{
    no_move_possible();
    longjmp(loop_back, 1);
}

void action_EAST(void)
{
    no_move_possible();
    longjmp(loop_back, 1);
}

void action_WEST(void)
{
    no_move_possible();
    longjmp(loop_back, 1);
}

void action_NORTHEAST(void)
{
    no_move_possible();
    longjmp(loop_back, 1);
}

void action_NORTHWEST(void)
{
    no_move_possible();
    longjmp(loop_back, 1);
}

void action_SOUTHEAST(void)
{
    no_move_possible();
    longjmp(loop_back, 1);
}

void action_SOUTHWEST(void)
{
    no_move_possible();
    longjmp(loop_back, 1);
}

void action_INVENTORY(void)
{
    set('V', 449, 'c', 0, NULL, NULL);
    *bitword(448) = -1;
    value[448] = FOBJ - 1;
    while (++value[448] <= LOBJ) {
        if (have(value[448], -1, -1)) {
            if (value[449] == 0) {
                say(0, 597, 0);
                set('V', 449, 'c', 1, NULL, NULL);
            }
            say(2, 448, 0);
        }
    }
    if (value[449] == 0) {
        say(0, 596, 0);
    }
    longjmp(loop_back, 1);
}

void action_LOOK_scenery(void)
{
    if (!anyof(410, 407, -1))
        return;
    assert(value[ARG2] < LPROC && value[ARG2] >= 0);
    (*procs[value[ARG2]]) ();
}

void action_LOOK(void)
{
    if (((isnear(69, -1, -1)) && value[69] == 1) || bitest(value[HERE], 0)) {
        set('V', 449, 'c', 0, NULL, NULL);
        if (!(bitest(evar(446), 2))) {
            bitmod('s', evar(446), 3);
            set('V', 449, 'c', 1, NULL, NULL);
        }
        say(2, 442, 0);
        if (bitest(value[HERE], 12)) {
            say(0, 576, 0);
        }
        bitmod('c', evar(446), 3);
        bitmod('s', value[HERE], 1);
        *bitword(448) = -1;
        value[448] = FOBJ - 1;
        while (++value[448] <= LOBJ) {
            if ((isnear(value[448], -1, -1)) && !have(value[448], -1, -1)) {
                if ((value[449] == 1) && !(bitest(evar(448), 7))) {
                    set('V', 449, 'c', 0, NULL, NULL);
                    say(0, 483, 0);
                }
                bitmod('s', evar(448), 1);
                say(2, 448, 0);
            }
        }
        if (have(37, -1, -1)) {
            say(0, 636, 0);
        }
    } else {
        say(0, 532, 0);
    }
    longjmp(loop_back, 1);
}

void action_LIGHT(void)
{
    if ((KEY(69)) || value[STATUS] == 1) {
        if (isnear(69, -1, -1)) {
            if (value[69] == 1) {
                say(0, 535, 0);
                longjmp(loop_back, 1);
            }
            if (value[447] < 40) {
                if ((isnear(38, -1, -1)) && value[38] == 0) {
                    say(0, 743, 0);
                    set('E', 38, 'c', 1, NULL, NULL);
                    value[447] += 300;
                    bitmod('c', 69, 10);
                }
            }
            if (value[447] > 0) {
                set('E', 69, 'c', 1, NULL, NULL);
                say(0, 533, 0);
                if (!(bitest(value[HERE], 0))) {
                    (*procs[377]) ();
                }
                phog();
            } else {
                say(0, 687, 0);
            }
        } else {
            if (KEY(69)) {
                say(12, 522, 445);
            } else {
                say(0, 532, 0);
            }
        }
        longjmp(loop_back, 1);
    }
}

void action_EXTINGUISH(void)
{
    if ((KEY(69)) || value[STATUS] == 1) {
        if (isnear(69, -1, -1)) {
            if (value[69] == 0) {
                say(0, 536, 0);
                longjmp(loop_back, 1);
            }
            set('E', 69, 'c', 0, NULL, NULL);
            say(0, 534, 0);
            if (!(bitest(value[HERE], 0))) {
                say(0, 509, 0);
            }
            phog();
        } else {
            if (KEY(69)) {
                say(12, 522, 445);
            } else {
                return;
            }
        }
    } else {
        say(0, 642, 0);
    }
    longjmp(loop_back, 1);
}

void action_OPEN_grate(void)
{
    default_to(0, value[HERE], 6);
    if (!keyword(3, -1))
        return;
    if (!isnear(3, -1, -1))
        return;
    if (have(68, -1, -1)) {
        if ((value[463] > 1) || bitest(evar(458), 3)) {
            say(0, 530, 0);
            if (value[463] > 1) {
                bitmod('s', evar(458), 4);
                if (!(bitest(3, 10))) {
                    bitmod('s', 3, 10);
                    say(0, 629, 0);
                }
            }
        } else {
            set('E', 3, 'c', 1, NULL, NULL);
            say(0, 529, 0);
            bitmod('c', 99, 5);
        }
    } else {
        say(0, 524, 0);
    }
    longjmp(loop_back, 1);
}

void action_OPEN_CHAIN(void)
{
    if (!KEY(62))
        return;
    if (!isnear(62, -1, -1))
        return;
    if (have(68, -1, -1)) {
        if (value[62] == 0) {
            say(0, 673, 0);
        } else {
            if (value[62] == 1) {
                if (value[37] == 0) {
                    say(0, 537, 0);
                } else {
                    say(0, 673, 0);
                    set('E', 62, 'c', 0, NULL, NULL);
                    set('E', 37, 'c', 2, NULL, NULL);
                }
            } else {
                say(0, 673, 0);
                set('E', 62, 'c', 0, NULL, NULL);
            }
        }
    } else {
        say(0, 524, 0);
    }
    longjmp(loop_back, 1);
}

void action_OPEN_DOOR(void)
{
    if (!KEY(5))
        return;
    if (!isnear(5, -1, -1))
        return;
    say(0, 525, 0);
    longjmp(loop_back, 1);
}

void action_OPEN_KEYS(void)
{
    if (!KEY(68))
        return;
    say(0, 551, 0);
    longjmp(loop_back, 1);
}

void action_OPEN_SAFE(void)
{
    if (!KEY(34))
        return;
    if (!isnear(34, -1, -1))
        return;
    if (value[34] == 0) {
        say(0, 817, 0);
    } else {
        if (value[34] == 1) {
            say(12, 818, 445);
        } else {
            say(0, 819, 0);
        }
    }
    longjmp(loop_back, 1);
}

void action_OPEN_CLAM(void)
{
    if (!KEY(76))
        return;
    if (!isnear(76, -1, -1))
        return;
    if (have(76, -1, -1)) {
        say(0, 618, 0);
    } else {
        if (have(50, -1, -1)) {
            apport(76, LIMBO);
            apport(77, value[HERE]);
            apport(58, 184);
            say(0, 622, 0);
        } else {
            say(0, 620, 0);
        }
    }
    longjmp(loop_back, 1);
}

void action_OPEN_OYSTER(void)
{
    if (!KEY(77))
        return;
    if (!isnear(77, -1, -1))
        return;
    if (have(77, -1, -1)) {
        say(0, 619, 0);
    } else {
        if (have(50, -1, -1)) {
            say(0, 623, 0);
        } else {
            say(0, 621, 0);
        }
    }
    longjmp(loop_back, 1);
}

void action_OPEN_VIAL(void)
{
    if (!KEY(88))
        return;
    if (!isnear(88, -1, -1))
        return;
    break_vial();
}

void action_OPEN_FLASK(void)
{
    if (!KEY(84))
        return;
    if (!isnear(84, -1, -1))
        return;
    if (value[84] < 2) {
        if ((location[84] == 214)) {
            apport(26, 214);
            say(0, 843, 0);
        } else {
            say(0, 844, 0);
        }
        set('E', 84, 'c', 2, NULL, NULL);
    } else {
        say(12, 818, 445);
    }
    longjmp(loop_back, 1);
}

void action_OPEN_PENTAGRAM(void)
{
    if (!KEY(214))
        return;
    if ((value[HERE] == 214)) {
        if (isnear(26, -1, -1)) {
            say(0, 845, 0);
            apport(26, LIMBO);
            bitmod('s', 26, 10);
        } else {
            say(0, 847, 0);
        }
    } else {
        say(12, 522, 445);
    }
    longjmp(loop_back, 1);
}

void action_CLOSE_grate(void)
{
    default_to(0, value[HERE], 6);
    if (!keyword(3, -1))
        return;
    if (!isnear(3, -1, -1))
        return;
    set('E', 3, 'c', 0, NULL, NULL);
    say(0, 528, 0);
    longjmp(loop_back, 1);
}

void action_CLOSE_CHAIN(void)
{
    if (!KEY(62))
        return;
    if (!isnear(62, -1, -1))
        return;
    if ((value[HERE] == 254)) {
        if (value[62] == 0) {
            if (isnear(37, -1, -1)) {
                apport(37, HERE);
                set('E', 37, 'c', 1, NULL, NULL);
                set('E', 62, 'c', 1, NULL, NULL);
            } else {
                set('E', 62, 'c', 2, NULL, NULL);
            }
        }
        say(0, 674, 0);
        if (have(62, -1, -1)) {
            apport(62, HERE);
        }
    } else {
        say(0, 675, 0);
    }
    longjmp(loop_back, 1);
}

void action_CLOSE_DOOR(void)
{
    if (!KEY(5))
        return;
    if (!isnear(5, -1, -1))
        return;
    say(0, 525, 0);
    longjmp(loop_back, 1);
}

void action_CLOSE_SAFE(void)
{
    if (!KEY(34))
        return;
    if (!isnear(34, -1, -1))
        return;
    if (value[34] == 1) {
        say(0, 821, 0);
        set('E', 34, 'c', 0, NULL, NULL);
    } else {
        say(12, 820, 445);
    }
    longjmp(loop_back, 1);
}

void action_CLOSE_FLASK(void)
{
    if (!KEY(84))
        return;
    if (!isnear(84, -1, -1))
        return;
    if (value[84] == 0) {
        say(12, 820, 445);
    } else {
        say(12, 744, 445);
    }
    longjmp(loop_back, 1);
}
