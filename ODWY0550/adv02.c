#include "adv0.h"
#include "adv3.h"
#ifdef __STDC__
void p229(void)
#else
void p229()
#endif
{
    if ((value[464] == 0) || value[463] > 2) {
        set('V', 455, 'c', 9, NULL, NULL);
    } else {
        set('V', 455, 'c', 0, NULL, NULL);
    }
    set('V', 456, 'c', 9, NULL, NULL);
    *bitword(448) = -1;
    value[448] = FOBJ - 1;
    while (++value[448] <= LOBJ) {
        if (bitest(evar(448), 2)) {
            if (((location[value[448]] == 94)) || value[463] > 2) {
                value[455] += 15;
            } else {
                if (bitest(evar(448), 1)) {
                    value[455] += 2;
                }
            }
            value[456] += 15;
        }
    }
    if ((location[78] == 187)) {
        value[455] += 1;
    }
    value[456] += 1;
    if ((bitest(102, 1)) || bitest(118, 1)) {
        value[455] += 20;
        if (bitest(278, 1)) {
            value[455] += 10;
        }
        if (bitest(331, 1)) {
            value[455] += 10;
        }
        if (bitest(208, 1)) {
            value[455] += 10;
        }
    }
    value[456] += 50;
    set('V', 448, 'v', 463, NULL, NULL);
    value[448] *= 20;
    value[455] += value[448];
    value[456] += 100;
    set('V', 451, 'v', 453, NULL, NULL);
    value[451] *= 10;
    value[455] -= value[451];
    value[455] -= value[454];
    if (value[455] < 0) {
        set('V', 455, 'c', 0, NULL, NULL);
    }
}

#ifdef __STDC__
void p204(void)
#else
void p204()
#endif
{
    p229();
    say(13, 723, 455);
    say(13, 724, 456);
    say(13, 725, 459);
    if (value[455] < 20) {
        value[448] = 732;
        *bitword(448) = -1;
        value[455] -= 20;
    } else {
        if (value[455] < 130) {
            value[448] = 733;
            *bitword(448) = -1;
            value[455] -= 130;
        } else {
            if (value[455] < 240) {
                value[448] = 734;
                *bitword(448) = -1;
                value[455] -= 240;
            } else {
                if (value[455] < 350) {
                    value[448] = 735;
                    *bitword(448) = -1;
                    value[455] -= 350;
                } else {
                    if (value[455] < 470) {
                        value[448] = 736;
                        *bitword(448) = -1;
                        value[455] -= 470;
                    } else {
                        if (value[455] < 510) {
                            value[448] = 737;
                            *bitword(448) = -1;
                            value[455] -= 510;
                        } else {
                            if (value[455] < 530) {
                                value[448] = 738;
                                *bitword(448) = -1;
                                value[455] -= 530;
                            } else {
                                if (value[455] < 550) {
                                    value[448] = 739;
                                    *bitword(448) = -1;
                                    value[455] -= 550;
                                } else {
                                    value[448] = 740;
                                    *bitword(448) = -1;
                                    value[455] = 0;
                                    *bitword(455) = -1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    say(2, 448, 0);
    say(0, 483, 0);
    value[455] *= -1;
    if (value[455] > 0) {
        if (value[455] == 1) {
            say(0, 741, 0);
        } else {
            say(13, 742, 455);
        }
    }
    say(0, 483, 0);
    finita();
}

#ifdef __STDC__
void phog(void)
#else
void phog()
#endif
{
    if (value[13] < 8) {
        value[13] = irand(8);
    }
    value[14] = irand(8);
    if ((isnear(69, -1, -1)) && value[69] == 1) {
        apport(14, LIMBO);
    } else {
        apport(14, 211);
        if (((value[HERE] == 211)) && !(bitest(evar(446), 0))) {
            say(0, 14, 0);
        }
    }
}

#ifdef __STDC__
void p124(void)
#else
void p124()
#endif
{
    set('V', 464, 'c', 0, NULL, NULL);
    say(0, 483, 0);
    bitmod('c', evar(458), 3);
    bitmod('c', evar(458), 1);
    set('E', 16, 'c', 0, NULL, NULL);
    apport(16, LIMBO);
    apport(90, LIMBO);
    if (value[24] == 1) {
        set('E', 24, 'c', 0, NULL, NULL);
    } else {
        if (value[24] == 3) {
            set('E', 24, 'c', 2, NULL, NULL);
        }
    }
    apport(13, 210);
    set('E', 13, 'c', 8, NULL, NULL);
    value[453] += 1;
    if (value[463] > 1) {
        if (value[463] == 2) {
            say(0, 630, 0);
        } else {
            value[453] -= 1;
        }
        p204();
    }
    value[448] = 578;
    *bitword(448) = -1;
    value[448] += value[453];
    value[448] += value[453];
    say(2, 448, 0);
    if (query(482)) {
        value[448] += 1;
        say(2, 448, 0);
        say(0, 483, 0);
        value[449] = 587;
        *bitword(449) = -1;
        if (value[448] < value[449]) {
            if (have(52, -1, -1)) {
                apport(52, LIMBO);
                apport(53, INHAND);
            }
            *bitword(448) = -1;
            value[448] = FOBJ - 1;
            while (++value[448] <= LOBJ) {
                if (have(value[448], -1, -1)) {
                    apport(value[448], HERE);
                }
            }
            apport(82, LIMBO);
            apport(83, LIMBO);
            set('V', 452, 'c', 0, NULL, NULL);
            set('E', 69, 'c', 0, NULL, NULL);
            apport(13, 210);
            set('E', 13, 'c', 8, NULL, NULL);
            phog();
            move(94, -1);
            set('V', 443, 'c', 0, NULL, NULL);
            apport(69, 92);
            if (value[447] == 0) {
                if ((bitest(69, 10)) || !(bitest(278, 1))) {
                    value[448] = location[38];
                    *bitword(448) = -1;
                    if ((value[38] == 1) || !(bitest(evar(448), 4))) {
                        apport(69, 334);
                    }
                }
            }
            apport(91, LIMBO);
            set('E', 91, 'c', 0, NULL, NULL);
            bitmod('c', 18, 10);
            longjmp(loop_back, 1);
        } else {
            p204();
        }
    } else {
        p204();
    }
    finita();
}

#ifdef __STDC__
void p222(void)
#else
void p222()
#endif
{
    if (!isnear(71, -1, -1))
        return;
    if (have(71, -1, -1)) {
        say(0, 517, 0);
        longjmp(loop_back, 1);
    }
    if (value[71] == 1) {
        apport(70, INHAND);
        apport(71, INHAND);
        say(2, 465, 0);
    } else {
        if (have(70, -1, -1)) {
            if (have(72, -1, -1)) {
                say(0, 519, 0);
            } else {
                bitmod('c', 104, 5);
                apport(71, INHAND);
                set('E', 71, 'c', 1, NULL, NULL);
                say(2, 465, 0);
            }
        } else {
            say(0, 520, 0);
        }
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p224(void)
#else
void p224()
#endif
{
    if (!isnear(70, -1, -1))
        return;
    if (have(70, -1, -1)) {
        say(0, 517, 0);
        longjmp(loop_back, 1);
    }
    apport(70, INHAND);
    if (isnear(71, -1, -1)) {
        if (value[71] == 1) {
            apport(71, INHAND);
        }
    }
    say(2, 465, 0);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p226(void)
#else
void p226()
#endif
{
    if (bitest(85, 1)) {
        say(0, 614, 0);
        longjmp(loop_back, 1);
    }
}

#ifdef __STDC__
void p159(void)
#else
void p159()
#endif
{
    if (!have(71, -1, -1))
        return;
    apport(71, HERE);
    set('E', 71, 'c', 0, NULL, NULL);
    if (isnear(6, -1, -1)) {
        say(0, 523, 0);
        apport(6, LIMBO);
        bitmod('c', 110, 5);
        longjmp(loop_back, 1);
    } else {
        if (isnear(19, -1, -1)) {
            if (value[19] == 0) {
                apport(71, LIMBO);
                say(0, 650, 0);
                longjmp(loop_back, 1);
            }
        } else {
            if (isnear(21, -1, -1)) {
                say(0, 651, 0);
                longjmp(loop_back, 1);
            } else {
                if (isnear(23, -1, -1)) {
                    say(0, 652, 0);
                    apport(71, LIMBO);
                    longjmp(loop_back, 1);
                } else {
                    if (isnear(37, -1, -1)) {
                        if (value[37] == 0) {
                            say(0, 653, 0);
                            apport(71, LIMBO);
                        } else {
                            say(0, 654, 0);
                        }
                        longjmp(loop_back, 1);
                    } else {
                        if ((isnear(24, -1, -1)) && value[24] < 2) {
                            say(0, 655, 0);
                            apport(71, LIMBO);
                            p124();
                        } else {
                            if (isnear(91, -1, -1)) {
                                say(12, 656, 91);
                                apport(71, LIMBO);
                                longjmp(loop_back, 1);
                            }
                        }
                    }
                }
            }
        }
    }
    say(2, 465, 0);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p161(void)
#else
void p161()
#endif
{
    if (!have(70, -1, -1))
        return;
    apport(70, HERE);
    if (have(71, -1, -1)) {
        apport(71, HERE);
    }
    say(2, 465, 0);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p163(void)
#else
void p163()
#endif
{
    if (!have(52, -1, -1))
        return;
    apport(52, HERE);
    if ((value[HERE] == 175)) {
        say(2, 465, 0);
    } else {
        if ((have(75, -1, -1)) || !isnear(75, -1, -1)) {
            set('E', 52, 'c', 2, NULL, NULL);
            say(0, 52, 0);
            apport(52, LIMBO);
            apport(53, value[442]);
        } else {
            set('E', 52, 'c', 1, NULL, NULL);
            say(0, 52, 0);
            set('E', 52, 'c', 0, NULL, NULL);
        }
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p162(void)
#else
void p162()
#endif
{
    if (!have(value[445], -1, -1))
        return;
    apport(value[445], LIMBO);
    set('E', 81, 'c', 1, NULL, NULL);
    if ((isnear(91, -1, -1)) && KEY(392)) {
        if (value[91] == 1) {
            say(12, 752, 445);
        } else {
            say(12, 753, 445);
        }
        bitmod('s', 91, 11);
    } else {
        say(0, 575, 0);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p160(void)
#else
void p160()
#endif
{
    if (!have(81, -1, -1))
        return;
    apport(81, HERE);
    apport(83, LIMBO);
    apport(82, LIMBO);
    say(2, 465, 0);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p223(void)
#else
void p223()
#endif
{
    if (!isnear(81, -1, -1))
        return;
    if (have(81, -1, -1)) {
        say(0, 517, 0);
        longjmp(loop_back, 1);
    }
    if (value[452] < value[468]) {
        apport(81, INHAND);
        if (value[81] == 0) {
            apport(82, INHAND);
        } else {
            if (value[81] == 2) {
                apport(83, INHAND);
            }
        }
        say(2, 465, 0);
    } else {
        say(0, 590, 0);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p227(void)
#else
void p227()
#endif
{
    if (value[HERE] != 113)
        return;
    if (have(81, -1, -1)) {
        if (value[81] == 1) {
            set('E', 81, 'c', 2, NULL, NULL);
            apport(83, INHAND);
            say(0, 606, 0);
        } else {
            say(0, 603, 0);
        }
    } else {
        say(0, 602, 0);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p231(void)
#else
void p231()
#endif
{
    if (!(bitest(evar(442), 6))) {
        return;
    }
    if (have(81, -1, -1)) {
        if (value[81] == 1) {
            set('E', 81, 'c', 0, NULL, NULL);
            apport(82, INHAND);
            say(0, 605, 0);
        } else {
            say(0, 603, 0);
        }
    } else {
        say(0, 602, 0);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p314(void)
#else
void p314()
#endif
{
    say(0, 659, 0);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p304(void)
#else
void p304()
#endif
{
    if (value[37] == 0) {
        say(0, 667, 0);
    } else {
        say(0, 668, 0);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p313(void)
#else
void p313()
#endif
{
    say(0, 542, 0);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p309(void)
#else
void p309()
#endif
{
    if (value[19] > 0) {
        say(0, 669, 0);
        longjmp(loop_back, 1);
    }
    if (query(546)) {
        set('E', 19, 'c', 1, NULL, NULL);
        say(0, 19, 0);
        set('E', 19, 'c', 2, NULL, NULL);
        apport(59, 202);
        apport(87, 202);
        bitmod('c', 19, 3);
        *bitword(448) = -1;
        value[448] = FOBJ - 1;
        while (++value[448] <= LOBJ) {
            if (isnear(value[448], -1, -1)) {
                if (!(have(value[448], -1, -1))) {
                    apport(value[448], 202);
                }
            }
        }
        move(202, -1);
    } else {
        say(12, 909, 444);
        say(12, 910, 445);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p305(void)
#else
void p305()
#endif
{
    if (value[463] < 3) {
        apport(71, LIMBO);
        say(0, 541, 0);
    } else {
        say(0, 632, 0);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p306(void)
#else
void p306()
#endif
{
    say(0, 646, 0);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p310(void)
#else
void p310()
#endif
{
    if (query(546)) {
        set('V', 448, 'v', 468, NULL, NULL);
        value[448] -= value[452];
        value[448] += 2;
        value[448] *= 10;
        if (pct(value[448])) {
            say(0, 543, 0);
            value[91] -= 1;
            if (value[91] == 0) {
                apport(91, LIMBO);
            }
            value[461] -= 1;
        } else {
            if (pct(value[448])) {
                say(0, 544, 0);
            } else {
                say(0, 545, 0);
                p124();
            }
        }
    } else {
        say(12, 909, 444);
        say(12, 910, 445);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p312(void)
#else
void p312()
#endif
{
    if (query(546)) {
        if (pct(50)) {
            say(0, 809, 0);
        } else {
            say(0, 810, 0);
            p124();
        }
    } else {
        say(12, 909, 444);
        say(12, 910, 445);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p307(void)
#else
void p307()
#endif
{
    say(0, 829, 0);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p308(void)
#else
void p308()
#endif
{
    say(0, 840, 0);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p311(void)
#else
void p311()
#endif
{
    say(0, 846, 0);
    p124();
}

#ifdef __STDC__
void p303(void)
#else
void p303()
#endif
{
    if (value[24] < 2) {
        say(0, 836, 0);
        p124();
    } else {
        say(0, 669, 0);
        longjmp(loop_back, 1);
    }
}

#ifdef __STDC__
void p243(void)
#else
void p243()
#endif
{
    if (isnear(27, -1, -1)) {
        say(0, 855, 0);
    } else {
        say(0, 856, 0);
        apport(27, value[442]);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p221(void)
#else
void p221()
#endif
{
    if (!isnear(37, -1, -1))
        return;
    if (have(37, -1, -1)) {
        say(0, 636, 0);
    } else {
        if ((value[HERE] == 254)) {
            if (value[37] == 2) {
                say(2, 465, 0);
                apport(37, INHAND);
            } else {
                say(0, 671, 0);
            }
        } else {
            say(2, 465, 0);
            apport(37, INHAND);
        }
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p158(void)
#else
void p158()
#endif
{
    if (!have(37, -1, -1))
        return;
    apport(37, HERE);
    say(2, 465, 0);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p214(void)
#else
void p214()
#endif
{
    if (!isnear(26, -1, -1))
        return;
    say(0, 845, 0);
    apport(26, LIMBO);
    bitmod('s', 26, 10);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p225(void)
#else
void p225()
#endif
{
    if (value[HERE] != 254)
        return;
    if (!isnear(62, -1, -1))
        return;
    if (value[62] == 0) {
        return;
    } else {
        say(0, 672, 0);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p230(void)
#else
void p230()
#endif
{
    if (!isnear(86, -1, -1))
        return;
    if (((value[86] == 0) && value[452] < value[468]) && !have(86, -1, -1)) {
        if (value[89] == 2) {
            say(0, 780, 0);
            set('E', 86, 'c', 1, NULL, NULL);
            apport(86, INHAND);
        } else {
            say(0, 781, 0);
        }
        longjmp(loop_back, 1);
    }
}

#ifdef __STDC__
void p228(void)
#else
void p228()
#endif
{
    if (!isnear(55, -1, -1))
        return;
    if ((value[55] == 0) && value[452] < value[468]) {
        apport(55, INHAND);
        set('E', 55, 'c', 1, NULL, NULL);
        value[472] = irand(5);
        if (value[472] == 0) {
            value[472] = 435;
            *bitword(472) = -1;
        } else {
            if (value[472] == 1) {
                value[472] = 430;
                *bitword(472) = -1;
            } else {
                if (value[472] == 2) {
                    value[472] = 429;
                    *bitword(472) = -1;
                } else {
                    if (value[472] == 3) {
                        value[472] = 423;
                        *bitword(472) = -1;
                    } else {
                        value[472] = 420;
                        *bitword(472) = -1;
                    }
                }
            }
        }
        if (value[34] == 0) {
            say(12, 823, 472);
        } else {
            say(0, 824, 0);
        }
        apport(36, LIMBO);
        longjmp(loop_back, 1);
    }
}

#ifdef __STDC__
void p517(void)
#else
void p517()
#endif
{
    value[448] = 709;
    *bitword(448) = -1;
    value[448] += value[453];
    say(2, 448, 0);
    p124();
}

#ifdef __STDC__
void p152(void)
#else
void p152()
#endif
{
    set('V', 475, 'c', 0, NULL, NULL);
    if ((((((((((((bitest(evar(442), 4)) ||
                  bitest(evar(442), 2)) ||
                 bitest(evar(442), 0)) ||
                bitest(evar(442), 8)) ||
               !have(69, -1, -1)) ||
              value[69] == 0) ||
             bitest(18, 10)) ||
            isnear(91, -1, -1)) ||
           isnear(19, -1, -1)) ||
          isnear(21, -1, -1)) || isnear(6, -1, -1)) || isnear(31, -1, -1)) {
        return;
    }
    value[448] = 881;
    *bitword(448) = -1;
    value[449] = irand(7);
    value[448] += value[449];
    say(0, 483, 0);
    say(2, 448, 0);
    say(0, 483, 0);
}

#ifdef __STDC__
void p121(void)
#else
void p121()
#endif
{
    say(0, 631, 0);
    bitmod('c', evar(446), 2);
    bitmod('c', evar(446), 1);
    *bitword(448) = -1;
    value[448] = FOBJ - 1;
    while (++value[448] <= LOBJ) {
        value[449] = location[value[448]];
        *bitword(449) = -1;
        if (((bitest(evar(449), 4)) ||
             have(value[448], -1, -1)) && bitest(evar(448), 0)) {
            apport(value[448], 334);
        }
    }
    *bitword(448) = -1;
    value[448] = FLOC - 1;
    while (++value[448] <= LLOC) {
        if (bitest(evar(448), 4)) {
            bitmod('c', evar(448), 1);
        }
    }
    set('V', 463, 'c', 3, NULL, NULL);
    set('V', 462, 'c', 999, NULL, NULL);
    move(196, -1);
    set('V', 474, 'c', 0, NULL, NULL);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p113(void)
#else
void p113()
#endif
{
    if (value[463] == 0) {
        set('V', 463, 'c', 1, NULL, NULL);
        *bitword(448) = -1;
        value[448] = FOBJ - 1;
        while (++value[448] <= LOBJ) {
            if ((bitest(evar(448), 2)) && !(location[value[448]] == 94)) {
                set('V', 463, 'c', 0, NULL, NULL);
            }
        }
        if (value[463] == 1) {
            set('V', 462, 'c', 35, NULL, NULL);
        } else {
            value[462] = irand(10);
            value[462] += 30;
        }
        if (value[65] > 0) {
            value[65] = irand(14);
            value[65] += 1;
        }
        if (value[86] > 0) {
            value[86] = irand(11);
            value[86] += 1;
        }
        if (value[19] == 2) {
            value[470] -= value[471];
            if (value[470] < 0) {
                set('E', 19, 'c', 3, NULL, NULL);
            }
        }
        if (((bitest(26, 10)) && !(bitest(26, 11))) && !isnear(91, -1, -1)) {
            bitmod('s', 26, 11);
            say(0, 863, 0);
            set('V', 462, 'c', 5, NULL, NULL);
            set('V', 471, 'v', 462, NULL, NULL);
            return;
        }
        if (value[89] > 1) {
            value[469] -= value[471];
            if (value[469] < 0) {
                if (value[89] == 2) {
                    set('E', 89, 'c', 3, NULL, NULL);
                    set('V', 469, 'c', 40, NULL, NULL);
                    say(0, 89, 0);
                    set('V', 468, 'c', 7, NULL, NULL);
                    set('V', 462, 'c', 8, NULL, NULL);
                    set('V', 471, 'v', 462, NULL, NULL);
                    return;
                } else {
                    set('E', 89, 'c', 0, NULL, NULL);
                    apport(89, 274);
                }
            }
        }
        if ((value[475] > 0) && value[475] < value[459]) {
            value[462] = irand(10);
            value[462] += 10;
            p152();
            set('V', 471, 'v', 462, NULL, NULL);
            return;
        }
        if ((bitest(106, 1)) || bitest(118, 1)) {
            if ((bitest(evar(442), 4)) || bitest(evar(442), 2)) {
                value[462] = irand(10);
                value[462] += 8;
            } else {
                if (((((((((value[459] > 150) &&
                           !(bitest(18, 1))) ||
                          bitest(18, 10)) ||
                         pct(15)) &&
                        !(bitest(48, 1))) &&
                       !isnear(91, -1, -1)) &&
                      !(bitest(evar(442), 0))) &&
                     have(69, -1, -1)) && value[69] > 0) {
                    bitmod('c', 18, 10);
                    set('V', 449, 'c', 0, NULL, NULL);
                    bitmod('c', 63, 2);
                    *bitword(448) = -1;
                    value[448] = FOBJ - 1;
                    while (++value[448] <= LOBJ) {
                        if ((bitest(evar(448), 2)) &&
                            isnear(value[448], -1, -1)) {
                            apport(value[448], 198);
                            set('V', 449, 'c', 1, NULL, NULL);
                        }
                    }
                    bitmod('s', 63, 2);
                    if (value[449] == 0) {
                        if ((!(bitest(18, 1))) && value[459] > 150) {
                            apport(48, 198);
                            apport(28, 264);
                            say(0, 689, 0);
                            bitmod('s', 18, 1);
                        } else {
                            say(0, 625, 0);
                            bitmod('s', 18, 10);
                            value[462] = irand(10);
                            value[462] += 4;
                        }
                    } else {
                        if (bitest(18, 1)) {
                            say(0, 627, 0);
                        } else {
                            say(0, 626, 0);
                            bitmod('s', 18, 1);
                            apport(48, 198);
                            apport(28, 264);
                        }
                    }
                } else {
                    set('V', 448, 'v', 461, NULL, NULL);
                    value[448] += 2;
                    value[448] *= 10;
                    if (((isnear(91, -1, -1)) ||
                         pct(value[448])) && value[91] < value[461]) {
                        if (bitest(85, 1)) {
                            apport(91, value[442]);
                            value[91] += 1;
                            if (value[91] == 1) {
                                say(0, 91, 0);
                                bitmod('s', 91, 10);
                                bitmod('c', 91, 11);
                            }
                        } else {
                            apport(85, value[442]);
                            bitmod('s', 85, 1);
                            say(0, 489, 0);
                        }
                    }
                }
            }
        }
    } else {
        if (value[463] == 1) {
            set('V', 463, 'c', 2, NULL, NULL);
            set('E', 3, 'c', 0, NULL, NULL);
            say(0, 628, 0);
            if (isnear(91, -1, -1)) {
                say(13, 728, 91);
            }
            apport(91, LIMBO);
            set('E', 91, 'c', 0, NULL, NULL);
            set('V', 461, 'c', 0, NULL, NULL);
            set('E', 7, 'c', 0, NULL, NULL);
            set('E', 29, 'c', 0, NULL, NULL);
            apport(21, LIMBO);
            apport(19, LIMBO);
            set('E', 21, 'c', 5, NULL, NULL);
            apport(22, 199);
            bitmod('s', 7, 7);
            bitmod('s', 29, 7);
            set('V', 462, 'c', 25, NULL, NULL);
        } else {
            if (bitest(evar(458), 4)) {
                bitmod('c', evar(458), 4);
                set('V', 462, 'c', 15, NULL, NULL);
            } else {
                p121();
            }
        }
    }
    set('V', 471, 'v', 462, NULL, NULL);
}

#ifdef __STDC__
void p42(void)
#else
void p42()
#endif
{
    if (value[446] == 1) {
        say(12, 727, 444);
        bitmod('s', evar(446), 4);
    } else {
        if ((value[446] == 2) && bitest(evar(445), 15)) {
            if (isnear(value[445], -1, -1)) {
                say(12, 744, 444);
            } else {
                say(12, 522, 445);
            }
        } else {
            say(12, 744, 444);
        }
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p318(void)
#else
void p318()
#endif
{
    if (value[447] > 0) {
        if (value[38] == 1) {
            say(0, 692, 0);
        } else {
            if (isnear(38, -1, -1)) {
                say(0, 691, 0);
                set('E', 38, 'c', 1, NULL, NULL);
                value[447] += 300;
                bitmod('c', 69, 10);
            } else {
                if (bitest(38, 1)) {
                    say(0, 690, 0);
                } else {
                    say(0, 686, 0);
                }
            }
        }
    } else {
        if (value[463] == 2) {
            p121();
        } else {
            if ((isnear(38, -1, -1)) && value[38] == 0) {
                say(0, 691, 0);
                set('E', 38, 'c', 1, NULL, NULL);
                value[447] += 300;
            } else {
                say(0, 687, 0);
                set('E', 69, 'c', 0, NULL, NULL);
                bitmod('s', evar(458), 6);
                phog();
            }
        }
    }
}

#ifdef __STDC__
void p463(void)
#else
void p463()
#endif
{
    say(0, 693, 0);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p465(void)
#else
void p465()
#endif
{
    say(0, 700, 0);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p464(void)
#else
void p464()
#endif
{
    say(0, 694, 0);
}

#ifdef __STDC__
void p462(void)
#else
void p462()
#endif
{
    say(0, 698, 0);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p242(void)
#else
void p242()
#endif
{
    set('V', 448, 'c', 0, NULL, NULL);
    if ((value[HERE] == 99)) {
        if (value[3] == 0) {
            if (!have(68, -1, -1)) {
                value[448] = 558;
                *bitword(448) = -1;
            }
        }
    }
    if ((value[HERE] == 104)) {
        if (isnear(71, -1, -1)) {
            if (value[71] == 0) {
                if (have(72, -1, -1)) {
                    value[448] = 511;
                    *bitword(448) = -1;
                }
            }
        }
    }
    if ((value[HERE] == 110)) {
        if (isnear(6, -1, -1)) {
            if (!isnear(71, -1, -1)) {
                value[448] = 513;
                *bitword(448) = -1;
            }
        }
    }
    if ((value[HERE] == 187)) {
        value[448] = 683;
        *bitword(448) = -1;
    }
    if ((((value[HERE] == 179)) ||
         (value[HERE] == 178)) || (value[HERE] == 180)) {
        if (!(bitest(180, 1))) {
            value[448] = 681;
            *bitword(448) = -1;
        }
    }
    if ((value[HERE] == 211)) {
        value[448] = 831;
        *bitword(448) = -1;
    }
    if (bitest(evar(442), 7)) {
        value[448] = 679;
        *bitword(448) = -1;
    }
    value[449] = 281;
    *bitword(449) = -1;
    value[450] = 319;
    *bitword(450) = -1;
    if ((value[442] > value[449]) && value[442] < value[450]) {
        value[448] = 904;
        *bitword(448) = -1;
    }
    if (value[448] > 0) {
        set('V', 467, 'c', 0, NULL, NULL);
        if (query(value[448])) {
            say(9, 677, 20);
            if (query(678)) {
                value[448] += 1;
                say(2, 448, 0);
                value[454] += 20;
                bitmod('c', evar(442), 5);
                if ((((value[HERE] == 179)) ||
                     (value[HERE] == 178)) || (value[HERE] == 180)) {
                    bitmod('c', 179, 5);
                    bitmod('c', 180, 5);
                    bitmod('c', 178, 5);
                } else {
                    if (bitest(evar(442), 7)) {
                        *bitword(448) = -1;
                        value[448] = FLOC - 1;
                        while (++value[448] <= LLOC) {
                            if (bitest(evar(448), 7)) {
                                bitmod('c', evar(448), 5);
                            }
                        }
                    }
                }
            }
        }
    }
}

#ifdef __STDC__
void p395(void)
#else
void p395()
#endif
{
    if (((isnear(69, -1, -1)) && value[69] == 1) || bitest(evar(442), 0)) {
        if (KEY(386)) {
            set('V', 448, 'v', 445, NULL, NULL);
        } else {
            set('V', 448, 'v', 444, NULL, NULL);
        }
        value[449] = irand(4);
        value[450] = 498;
        *bitword(450) = -1;
        value[449] += value[450];
        say(14, 449, 448);
    } else {
        if (pct(25)) {
            say(0, 516, 0);
            p124();
        } else {
            say(0, 502, 0);
        }
    }
    if ((isnear(69, -1, -1)) && value[69] == 1) {
        value[447] -= 1;
        if ((value[447] == 0) || value[447] == 40) {
            p318();
        }
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p65(void)
#else
void p65()
#endif
{
    apport(88, LIMBO);
    say(0, 783, 0);
    value[448] = irand(7);
    value[449] = 784;
    *bitword(449) = -1;
    value[448] += value[449];
    say(2, 448, 0);
    say(0, 483, 0);
    if (isnear(91, -1, -1)) {
        if (value[91] == 1) {
            set('E', 88, 'c', 1, NULL, NULL);
        } else {
            set('E', 88, 'c', 2, NULL, NULL);
        }
        say(0, 88, 0);
        apport(91, LIMBO);
        value[461] -= value[91];
        set('E', 91, 'c', 0, NULL, NULL);
    }
    if (isnear(21, -1, -1)) {
        set('E', 88, 'c', 3, NULL, NULL);
        say(0, 88, 0);
    }
    if (isnear(37, -1, -1)) {
        set('E', 88, 'c', 4, NULL, NULL);
        if (value[37] > 0) {
            set('E', 88, 'c', 5, NULL, NULL);
        }
        say(0, 88, 0);
    }
    if (isnear(6, -1, -1)) {
        set('E', 88, 'c', 6, NULL, NULL);
        say(0, 88, 0);
    }
    if (isnear(71, -1, -1)) {
        set('E', 88, 'c', 7, NULL, NULL);
        say(0, 88, 0);
    }
    if (isnear(32, -1, -1)) {
        set('E', 88, 'c', 8, NULL, NULL);
        say(0, 88, 0);
        apport(32, LIMBO);
    }
    if ((isnear(19, -1, -1)) && value[19] == 0) {
        set('E', 88, 'c', 9, NULL, NULL);
        say(0, 88, 0);
    }
    if (isnear(26, -1, -1)) {
        set('E', 88, 'c', 10, NULL, NULL);
        say(0, 88, 0);
    }
    if ((isnear(24, -1, -1)) && value[24] < 2) {
        set('E', 88, 'c', 11, NULL, NULL);
        say(0, 88, 0);
    }
    if (isnear(90, -1, -1)) {
        set('E', 88, 'c', 12, NULL, NULL);
        say(0, 88, 0);
        apport(90, LIMBO);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p164(void)
#else
void p164()
#endif
{
    if ((have(88, -1, -1)) && pct(10)) {
        say(0, 782, 0);
        apport(88, LIMBO);
        p124();
    }
}

#ifdef __STDC__
void dwarf_attack(void)
#else
void dwarf_attack()
#endif
{
    set('V', 449, 'e', 91, NULL, NULL);
    value[449] += 2;
    value[449] = irand(value[449]);
    value[449] -= 1;
    if (value[449] < 1) {
        return;
    }
    say(13, 490, 449);
    set('V', 450, 'v', 468, NULL, NULL);
    value[450] -= value[452];
    value[450] *= 5;
    if (bitest(91, 11)) {
        value[450] -= 20;
    }
    value[478] += value[450];
    value[478] += 35;
    value[478] /= value[449];
    if (have(63, -1, -1)) {
        value[448] = irand(496);
        if (value[448] > 0) {
            value[450] = 493;
            *bitword(450) = -1;
            value[448] += value[450];
            say(15, 448, 449);
            return;
        }
    }
    if (pct(value[478]) || bitest(91, 10)) {
        say(13, 491, 449);
        bitmod('c', 91, 10);
    } else {
        say(13, 492, 449);
        p124();
    }
}

#ifdef __STDC__
void p563(void)
#else
void p563()
#endif
{
    if (isnear(24, -1, -1)) {
        if (value[24] > 1) {
            say(0, 669, 0);
            if (KEY(392)) {
                apport(value[445], INHAND);
            }
        } else {
            say(12, 837, 445);
            p124();
        }
    } else {
        if (isnear(26, -1, -1)) {
            say(12, 838, 445);
        } else {
            if (isnear(90, -1, -1)) {
                say(12, 839, 445);
                p124();
            } else {
                if (KEY(392)) {
                    apport(value[445], INHAND);
                    return;
                } else {
                    say(2, 465, 0);
                }
            }
        }
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p562(void)
#else
void p562()
#endif
{
    if (have(value[445], -1, -1)) {
        if (KEY(392)) {
            apport(value[445], HERE);
        }
        if (isnear(91, -1, -1)) {
            set('V', 448, 'v', 468, NULL, NULL);
            value[448] -= value[452];
            value[448] *= 5;
            set('V', 449, 'e', 91, NULL, NULL);
            value[449] += 2;
            value[449] *= 15;
            value[448] += value[449];
            if ((KEY(85)) ^ KEY(384)) {
                value[448] += 15;
            }
            if (pct(value[448])) {
                if (pct(5) && !(bitest(evar(458), 8))) {
                    say(12, 645, 445);
                    bitmod('s', evar(458), 8);
                } else {
                    say(0, 644, 0);
                }
                value[91] -= 1;
                if (value[91] == 0) {
                    apport(91, LIMBO);
                } else {
                    set('V', 478, 'c', 0, NULL, NULL);
                    set('V', 477, 'c', 100, NULL, NULL);
                }
                value[461] -= 1;
            } else {
                say(0, 544, 0);
                set('V', 478, 'c', 0, NULL, NULL);
                set('V', 477, 'c', 100, NULL, NULL);
            }
        } else {
            if (isnear(6, -1, -1)) {
                say(0, 542, 0);
                if (KEY(392)) {
                    apport(value[445], INHAND);
                }
            } else {
                if (isnear(19, -1, -1)) {
                    say(12, 648, 445);
                } else {
                    if (isnear(37, -1, -1)) {
                        if (value[37] == 0) {
                            if (KEY(392)) {
                                if (KEY(85)) {
                                    say(0, 666, 0);
                                    set('E', 85, 'c', 1, NULL, NULL);
                                } else {
                                    say(0, 793, 0);
                                }
                            } else {
                                if (pct(50)) {
                                    say(0, 794, 0);
                                } else {
                                    say(0, 795, 0);
                                    p124();
                                }
                            }
                        } else {
                            say(0, 668, 0);
                        }
                        longjmp(loop_back, 1);
                    } else {
                        if (isnear(21, -1, -1)) {
                            say(0, 659, 0);
                        } else {
                            if (isnear(23, -1, -1)) {
                                if (KEY(384)) {
                                    say(12, 806, 445);
                                    p124();
                                } else {
                                    if (KEY(85)) {
                                        say(12, 807, 445);
                                        p124();
                                    } else {
                                        say(0, 808, 0);
                                        apport(86, LIMBO);
                                        apport(23, LIMBO);
                                        apport(63, value[442]);
                                        longjmp(loop_back, 1);
                                    }
                                }
                            } else {
                                if (isnear(16, -1, -1)) {
                                    say(12, 828, 445);
                                    longjmp(loop_back, 1);
                                } else {
                                    p563();
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }
    } else {
        say(12, 717, 445);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p426(void)
#else
void p426()
#endif
{
    if (isnear(34, -1, -1)) {
        if (value[34] == 0) {
            if (((value[446] == 2) &&
                 value[445] == value[472]) || value[444] == value[472]) {
                say(0, 822, 0);
                set('E', 34, 'c', 1, NULL, NULL);
                bitmod('s', 34, 10);
                longjmp(loop_back, 1);
            } else {
                if (!(bitest(34, 10))) {
                    say(0, 825, 0);
                    set('E', 34, 'c', 2, NULL, NULL);
                    set('E', 16, 'c', 1, NULL, NULL);
                    bitmod('s', evar(458), 1);
                    bitmod('s', evar(458), 3);
                    set('E', 3, 'c', 0, NULL, NULL);
                    longjmp(loop_back, 1);
                }
            }
        }
    }
    say(0, 538, 0);
    longjmp(loop_back, 1);
}

void blob_chases_you(void)
{
    if (value[16] > 0) {
        if (value[16] == 16) {
            set('E', 16, 'c', 17, NULL, NULL);
            say(0, 16, 0);
            p124();
        }
        say(0, 16, 0);
        value[16] += 1;
    }
}

#ifdef __STDC__
void p458(void)
#else
void p458()
#endif
{
    if (value[446] > 1) {
        say(12, 729, 445);
    }
}

#ifdef __STDC__
void p454(void)
#else
void p454()
#endif
{
    move(334, -1);
    if (value[69] == 1) {
        set('V', 447, 'c', 0, NULL, NULL);
        if (have(69, -1, -1)) {
            say(0, 849, 0);
        } else {
            say(0, 850, 0);
        }
    } else {
        say(0, 850, 0);
    }
    p124();
}

#ifdef __STDC__
void p549(void)
#else
void p549()
#endif
{
    set('V', 448, 'c', 0, NULL, NULL);
    if ((value[HERE] == 105)) {
        value[448] = 888;
        *bitword(448) = -1;
        value[449] = 106;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 107)) {
        value[448] = 889;
        *bitword(448) = -1;
        value[449] = 174;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 108)) {
        value[448] = 889;
        *bitword(448) = -1;
        value[449] = 174;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 111)) {
        value[448] = 888;
        *bitword(448) = -1;
        value[449] = 114;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 112)) {
        value[448] = 888;
        *bitword(448) = -1;
        value[449] = 113;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 115)) {
        value[448] = 890;
        *bitword(448) = -1;
        value[449] = 122;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 120)) {
        value[448] = 888;
        *bitword(448) = -1;
        value[449] = 188;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 121)) {
        value[448] = 888;
        *bitword(448) = -1;
        value[449] = 188;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 123)) {
        value[448] = 888;
        *bitword(448) = -1;
        value[449] = 124;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 125)) {
        value[448] = 890;
        *bitword(448) = -1;
        value[449] = 148;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 142)) {
        value[448] = 888;
        *bitword(448) = -1;
        value[449] = 104;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 152)) {
        value[448] = 891;
        *bitword(448) = -1;
        value[449] = 151;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 153)) {
        value[448] = 891;
        *bitword(448) = -1;
        value[449] = 149;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 157)) {
        value[448] = 892;
        *bitword(448) = -1;
        value[449] = 158;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 171)) {
        value[448] = 891;
        *bitword(448) = -1;
        value[449] = 155;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 174)) {
        value[448] = 893;
        *bitword(448) = -1;
        value[449] = 334;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 177)) {
        value[448] = 894;
        *bitword(448) = -1;
        value[449] = 174;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 189)) {
        value[448] = 891;
        *bitword(448) = -1;
        value[449] = 138;
        *bitword(449) = -1;
    }
    if (((value[HERE] == 191)) || (value[HERE] == 192)) {
        value[448] = 902;
        *bitword(448) = -1;
        value[449] = 334;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 194)) {
        value[448] = 891;
        *bitword(448) = -1;
        value[449] = 334;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 199)) {
        value[448] = 895;
        *bitword(448) = -1;
        value[449] = 334;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 200)) {
        value[448] = 895;
        *bitword(448) = -1;
        value[449] = 334;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 207)) {
        value[448] = 896;
        *bitword(448) = -1;
        value[449] = 334;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 208)) {
        value[448] = 896;
        *bitword(448) = -1;
        value[449] = 334;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 216)) {
        value[448] = 897;
        *bitword(448) = -1;
        value[449] = 215;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 217)) {
        value[448] = 898;
        *bitword(448) = -1;
        value[449] = 210;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 220)) {
        value[448] = 899;
        *bitword(448) = -1;
        value[449] = 221;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 221)) {
        value[448] = 899;
        *bitword(448) = -1;
        value[449] = 222;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 222)) {
        value[448] = 899;
        *bitword(448) = -1;
        value[449] = 223;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 279)) {
        value[448] = 888;
        *bitword(448) = -1;
        value[449] = 334;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 280)) {
        value[448] = 888;
        *bitword(448) = -1;
        value[449] = 334;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 281)) {
        value[448] = 900;
        *bitword(448) = -1;
        value[449] = 282;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 319)) {
        value[448] = 888;
        *bitword(448) = -1;
        value[449] = 334;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 330)) {
        value[448] = 901;
        *bitword(448) = -1;
        value[449] = 331;
        *bitword(449) = -1;
    }
    if ((value[HERE] == 332)) {
        value[448] = 896;
        *bitword(448) = -1;
        value[449] = 334;
        *bitword(449) = -1;
    }
    if ((value[448] == 0) || KEY(37)) {
        return;
    }
    say(14, 448, 445);
    apport(value[445], value[449]);
    if (KEY(52)) {
        apport(52, 334);
        apport(53, value[449]);
        value[448] = 334;
        *bitword(448) = -1;
        if (!(value[448] == value[449])) {
            say(0, 903, 0);
        }
    }
    if (KEY(81)) {
        apport(83, LIMBO);
        apport(82, LIMBO);
    }
    if ((KEY(83)) || KEY(82)) {
        set('E', 81, 'c', 1, NULL, NULL);
        apport(value[445], LIMBO);
    }
    if ((KEY(70)) && have(71, -1, -1)) {
        apport(71, value[449]);
    }
    if (((KEY(69)) && value[69] == 1) && !(bitest(evar(442), 0))) {
        say(0, 509, 0);
    }
    if (KEY(71)) {
        set('E', 71, 'c', 0, NULL, NULL);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p239(void)
#else
void p239()
#endif
{
    if (value[446] == 1) {
        say(0, 906, 0);
        longjmp(loop_back, 1);
    }
    if (bitest(evar(445), 15)) {
        if ((have(value[445], -1, -1)) || !(bitest(evar(445), 0))) {
            return;
        }
        set('V', 448, 'v', 452, NULL, NULL);
        value[448] -= value[468];
        value[448] *= 5;
        value[448] += 60;
        if (pct(value[448])) {
            say(12, 907, 445);
            p124();
        } else {
            say(12, 908, 445);
            longjmp(loop_back, 1);
        }
    }
}

#ifdef __STDC__
void p470(void)
#else
void p470()
#endif
{
    move(93, 0, -92);
    move(93, 0, -368);
    move(93, 0, -357);
    move(94, 0, -350);
    move(94, 0, -94);
    move(94, 0, -350);
    move(94, 0, -367);
    move(95, 0, -340);
    move(95, 0, -345);
    move(95, 0, -346);
    move(95, 0, -370);
    move(95, 0, -358);
    move(96, 0, -96);
    move(96, 0, -369);
    move(96, 0, -367);
    move(99, 0, -99);
}

#ifdef __STDC__
void p241(void)
#else
void p241()
#endif
{
    move(92, 0, -92);
    move(92, 0, -94);
    move(92, 0, -341);
    move(92, 0, -367);
    move(92, 0, -369);
    move(96, 0, -96);
    move(96, 0, -369);
    move(96, 0, -370);
}

#ifdef __STDC__
void p82(void)
#else
void p82()
#endif
{
    move(92, 0, -92);
    move(92, 0, -343);
    move(92, 0, -359);
    move(92, 0, -368);
    if (value[463] < 2) {
        move(102, value[479], -421);
        move(118, value[479], -425);
    }
}

#ifdef __STDC__
void p83(void)
#else
void p83()
#endif
{
    if (!anyof(346, 340, -1))
        return;
    say(0, 716, 0);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p550(void)
#else
void p550()
#endif
{
    move(92, 0, -339);
    move(92, 0, -94);
    move(92, 0, -369);
    move(96, 0, -96);
    move(96, 0, -367);
    move(96, 0, -368);
    move(96, 0, -357);
    move(99, 0, -99);
    if (!anyof(370, 358, 340, -1))
        return;
    if (value[463] < 2) {
        move(98, -1);
    } else {
        move(195, -1);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p209(void)
#else
void p209()
#endif
{
    move(95, 0, -95);
    move(95, 0, -367);
    move(95, 0, -358);
    move(96, 0, -368);
    move(96, 0, -370);
}

void p210(void)
{
    if (!anyof(96, 341, 369, -1))
        return;
    if (pct(50)) {
        move(96, -1);
    } else {
        move(97, -1);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p211(void)
#else
void p211()
#endif
{
    move(92, 0, -92);
    move(92, 0, -369);
    move(95, 0, -95);
    move(95, 0, -367);
    move(95, 0, -368);
    move(95, 0, -358);
    move(96, 0, -96);
    move(96, 0, -370);
}

#ifdef __STDC__
void p507(void)
#else
void p507()
#endif
{
    move(92, 0, -94);
    move(95, 0, -339);
    move(95, 0, -369);
    move(96, 0, -96);
    move(96, 0, -367);
    move(96, 0, -368);
    move(99, 0, -340);
    move(99, 0, -347);
    move(99, 0, -348);
    move(99, 0, -370);
}

#ifdef __STDC__
void p184(void)
#else
void p184()
#endif
{
    move(92, 0, -94);
    move(95, 0, -339);
    move(95, 0, -369);
    move(96, 0, -96);
    move(96, 0, -367);
    move(96, 0, -368);
    move(99, 0, -340);
    move(99, 0, -347);
    move(99, 0, -348);
    move(99, 0, -370);
}

#ifdef __STDC__
void p508(void)
#else
void p508()
#endif
{
    if (!anyof(98, 346, 358, 350, -1))
        return;
    say(0, 593, 0);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p185(void)
#else
void p185()
#endif
{
    if (!anyof(98, 346, 358, 350, -1))
        return;
    set('V', 463, 'c', 5, NULL, NULL);
    bitmod('c', evar(446), 2);
    bitmod('c', evar(446), 1);
    say(0, 197, 0);
    set('V', 464, 'c', 0, NULL, NULL);
    p204();
}

#ifdef __STDC__
void p148(void)
#else
void p148()
#endif
{
    move(96, 0, -96);
    move(97, 0, -367);
    move(96, 0, -368);
    move(97, 0, -370);
    move(92, 0, -94);
    if (!anyof(339, 345, 369, 98, -1))
        return;
    if (value[463] < 2) {
        move(98, -1);
    } else {
        move(195, -1);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p149(void)
#else
void p149()
#endif
{
    if (!anyof(350, 358, 350, -1))
        return;
    if (value[3] == 1) {
        move(100, -1);
    } else {
        say(0, 591, 0);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p289(void)
#else
void p289()
#endif
{
    if (!anyof(357, 343, 343, 351, -1))
        return;
    if (value[3] == 1) {
        move(99, -1);
    } else {
        say(0, 591, 0);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p290(void)
#else
void p290()
#endif
{
    move(101, 0, -349);
    move(101, 0, -101);
    move(101, 0, -350);
    move(101, 0, -368);
    move(105, 0, -105);
    move(102, 0, -102);
}

#ifdef __STDC__
void p122(void)
#else
void p122()
#endif
{
    move(100, 0, -343);
    move(100, 0, -351);
    move(100, 0, -352);
    move(100, 0, -344);
    move(100, 0, -367);
    move(102, 0, -350);
    move(102, 0, -180);
    move(102, 0, -368);
    move(102, 0, -102);
    move(105, 0, -105);
}

#ifdef __STDC__
void p145(void)
#else
void p145()
#endif
{
    move(100, 0, -344);
    move(101, 0, -349);
    move(101, 0, -101);
    move(101, 0, -353);
    move(101, 0, -155);
    move(101, 0, -367);
    move(103, 0, -103);
    move(103, 0, -350);
    move(103, 0, -357);
    move(103, 0, -368);
    move(105, 0, -105);
    if (value[3] == 1) {
        move(99, 0, -99);
    } else {
        move(100, 591, -99);
    }
}

#ifdef __STDC__
void p146(void)
#else
void p146()
#endif
{
    if (!keyword(421, -1))
        return;
    if ((value[463] > 1) || bitest(evar(458), 3)) {
        say(0, 538, 0);
        bitmod('s', evar(458), 4);
        longjmp(loop_back, 1);
    } else {
        move(94, value[479], -421);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p85(void)
#else
void p85()
#endif
{
    move(100, 0, -344);
    move(102, 0, -358);
    move(102, 0, -367);
    move(102, 0, -102);
    move(104, 0, -350);
    move(104, 0, -357);
    move(104, 0, -368);
    move(105, 0, -105);
    if (value[3] == 1) {
        move(99, 0, -99);
    } else {
        move(100, 591, -99);
    }
}

#ifdef __STDC__
void p59(void)
#else
void p59()
#endif
{
    move(100, 0, -344);
    move(102, 0, -102);
    move(103, 0, -103);
    move(103, 0, -367);
    move(105, 0, -353);
    move(105, 0, -105);
    move(105, 0, -368);
    if (value[3] == 1) {
        move(99, 0, -99);
    } else {
        move(100, 591, -99);
    }
}

#ifdef __STDC__
void p436(void)
#else
void p436()
#endif
{
    move(100, 0, -344);
    move(102, 0, -102);
    move(104, 0, -353);
    move(104, 0, -367);
    move(106, 0, -358);
    if (value[3] == 1) {
        move(99, 0, -99);
    } else {
        move(100, 591, -99);
    }
}

#ifdef __STDC__
void p437(void)
#else
void p437()
#endif
{
    if (!anyof(358, 4, 105, -1))
        return;
    if (have(43, -1, -1)) {
        say(0, 705, 0);
        move(106, -1);
        p124();
    } else {
        move(106, -1);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p438(void)
#else
void p438()
#endif
{
    if (!anyof(368, 360, -1))
        return;
    say(0, 715, 0);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p439(void)
#else
void p439()
#endif
{
    if (!keyword(365, -1))
        return;
    move(106, -1);
    p517();
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p376(void)
#else
void p376()
#endif
{
    move(109, 0, -362);
    move(109, 0, -370);
    move(265, 0, -367);
    move(107, 0, -341);
    move(107, 0, -364);
    move(107, 0, -368);
    move(110, 0, -356);
    move(110, 0, -358);
    move(110, 0, -369);
    move(119, 0, -118);
}

#ifdef __STDC__
void p377(void)
#else
void p377()
#endif
{
    if (!anyof(357, 105, 4, 361, 353, 350, -1))
        return;
    if (have(43, -1, -1)) {
        say(0, 706, 0);
    } else {
        move(105, -1);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p169(void)
#else
void p169()
#endif
{
    move(106, 0, -364);
    move(106, 0, -367);
}

#ifdef __STDC__
void p170(void)
#else
void p170()
#endif
{
    if (!anyof(341, 366, 368, -1))
        return;
    if (value[7] == 0) {
        say(0, 595, 0);
    } else {
        move(108, -1);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p171(void)
#else
void p171()
#endif
{
    if (!keyword(365, -1))
        return;
    if (value[7] == 1) {
        say(0, 594, 0);
    } else {
        move(174, -1);
        p517();
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p237(void)
#else
void p237()
#endif
{
    move(106, 0, -364);
    move(106, 0, -343);
    move(106, 0, -369);
}

#ifdef __STDC__
void p383(void)
#else
void p383()
#endif
{
    move(106, 0, -356);
    move(106, 0, -357);
    move(106, 0, -367);
}

#ifdef __STDC__
void p384(void)
#else
void p384()
#endif
{
    if (!anyof(369, 362, 370, 363, 368, 341, 373, 375,
               374, 372, 371, 358, -1))
        return;
    if (isnear(6, -1, -1)) {
        say(0, 707, 0);
        longjmp(loop_back, 1);
    } else {
        move(115, 0, -369);
        move(115, 0, -362);
        move(116, 0, -370);
        move(116, 0, -363);
        move(117, 0, -368);
        move(117, 0, -341);
        move(267, 0, -358);
        move(266, 0, -371);
        move(273, 0, -374);
        move(270, 0, -372);
        move(157, 0, -375);
        if (pct(35)) {
            move(157, 0, -373);
        } else {
            say(0, 552, 0);
        }
        longjmp(loop_back, 1);
    }
}

#ifdef __STDC__
void p565(void)
#else
void p565()
#endif
{
    move(112, 0, -367);
    move(112, 0, -366);
    move(151, 0, -368);
    move(151, 0, -151);
    move(114, 0, -358);
    move(114, 0, -105);
}

#ifdef __STDC__
void p566(void)
#else
void p566()
#endif
{
    if (!keyword(355, -1))
        return;
    say(0, 643, 0);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p172(void)
#else
void p172()
#endif
{
    move(112, 0, -357);
    move(112, 0, -343);
}

#ifdef __STDC__
void p577(void)
#else
void p577()
#endif
{
    move(111, 0, -357);
    move(111, 0, -343);
}

#ifdef __STDC__
void p578(void)
#else
void p578()
#endif
{
    if (!keyword(376, -1))
        return;
    if (value[10] == 0) {
        say(0, 718, 0);
    } else {
        if (value[10] == 2) {
            say(0, 719, 0);
            move(111, -1);
        } else {
            say(0, 720, 0);
            move(170, -1);
        }
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p579(void)
#else
void p579()
#endif
{
    if (!keyword(378, 10, -1))
        return;
    say(0, 613, 0);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p572(void)
#else
void p572()
#endif
{
    move(126, 0, -368);
}

#ifdef __STDC__
void p573(void)
#else
void p573()
#endif
{
    if (!anyof(341, 366, 367, -1))
        return;
    if (value[7] == 0) {
        say(0, 595, 0);
    } else {
        move(107, -1);
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p574(void)
#else
void p574()
#endif
{
    if (!keyword(365, -1))
        return;
    if (value[7] == 1) {
        say(0, 594, 0);
    } else {
        move(174, -1);
        p517();
    }
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p575(void)
#else
void p575()
#endif
{
    if (!keyword(369, -1))
        return;
    say(0, 708, 0);
    move(126, -1);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p328(void)
#else
void p328()
#endif
{
    move(110, 0, -364);
    move(110, 0, -343);
    move(110, 0, -370);
    move(118, 0, -369);
    move(118, 0, -118);
    move(122, 0, -358);
    move(122, 0, -355);
}

#ifdef __STDC__
void p514(void)
#else
void p514()
#endif
{
    move(110, 0, -364);
    move(110, 0, -343);
    move(110, 0, -369);
}

#ifdef __STDC__
void p580(void)
#else
void p580()
#endif
{
    move(110, 0, -364);
    move(110, 0, -343);
    move(110, 0, -367);
    move(146, 0, -368);
    move(146, 0, -357);
}

#ifdef __STDC__
void p593(void)
#else
void p593()
#endif
{
    move(115, 0, -370);
    move(119, 0, -367);
    move(119, 0, -119);
    move(120, 0, -368);
}

#ifdef __STDC__
void p594(void)
#else
void p594()
#endif
{
    if (!anyof(425, 179, -1))
        return;
    if ((value[446] == 2) && !KEY(336)) {
        return;
    }
    if ((value[463] > 1) || bitest(evar(458), 3)) {
        say(0, 538, 0);
        bitmod('s', evar(458), 4);
        longjmp(loop_back, 1);
    } else {
        if (KEY(425)) {
            move(94, -1);
        } else {
            move(179, -1);
        }
        say(2, 479, 0);
        longjmp(loop_back, 1);
    }
}

#ifdef __STDC__
void p298(void)
#else
void p298()
#endif
{
    move(118, 0, -358);
    move(118, 0, -118);
    move(106, 0, -357);
}

#ifdef __STDC__
void p582(void)
#else
void p582()
#endif
{
    move(118, 0, -367);
    move(118, 0, -118);
}

#ifdef __STDC__
void p583(void)
#else
void p583()
#endif
{
    if (!keyword(365, -1))
        return;
    say(0, 705, 0);
    move(188, -1);
    p124();
}

#ifdef __STDC__
void p151(void)
#else
void p151()
#endif
{
    move(123, 0, -367);
    move(123, 0, -349);
    move(115, 0, -357);
    move(115, 0, -355);
    move(125, 0, -368);
    move(149, 0, -149);
}

#ifdef __STDC__
void p71(void)
#else
void p71()
#endif
{
    move(122, 0, -368);
    move(122, 0, -349);
    move(124, 0, -358);
    move(124, 0, -105);
    move(124, 0, -376);
}

#ifdef __STDC__
void p524(void)
#else
void p524()
#endif
{
    move(123, 0, -376);
    move(123, 0, -357);
    move(123, 0, -343);
}

#ifdef __STDC__
void p525(void)
#else
void p525()
#endif
{
    if (!anyof(98, 346, 358, 339, 340, -1))
        return;
    say(0, 593, 0);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p165(void)
#else
void p165()
#endif
{
    move(122, 0, -367);
    move(122, 0, -353);
    move(148, 0, -358);
    move(148, 0, -355);
    move(149, 0, -149);
}

#ifdef __STDC__
void p568(void)
#else
void p568()
#endif
{
    move(127, 0, -370);
    move(127, 0, -357);
    move(127, 0, -353);
    move(127, 0, -376);
    move(108, 0, -367);
    move(144, 0, -368);
    move(144, 0, -349);
}

#ifdef __STDC__
void p569(void)
#else
void p569()
#endif
{
    if (!keyword(369, -1))
        return;
    say(0, 708, 0);
    move(108, -1);
    longjmp(loop_back, 1);
}

#ifdef __STDC__
void p331(void)
#else
void p331()
#endif
{
    move(126, 0, -357);
    move(127, 0, -369);
    move(128, 0, -367);
    move(130, 0, -370);
    move(162, 0, -368);
}

#ifdef __STDC__
void p332(void)
#else
void p332()
#endif
{
    move(127, 0, -368);
    move(129, 0, -370);
    move(130, 0, -367);
}

#ifdef __STDC__
void p333(void)
#else
void p333()
#endif
{
    move(128, 0, -367);
    move(133, 0, -358);
    move(135, 0, -370);
    move(164, 0, -369);
}

#ifdef __STDC__
void p334(void)
#else
void p334()
#endif
{
    move(127, 0, -368);
    move(128, 0, -369);
    move(131, 0, -367);
    move(132, 0, -370);
    move(169, 0, -357);
    move(169, 0, -358);
}

#ifdef __STDC__
void p335(void)
#else
void p335()
#endif
{
    move(130, 0, -368);
    move(130, 0, -343);
}

#ifdef __STDC__
void p336(void)
#else
void p336()
#endif
{
    move(130, 0, -367);
    move(130, 0, -343);
}

#ifdef __STDC__
void p337(void)
#else
void p337()
#endif
{
    move(129, 0, -357);
    move(129, 0, -343);
}

#ifdef __STDC__
void p338(void)
#else
void p338()
#endif
{
    move(135, 0, -367);
    move(136, 0, -368);
}

#ifdef __STDC__
void p339(void)
#else
void p339()
#endif
{
    move(129, 0, -367);
    move(134, 0, -368);
    move(136, 0, -358);
    move(137, 0, -370);
}

#ifdef __STDC__
void p340(void)
#else
void p340()
#endif
{
    move(134, 0, -368);
    move(135, 0, -357);
    move(137, 0, -367);
    move(138, 0, -370);
}

#ifdef __STDC__
void p341(void)
#else
void p341()
#endif
{
    move(135, 0, -368);
    move(136, 0, -367);
    move(137, 0, -370);
    move(138, 0, -357);
    move(140, 0, -369);
    move(168, 0, -358);
}

#ifdef __STDC__
void p342(void)
#else
void p342()
#endif
{
    move(136, 0, -368);
    move(137, 0, -369);
    move(139, 0, -370);
}

#ifdef __STDC__
void p343(void)
#else
void p343()
#endif
{
    move(138, 0, -368);
    move(138, 0, -343);
}

#ifdef __STDC__
void p344(void)
#else
void p344()
#endif
{
    move(137, 0, -368);
    move(140, 0, -369);
    move(141, 0, -358);
    move(142, 0, -367);
}

#ifdef __STDC__
void p345(void)
#else
void p345()
#endif
{
    move(140, 0, -357);
    move(140, 0, -343);
}

#ifdef __STDC__
void p346(void)
#else
void p346()
#endif
{
    move(104, 0, -358);
    move(104, 0, -376);
    move(140, 0, -368);
    move(143, 0, -370);
    move(165, 0, -369);
    move(166, 0, -367);
}

#ifdef __STDC__
void p347(void)
#else
void p347()
#endif
{
    move(142, 0, -367);
    move(142, 0, -343);
}

#ifdef __STDC__
void p321(void)
#else
void p321()
#endif
{
    move(126, 0, -367);
    move(126, 0, -357);
    move(126, 0, -349);
    move(145, 0, -368);
    move(146, 0, -369);
    move(146, 0, -358);
    move(146, 0, -355);
}

#ifdef __STDC__
void p322(void)
#else
void p322()
#endif
{
    move(144, 0, -367);
    move(146, 0, -369);
    move(186, 0, -370);
}

#ifdef __STDC__
void p135(void)
#else
void p135()
#endif
{
    move(144, 0, -368);
    move(147, 0, -369);
    move(117, 0, -367);
    move(145, 0, -370);
}

#ifdef __STDC__
void p141(void)
#else
void p141()
#endif
{
    move(146, 0, -370);
    move(146, 0, -343);
}

#ifdef __STDC__
void p123(void)
#else
void p123()
#endif
{
    move(125, 0, -357);
    move(125, 0, -376);
    move(149, 0, -368);
    move(149, 0, -149);
    move(182, 0, -369);
    move(182, 0, -182);
    move(185, 0, -367);
}
