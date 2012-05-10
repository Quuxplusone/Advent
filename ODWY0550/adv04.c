#include "adv0.h"
#include "adv3.h"

void p120(void)
{
    if (!KEY(417))
        return;
    if (bitest(evar(458), 5)) {
        p121();
    }
}

void p240(void)
{
    if (value[467] == 0) {
        set('V', 467, 'c', 1, NULL, NULL);
    }
    p242();
    if (value[467] > 0) {
        if ((((((((isnear(91, -1, -1)) ||
                  isnear(19, -1, -1)) ||
                 isnear(21, -1, -1)) ||
                isnear(23, -1, -1)) ||
               value[16] > 0) ||
              isnear(31, -1, -1)) || value[90] > 0) || isnear(24, -1, -1)) {
            say(0, 549, 0);
        } else {
            if (bitest(evar(458), 7)) {
                say(0, 550, 0);
            } else {
                say(0, 548, 0);
                bitmod('s', evar(458), 7);
            }
        }
    }
    longjmp(loop_back, 1);
}

void p292(void)
{
    say(0, 637, 0);
    longjmp(loop_back, 1);
}

void p459(void)
{
    if (query(515)) {
        set('V', 464, 'c', 1, NULL, NULL);
        p204();
    }
    say(2, 465, 0);
    longjmp(loop_back, 1);
}

void p70(void)
{
    say(0, 658, 0);
    bitmod('s', evar(446), 1);
    bitmod('c', evar(446), 2);
    value[465] = 484;
    *bitword(465) = -1;
    longjmp(loop_back, 1);
}

void p329(void)
{
    if ((value[446] == 1) || !(bitest(evar(458), 5))) {
        say(0, 557, 0);
        longjmp(loop_back, 1);
    }
    if (bitest(evar(445), 13)) {
        say(2, 479, 0);
        move(value[445], -1);
    } else {
        say(0, 557, 0);
    }
    longjmp(loop_back, 1);
}

void p301(void)
{
    default_to(0, value[HERE], 5);
    if (!anyof(21, 91, 19, 6, 71, 37, 77, 76, 23, 16, 26, 90, 24, 25, -1))
        return;
    if (!isnear(value[445], -1, -1))
        return;
    if (KEY(21)) {
        p314();
    } else {
        if (KEY(91)) {
            p310();
        } else {
            if (KEY(19)) {
                p309();
            } else {
                if (KEY(6)) {
                    p313();
                } else {
                    if (KEY(16)) {
                        p307();
                    } else {
                        if (KEY(37)) {
                            p304();
                        } else {
                            if ((KEY(76)) || KEY(77)) {
                                p306();
                            } else {
                                if (KEY(23)) {
                                    p312();
                                } else {
                                    if (KEY(71)) {
                                        p305();
                                    } else {
                                        if (KEY(26)) {
                                            p308();
                                        } else {
                                            if (KEY(90)) {
                                                p311();
                                            } else {
                                                if (KEY(24)) {
                                                    p303();
                                                } else {
                                                    if (KEY(25)) {
                                                        p243();
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
    }
    longjmp(loop_back, 1);
}

void p302(void)
{
    if (value[446] == 1) {
        say(0, 540, 0);
        longjmp(loop_back, 1);
    }
}

void p189(void)
{
    bitmod('c', evar(446), 1);
    bitmod('s', evar(446), 2);
    say(2, 465, 0);
    value[465] = 482;
    *bitword(465) = -1;
    longjmp(loop_back, 1);
}

void p215(void)
{
    bitmod('c', evar(446), 1);
    bitmod('c', evar(446), 2);
    value[465] = 484;
    *bitword(465) = -1;
    say(2, 465, 0);
    longjmp(loop_back, 1);
}

void p194(void)
{
    if (!anyof(37, 21, 71, 6, 91, 19, 24, 90, -1))
        return;
    if (isnear(value[445], -1, -1)) {
        if (KEY(37)) {
            if (isnear(80, -1, -1)) {
                say(0, 670, 0);
                set('E', 37, 'c', 1, NULL, NULL);
                if (have(80, -1, -1)) {
                }
                apport(80, 333);
                if (value[85] == 1) {
                    set('E', 85, 'c', 0, NULL, NULL);
                }
            } else {
                say(0, 600, 0);
            }
        } else {
            if (KEY(21)) {
                say(0, 685, 0);
            } else {
                if (KEY(6)) {
                    if (have(71, -1, -1)) {
                        say(0, 599, 0);
                        apport(71, 333);
                    } else {
                        say(0, 600, 0);
                    }
                } else {
                    if (KEY(91)) {
                        say(0, 601, 0);
                        bitmod('s', 91, 11);
                    } else {
                        if (KEY(71)) {
                            say(0, 598, 0);
                        } else {
                            if (KEY(19)) {
                                if (value[19] == 0) {
                                    say(0, 600, 0);
                                } else {
                                    say(0, 669, 0);
                                }
                            } else {
                                if (KEY(24)) {
                                    if (value[24] < 2) {
                                        say(0, 600, 0);
                                    } else {
                                        say(0, 669, 0);
                                    }
                                } else {
                                    if (KEY(90)) {
                                        say(0, 841, 0);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } else {
        say(12, 522, 445);
    }
    longjmp(loop_back, 1);
}

void p195(void)
{
    if (value[446] == 1) {
        say(0, 506, 0);
    } else {
        say(0, 608, 0);
    }
    longjmp(loop_back, 1);
}

void p481(void)
{
    set('V', 464, 'c', 1, NULL, NULL);
    p229();
    say(13, 730, 455);
    say(13, 731, 456);
    longjmp(loop_back, 1);
}

void p299(void)
{
    say(0, 726, 0);
    longjmp(loop_back, 1);
}

void p286(void)
{
    say(0, 504, 0);
    longjmp(loop_back, 1);
}

void p425(void)
{
    say(0, 504, 0);
    longjmp(loop_back, 1);
}

void p496(void)
{
    say(0, 547, 0);
    longjmp(loop_back, 1);
}

void p191(void)
{
    if (!anyof(433, 432, 431, 404, -1))
        return;
    set('V', 460, 'c', 0, NULL, NULL);
    say(0, 538, 0);
    longjmp(loop_back, 1);
}

void p192(void)
{
    set('V', 460, 'c', 1, NULL, NULL);
    say(2, 465, 0);
    longjmp(loop_back, 1);
}

void p196(void)
{
    if (value[460] == 0) {
        set('V', 460, 'c', 2, NULL, NULL);
        say(2, 465, 0);
    } else {
        set('V', 460, 'c', 0, NULL, NULL);
        say(0, 538, 0);
    }
    longjmp(loop_back, 1);
}

void p206(void)
{
    if (value[460] == 1) {
        set('V', 460, 'c', 3, NULL, NULL);
        say(2, 465, 0);
    } else {
        set('V', 460, 'c', 0, NULL, NULL);
        say(0, 538, 0);
    }
    longjmp(loop_back, 1);
}

void p207(void)
{
    if (value[460] == 2) {
        if (((location[49] == 172)) || (location[49] == 334)) {
            say(0, 538, 0);
        } else {
            if (isnear(49, -1, -1)) {
                set('E', 49, 'c', 1, NULL, NULL);
            } else {
                if ((value[HERE] == 172)) {
                    set('E', 49, 'c', 0, NULL, NULL);
                } else {
                    set('E', 49, 'c', 2, NULL, NULL);
                }
            }
            if (have(49, -1, -1)) {
                apport(49, HERE);
            }
            say(0, 49, 0);
            set('E', 49, 'c', 0, NULL, NULL);
            if ((location[49] == 333)) {
                bitmod('s', 21, 10);
                if ((value[21] == 1) || value[21] == 2) {
                    if (isnear(22, -1, -1)) {
                        set('E', 21, 'c', 5, NULL, NULL);
                        say(0, 21, 0);
                    }
                    set('E', 21, 'c', 0, NULL, NULL);
                    apport(21, 199);
                    apport(22, 333);
                }
            }
            apport(49, 172);
        }
    } else {
        say(0, 538, 0);
    }
    set('V', 460, 'c', 0, NULL, NULL);
    longjmp(loop_back, 1);
}

void p216(void)
{
    set('V', 460, 'c', 0, NULL, NULL);
    say(0, 538, 0);
    longjmp(loop_back, 1);
}

void p591(void)
{
    say(0, 538, 0);
    longjmp(loop_back, 1);
}

void p453(void)
{
    say(0, 538, 0);
    longjmp(loop_back, 1);
}

void p202(void)
{
    if (!KEY(417))
        return;
    if ((bitest(evar(442), 4)) && !(bitest(100, 1))) {
        say(0, 553, 0);
    } else {
        say(0, 554, 0);
    }
    longjmp(loop_back, 1);
}

void p203(void)
{
    if (value[446] > 1) {
        if (bitest(evar(445), 15)) {
            if (isnear(value[445], -1, -1)) {
                if (have(value[445], -1, -1)) {
                    say(0, 592, 0);
                } else {
                    say(0, 633, 0);
                }
            } else {
                say(0, 555, 0);
            }
            longjmp(loop_back, 1);
        } else {
            if (bitest(evar(445), 13)) {
                if ((value[HERE] == value[445])) {
                    say(0, 635, 0);
                } else {
                    say(0, 555, 0);
                }
            } else {
                say(0, 557, 0);
            }
            longjmp(loop_back, 1);
        }
    }
}

void p526(void)
{
    say(0, 642, 0);
    longjmp(loop_back, 1);
}

void p63(void)
{
    if (!KEY(52))
        return;
    if (!isnear(52, -1, -1))
        return;
    say(0, 702, 0);
    if (have(52, -1, -1)) {
    }
    apport(52, 333);
    apport(53, value[442]);
    longjmp(loop_back, 1);
}

void p64(void)
{
    if (!KEY(88))
        return;
    if (!isnear(88, -1, -1))
        return;
    p65();
}

void p205(void)
{
    if (!KEY(52))
        return;
    if (!isnear(53, -1, -1))
        return;
    say(0, 641, 0);
    longjmp(loop_back, 1);
}

void p198(void)
{
    if (!KEY(52))
        return;
    if (!isnear(52, -1, -1))
        return;
    if ((bitest(evar(442), 6)) || (value[HERE] == 113)) {
        say(0, 640, 0);
        if (have(52, -1, -1)) {
        }
        apport(52, 333);
        apport(53, value[442]);
    } else {
        say(0, 639, 0);
    }
    longjmp(loop_back, 1);
}

void p199(void)
{
    if (!KEY(81))
        return;
    if (!isnear(81, -1, -1))
        return;
    if (value[81] == 1) {
        if (bitest(evar(442), 6)) {
            say(0, 605, 0);
            set('E', 81, 'c', 0, NULL, NULL);
            if (have(81, -1, -1)) {
                apport(82, INHAND);
            }
        } else {
            if ((value[HERE] == 113)) {
                say(0, 606, 0);
                set('E', 81, 'c', 2, NULL, NULL);
                if (have(81, -1, -1)) {
                    apport(83, INHAND);
                }
            } else {
                say(0, 604, 0);
            }
        }
    } else {
        say(0, 603, 0);
    }
    longjmp(loop_back, 1);
}

void p200(void)
{
    if (value[446] > 1) {
        say(0, 607, 0);
        longjmp(loop_back, 1);
    }
}

void p456(void)
{
    if (!KEY(82))
        return;
    if (have(82, -1, -1)) {
        apport(82, 333);
        set('E', 81, 'c', 1, NULL, NULL);
        if (isnear(10, -1, -1)) {
            value[10] += 1;
            say(0, 10, 0);
            value[10] += 1;
            if (value[10] == 6) {
                set('E', 10, 'c', 0, NULL, NULL);
                bitmod('s', 11, 7);
            } else {
                bitmod('c', 11, 7);
            }
            move(value[442], -1);
            set('E', 11, 'e', 10, NULL, NULL);
        } else {
            if (isnear(5, -1, -1)) {
                say(0, 611, 0);
                set('E', 5, 'c', 0, NULL, NULL);
            } else {
                say(0, 575, 0);
                bitmod('s', evar(442), 12);
            }
        }
    } else {
        say(12, 717, 82);
    }
    longjmp(loop_back, 1);
}

void p457(void)
{
    if (!KEY(83))
        return;
    if (have(83, -1, -1)) {
        apport(83, 333);
        set('E', 81, 'c', 1, NULL, NULL);
        if (isnear(10, -1, -1)) {
            say(0, 610, 0);
        } else {
            if (isnear(5, -1, -1)) {
                say(0, 612, 0);
                set('E', 5, 'c', 1, NULL, NULL);
            } else {
                say(0, 575, 0);
                bitmod('s', evar(442), 12);
            }
        }
    } else {
        say(12, 717, 445);
    }
    longjmp(loop_back, 1);
}

void p440(void)
{
    if (!anyof(91, 6, 71, 19, 21, 37, 18, 23, 24, 90, -1))
        return;
    if (!isnear(value[445], -1, -1))
        return;
    say(0, 507, 0);
    longjmp(loop_back, 1);
}

void p174(void)
{
    default_to(0, value[HERE], 8);
    if (!isnear(89, -1, -1))
        return;
    if ((value[446] == 1) || KEY(89)) {
        if (have(89, -1, -1)) {
            apport(89, HERE);
        }
        set('E', 89, 'c', 2, NULL, NULL);
        say(0, 89, 0);
        set('V', 469, 'c', 30, NULL, NULL);
        value[469] += value[471];
        apport(89, 333);
        set('V', 468, 'c', 12, NULL, NULL);
        longjmp(loop_back, 1);
    }
}

void p175(void)
{
    if (!isnear(80, -1, -1))
        return;
    if ((value[446] == 1) || KEY(80)) {
        if (have(80, -1, -1)) {
        }
        apport(80, 333);
        say(0, 568, 0);
        longjmp(loop_back, 1);
    }
}

void p176(void)
{
    if (!anyof(91, 19, 71, 6, 37, 21, 10, 23, 24, 90, -1))
        return;
    if (isnear(value[445], -1, -1)) {
        say(0, 567, 0);
    } else {
        say(12, 522, 445);
    }
    longjmp(loop_back, 1);
}

void p177(void)
{
    if (value[446] == 1) {
        say(0, 676, 0);
        longjmp(loop_back, 1);
    } else {
        if (bitest(evar(445), 15)) {
            if (isnear(value[445], -1, -1)) {
                say(0, 574, 0);
                longjmp(loop_back, 1);
            }
        }
    }
}

void p472(void)
{
    if (!KEY(69))
        return;
    if (!isnear(69, -1, -1))
        return;
    say(12, 572, 445);
    longjmp(loop_back, 1);
}

void p473(void)
{
    if (value[446] > 1) {
        if (bitest(evar(445), 15)) {
            if (isnear(value[445], -1, -1)) {
                say(0, 573, 0);
                longjmp(loop_back, 1);
            }
        }
    }
}

void p41(void)
{
    if (((bitest(evar(442), 3)) || bitest(evar(443), 3)) || value[443] == 0) {
        say(0, 589, 0);
    } else {
        move(value[443], -1);
    }
    longjmp(loop_back, 1);
}

void p374(void)
{
    say(0, 565, 0);
    longjmp(loop_back, 1);
}

void p479(void)
{
    if (!anyof(425, 421, 179, 422, 428, 427, 424,
               429, 420, 430, 423, 435, 426, -1))
        return;
    if (value[445] < LPROC && value[445] >= 0)
        (*procs[value[445]]) ();
    else
        pcall(value[445]);
    longjmp(loop_back, 1);
}

void p480(void)
{
    if (value[446] == 2) {
        longjmp(loop_back, 1);
    }
}

void p477(void)
{
    if (bitest(evar(458), 0)) {
        say(0, 704, 0);
    } else {
        set('V', 448, 'c', 1, NULL, NULL);
        if ((0 > 0) && !(bitest(evar(458), 5))) {
            say(9, 745, 0);
            if (!(query(703))) {
                set('V', 448, 'c', 0, NULL, NULL);
            }
        }
        if (value[448] > 0) {
            svar(4, &value[466]);
            value[466] *= 60;
            svar(5, &value[448]);
            value[466] += value[448];
            special(1, &value[448]);
            if (value[448] == 0) {
                if ((bitest(evar(458), 5)) || 0 == 0) {
                    if (query(750)) {
                        say(2, 465, 0);
                        say(0, 483, 0);
                        longjmp(loop_back, 1);
                    }
                }
                say(2, 465, 0);
                say(0, 483, 0);
                finita();
            } else {
                say(0, 746, 0);
            }
        } else {
            say(2, 465, 0);
        }
    }
    longjmp(loop_back, 1);
}

void p468(void)
{
    special(5, &value[448]);
    if (value[448] > 0) {
        if (!(bitest(evar(458), 5))) {
            value[449] = 764;
            *bitword(449) = -1;
            value[449] += value[448];
            say(2, 449, 0);
            longjmp(loop_back, 1);
        }
    }
    if (bitest(evar(458), 5)) {
        set('V', 448, 'c', 1, NULL, NULL);
    } else {
        set('V', 448, 'c', 0, NULL, NULL);
    }
    special(7, &value[448]);
    special(2, &value[448]);
    if (value[448] == 0) {
        svar(4, &value[448]);
        svar(5, &value[449]);
        value[448] *= 60;
        value[448] += value[449];
        if (value[448] < value[466]) {
            value[448] += 720;
            value[448] += 720;
        }
        value[448] -= value[466];
        special(8, &value[449]);
        if (value[449] == 1) {
            bitmod('s', evar(458), 5);
        } else {
            bitmod('c', evar(458), 5);
        }
        if ((bitest(evar(458), 5)) || value[448] >= 0) {
            if (value[69] == 1) {
                value[447] += 1;
            }
            if ((bitest(evar(458), 5)) || 0 == 0) {
                if (!(query(749))) {
                    special(3, &value[448]);
                }
            } else {
                special(3, &value[448]);
            }
            special(4, &value[448]);
            move(value[442], -1);
            say(2, 465, 0);
            say(0, 483, 0);
        } else {
            say(9, 751, 0);
            say(0, 483, 0);
            finita();
        }
    } else {
        if (value[448] == 1) {
            say(0, 747, 0);
            value[457] -= 1;
        } else {
            say(0, 748, 0);
            special(3, &value[448]);
        }
    }
    longjmp(loop_back, 1);
}

void p154(void)
{
    if ((value[446] == 1) || KEY(82)) {
        if (bitest(evar(442), 6)) {
            say(0, 569, 0);
            longjmp(loop_back, 1);
        } else {
            if ((isnear(81, -1, -1)) && value[81] == 0) {
                say(0, 571, 0);
                apport(82, 333);
                set('E', 81, 'c', 1, NULL, NULL);
                longjmp(loop_back, 1);
            }
        }
        if (KEY(82)) {
            say(12, 522, 445);
        } else {
            say(0, 754, 0);
        }
    } else {
        if (bitest(evar(445), 15)) {
            if (isnear(value[445], -1, -1)) {
                say(0, 608, 0);
            } else {
                say(12, 522, 445);
            }
        } else {
            say(0, 608, 0);
        }
    }
    longjmp(loop_back, 1);
}

void p394(void)
{
    say(0, 486, 0);
    longjmp(loop_back, 1);
}

void p461(void)
{
    if (value[446] > 1) {
        if (bitest(evar(445), 15)) {
            if (isnear(value[445], -1, -1)) {
                if (KEY(78)) {
                    p463();
                } else {
                    if (KEY(28)) {
                        p464();
                    } else {
                        if (KEY(8)) {
                            p465();
                        } else {
                            say(12, 744, 444);
                        }
                    }
                }
            } else {
                say(12, 522, 445);
            }
        } else {
            say(0, 608, 0);
        }
    } else {
        return;
    }
    longjmp(loop_back, 1);
}

void p244(void)
{
    say(0, 767, 0);
    special(6, &value[448]);
    longjmp(loop_back, 1);
}

void p590(void)
{
    if (query(768)) {
        say(0, 769, 0);
        input(0);
        say(0, 770, 0);
        value[454] += 10;
    } else {
        say(2, 465, 0);
    }
    longjmp(loop_back, 1);
}

void p138(void)
{
    if (value[476] == 0) {
        set('V', 476, 'c', 1, NULL, NULL);
    } else {
        set('V', 476, 'c', -1, NULL, NULL);
        say(0, 557, 0);
    }
    longjmp(loop_back, 1);
}

void p238(void)
{
    if (value[476] == 1) {
        set('V', 476, 'c', 2, NULL, NULL);
    } else {
        set('V', 476, 'c', -1, NULL, NULL);
        say(0, 557, 0);
    }
    longjmp(loop_back, 1);
}

void p282(void)
{
    if (value[476] == 2) {
        set('V', 476, 'c', 3, NULL, NULL);
    } else {
        set('V', 476, 'c', -1, NULL, NULL);
        say(0, 557, 0);
    }
    longjmp(loop_back, 1);
}

void p502(void)
{
    if (value[476] == 3) {
        set('V', 476, 'c', 4, NULL, NULL);
    } else {
        set('V', 476, 'c', -1, NULL, NULL);
        say(0, 557, 0);
    }
    longjmp(loop_back, 1);
}

void p217(void)
{
    if (value[476] == 4) {
        set('V', 476, 'c', -1, NULL, NULL);
        bitmod('s', evar(458), 5);
        bitmod('c', evar(458), 0);
        say(0, 484, 0);
    } else {
        set('V', 476, 'c', -1, NULL, NULL);
        say(0, 557, 0);
    }
    longjmp(loop_back, 1);
}

void p112(void)
{
    say(0, 713, 0);
    longjmp(loop_back, 1);
}

void p326(void)
{
    say(0, 564, 0);
    longjmp(loop_back, 1);
}

void p370(void)
{
    if ((isnear(30, -1, -1)) && value[30] == 0) {
        set('E', 30, 'c', 1, NULL, NULL);
        say(0, 830, 0);
    } else {
        say(0, 538, 0);
    }
    longjmp(loop_back, 1);
}

void p403(void)
{
    if (!KEY(424))
        return;
    if ((((bitest(69, 10)) ||
          bitest(evar(442), 0)) ||
         !(bitest(278, 1))) || !isnear(69, -1, -1)) {
        say(0, 538, 0);
    } else {
        if (have(69, -1, -1)) {
            say(0, 776, 0);
            p124();
        } else {
            if (value[447] > 40) {
                apport(69, 334);
                set('E', 38, 'c', 1, NULL, NULL);
                set('E', 69, 'c', 0, NULL, NULL);
                set('V', 447, 'c', 0, NULL, NULL);
                if (irand(100) < 50) {
                    say(0, 777, 0);
                    say(0, 509, 0);
                } else {
                    say(0, 778, 0);
                    p124();
                }
            } else {
                say(0, 779, 0);
                value[447] += 150;
                set('E', 69, 'c', 1, NULL, NULL);
                bitmod('s', 69, 10);
            }
        }
    }
    longjmp(loop_back, 1);
}

void p404(void)
{
    say(0, 538, 0);
    longjmp(loop_back, 1);
}

void p475(void)
{
    say(0, 538, 0);
    longjmp(loop_back, 1);
}

void p540(void)
{
    say(0, 538, 0);
    longjmp(loop_back, 1);
}

void p316(void)
{
    p426();
}

void p595(void)
{
    p426();
}

void p315(void)
{
    p426();
}

void p510(void)
{
    p426();
}

void p60(void)
{
    p426();
}

void p469(void)
{
    if (((value[446] == 1) || KEY(27)) && isnear(27, -1, -1)) {
        say(0, 854, 0);
        move(191, -1);
        apport(27, 333);
        longjmp(loop_back, 1);
    }
}

void p434(void)
{
    bitmod('s', 26, 11);
    if (bitest(evar(442), 4)) {
        say(0, 538, 0);
    } else {
        if (((isnear(81, -1, -1)) && value[81] == 0) || bitest(evar(442), 6)) {
            if (irand(100) < 85) {
                say(0, 538, 0);
            } else {
                if (irand(100) < 95) {
                    say(0, 864, 0);
                    p124();
                } else {
                    say(0, 865, 0);
                    finita();
                }
            }
        } else {
            if ((isnear(85, -1, -1)) || isnear(86, -1, -1)) {
                if (isnear(85, -1, -1)) {
                    say(0, 866, 0);
                    if (have(85, -1, -1)) {
                    }
                    apport(85, 333);
                }
                if (isnear(86, -1, -1)) {
                    say(0, 867, 0);
                    if (have(86, -1, -1)) {
                    }
                    apport(86, 333);
                }
            } else {
                value[448] = irand(3);
                value[448] *= 2;
                if (value[91] > 1) {
                    value[448] += 1;
                }
                if (isnear(91, -1, -1)) {
                    if (irand(100) < 70) {
                        value[449] = 868;
                        *bitword(449) = -1;
                        value[448] += value[449];
                        say(2, 448, 0);
                        apport(91, 333);
                        value[461] -= value[91];
                        set('E', 91, 'c', 0, NULL, NULL);
                    } else {
                        value[449] = 874;
                        *bitword(449) = -1;
                        value[448] += value[449];
                        say(2, 448, 0);
                        p124();
                    }
                } else {
                    say(0, 538, 0);
                }
            }
        }
    }
    longjmp(loop_back, 1);
}

void p474(void)
{
    if (bitest(evar(458), 5)) {
        set('V', 462, 'c', 0, NULL, NULL);
        move(value[442], -1);
        say(2, 465, 0);
    } else {
        say(0, 557, 0);
    }
    longjmp(loop_back, 1);
}

void make_portable(int obj)
{
    objbits[obj - FOBJ] |= (1 << 0);
}

void make_freebie(int obj)
{
    objbits[obj - FOBJ] |= (1 << 9);
}

void make_lit(int room)
{
    placebits[room - FLOC] |= (1 << 0);
}

void make_thrower(int room)
{
    placebits[room - FLOC] |= (1 << 9);
}

void set_notincave(int room)
{
    placebits[room - FLOC] |= (1 << 4);
}

void set_room_nodwarf(int room)
{
    placebits[room - FLOC] |= (1 << 2);
}

void initialize_portability(void)
{
    make_portable(68);
    make_portable(69);
    make_portable(70);
    make_portable(72);
    make_portable(74);
    make_portable(71);
    make_portable(75);
    make_portable(76);
    make_portable(77);
    make_portable(78);
    make_portable(79);
    make_portable(80);
    make_portable(81);
    make_portable(82);
    make_portable(83);
    make_portable(85);
    make_portable(37);
    make_portable(38);
    make_portable(43);
    make_portable(44);
    make_portable(45);
    make_portable(46);
    make_portable(47);
    make_portable(48);
    make_portable(49);
    make_portable(50);
    make_portable(52);
    make_portable(53);
    make_portable(54);
    make_portable(57);
    make_portable(58);
    make_portable(59);
    make_portable(60);
    make_portable(62);
    make_portable(63);
    make_portable(64);
    make_portable(41);
    make_portable(51);
    make_portable(87);
    make_portable(88);
    make_portable(89);
    make_portable(65);
    make_portable(66);
    make_portable(67);
    make_portable(86);
    make_portable(42);
    make_portable(55);
    make_portable(56);
    make_portable(84);
    make_portable(73);
    make_portable(61);
}

void set_freebie_objects(void)
{
    make_freebie(71);
    make_freebie(83);
    make_freebie(82);
    make_freebie(37);
}

void set_rooms_lit(void)
{
    make_lit(92);
    make_lit(93);
    make_lit(94);
    make_lit(95);
    make_lit(96);
    make_lit(97);
    make_lit(98);
    make_lit(195);
    make_lit(99);
    make_lit(100);
    make_lit(101);
    make_lit(179);
    make_lit(207);
    make_lit(278);
    make_lit(330);
    make_lit(331);
    make_lit(211);
    make_lit(212);
    make_lit(208);
    make_lit(209);
    make_lit(245);
    make_lit(246);
    make_lit(250);
    make_lit(332);
    make_lit(196);
    make_lit(197);
    make_lit(194);
}

void set_nodwarf_rooms(void)
{
    set_room_nodwarf(100);
    set_room_nodwarf(101);
    set_room_nodwarf(178);
    set_room_nodwarf(179);
    set_room_nodwarf(180);
    set_room_nodwarf(199);
    set_room_nodwarf(200);
    set_room_nodwarf(204);
    set_room_nodwarf(205);
    set_room_nodwarf(206);
    set_room_nodwarf(207);
    set_room_nodwarf(251);
    set_room_nodwarf(252);
    set_room_nodwarf(253);
    set_room_nodwarf(254);
    set_room_nodwarf(198);
    set_room_nodwarf(113);
    set_room_nodwarf(114);
    set_room_nodwarf(124);
    set_room_nodwarf(208);
    set_room_nodwarf(209);
    set_room_nodwarf(210);
    set_room_nodwarf(211);
    set_room_nodwarf(212);
    set_room_nodwarf(213);
    set_room_nodwarf(214);
    set_room_nodwarf(215);
    set_room_nodwarf(216);
    set_room_nodwarf(217);
    set_room_nodwarf(218);
    set_room_nodwarf(219);
    set_room_nodwarf(220);
    set_room_nodwarf(268);
    set_room_nodwarf(221);
    set_room_nodwarf(222);
    set_room_nodwarf(223);
    set_room_nodwarf(224);
    set_room_nodwarf(225);
    set_room_nodwarf(226);
    set_room_nodwarf(227);
    set_room_nodwarf(228);
    set_room_nodwarf(229);
    set_room_nodwarf(230);
    set_room_nodwarf(231);
    set_room_nodwarf(232);
    set_room_nodwarf(233);
    set_room_nodwarf(234);
    set_room_nodwarf(235);
    set_room_nodwarf(236);
    set_room_nodwarf(237);
    set_room_nodwarf(238);
    set_room_nodwarf(239);
    set_room_nodwarf(240);
    set_room_nodwarf(241);
    set_room_nodwarf(242);
    set_room_nodwarf(243);
    set_room_nodwarf(244);
    set_room_nodwarf(245);
    set_room_nodwarf(246);
    set_room_nodwarf(247);
    set_room_nodwarf(248);
    set_room_nodwarf(249);
    set_room_nodwarf(250);
    set_room_nodwarf(192);
    set_room_nodwarf(193);
    set_room_nodwarf(194);
    set_room_nodwarf(269);
}

void set_notincave_rooms(void)
{
    set_notincave(92);
    set_notincave(93);
    set_notincave(94);
    set_notincave(95);
    set_notincave(96);
    set_notincave(97);
    set_notincave(98);
    set_notincave(99);
    set_notincave(195);
}

void set_noback(int room)
{
    placebits[room - FLOC] |= (1 << 3);
}

void set_noback_rooms(void)
{
    for (int i = 128; i <= 143; ++i)
        set_noback(i);
    set_noback(158);
    for (int i = 162; i <= 169; ++i)
        set_noback(i);
    set_noback(171);
    set_noback(178);
    set_noback(179);
    set_noback(180);
    set_noback(182);
    set_noback(187);
    set_noback(189);
    set_noback(190);
    set_noback(192);
    set_noback(196);
    set_noback(198);
    set_noback(199);
    set_noback(200);
    set_noback(208);
    set_noback(210);
    set_noback(211);
    set_noback(218);
    for (int i = 226; i <= 244; ++i)
        set_noback(i);
    for (int i = 255; i <= 264; ++i)
        set_noback(i);
    for (int i = 281; i <= 308; ++i) {
        if (i == 284 || i == 286 || i == 288 || i == 298 || i == 302)
            continue;
        set_noback(i);
    }
    set_noback(324);
    set_noback(325);
    set_noback(332);
}

void set_inmaze_rooms(void)
{
    bitmod('s', 127, 7);
    bitmod('s', 128, 7);
    bitmod('s', 129, 7);
    bitmod('s', 130, 7);
    bitmod('s', 131, 7);
    bitmod('s', 132, 7);
    bitmod('s', 133, 7);
    bitmod('s', 134, 7);
    bitmod('s', 135, 7);
    bitmod('s', 136, 7);
    bitmod('s', 137, 7);
    bitmod('s', 138, 7);
    bitmod('s', 139, 7);
    bitmod('s', 140, 7);
    bitmod('s', 141, 7);
    bitmod('s', 142, 7);
    bitmod('s', 143, 7);
    bitmod('s', 162, 7);
    bitmod('s', 163, 7);
    bitmod('s', 164, 7);
    bitmod('s', 165, 7);
    bitmod('s', 166, 7);
    bitmod('s', 167, 7);
    bitmod('s', 168, 7);
    bitmod('s', 169, 7);
    bitmod('s', 186, 7);
    bitmod('s', 190, 7);
    bitmod('s', 198, 7);
    bitmod('s', 255, 7);
    bitmod('s', 256, 7);
    bitmod('s', 257, 7);
    bitmod('s', 258, 7);
    bitmod('s', 259, 7);
    bitmod('s', 260, 7);
    bitmod('s', 261, 7);
    bitmod('s', 262, 7);
    bitmod('s', 263, 7);
    bitmod('s', 264, 7);
    bitmod('s', 282, 7);
    bitmod('s', 283, 7);
    bitmod('s', 284, 7);
    bitmod('s', 285, 7);
    bitmod('s', 286, 7);
    bitmod('s', 287, 7);
    bitmod('s', 288, 7);
    bitmod('s', 289, 7);
    bitmod('s', 290, 7);
    bitmod('s', 291, 7);
    bitmod('s', 292, 7);
    bitmod('s', 293, 7);
    bitmod('s', 294, 7);
    bitmod('s', 295, 7);
    bitmod('s', 296, 7);
    bitmod('s', 297, 7);
    bitmod('s', 298, 7);
    bitmod('s', 299, 7);
    bitmod('s', 300, 7);
    bitmod('s', 301, 7);
    bitmod('s', 302, 7);
    bitmod('s', 303, 7);
    bitmod('s', 304, 7);
    bitmod('s', 305, 7);
    bitmod('s', 306, 7);
    bitmod('s', 307, 7);
    bitmod('s', 308, 7);
    bitmod('s', 309, 7);
    bitmod('s', 310, 7);
    bitmod('s', 311, 7);
    bitmod('s', 312, 7);
    bitmod('s', 313, 7);
    bitmod('s', 314, 7);
    bitmod('s', 315, 7);
    bitmod('s', 316, 7);
    bitmod('s', 317, 7);
    bitmod('s', 318, 7);
}

void set_oneexit_rooms(void)
{
    bitmod('s', 109, 8);
    bitmod('s', 116, 8);
    bitmod('s', 265, 8);
    bitmod('s', 120, 8);
    bitmod('s', 121, 8);
    bitmod('s', 131, 8);
    bitmod('s', 132, 8);
    bitmod('s', 133, 8);
    bitmod('s', 134, 8);
    bitmod('s', 139, 8);
    bitmod('s', 141, 8);
    bitmod('s', 143, 8);
    bitmod('s', 147, 8);
    bitmod('s', 156, 8);
    bitmod('s', 161, 8);
    bitmod('s', 163, 8);
    bitmod('s', 164, 8);
    bitmod('s', 167, 8);
    bitmod('s', 168, 8);
    bitmod('s', 175, 8);
    bitmod('s', 184, 8);
    bitmod('s', 191, 8);
    bitmod('s', 194, 8);
    bitmod('s', 264, 8);
    bitmod('s', 269, 8);
    bitmod('s', 272, 8);
    bitmod('s', 274, 8);
    bitmod('s', 275, 8);
    bitmod('s', 284, 8);
    bitmod('s', 288, 8);
    bitmod('s', 292, 8);
    bitmod('s', 293, 8);
    bitmod('s', 299, 8);
    bitmod('s', 300, 8);
    bitmod('s', 323, 8);
    bitmod('s', 331, 8);
}

void set_openable_objects(void)
{
    bitmod('s', 3, 6);
    bitmod('s', 5, 6);
    bitmod('s', 76, 6);
    bitmod('s', 77, 6);
    bitmod('s', 84, 6);
    bitmod('s', 62, 6);
    bitmod('s', 34, 6);
    bitmod('s', 88, 6);
}

void set_invisible_objects(void)
{
    bitmod('s', 4, 7);
    bitmod('s', 7, 7);
    bitmod('s', 9, 7);
    bitmod('s', 12, 7);
    bitmod('s', 17, 7);
    bitmod('s', 18, 7);
    bitmod('s', 30, 7);
    bitmod('s', 29, 7);
    bitmod('s', 31, 7);
    bitmod('s', 35, 7);
    bitmod('s', 39, 7);
    bitmod('s', 91, 7);
    bitmod('s', 79, 7);
    bitmod('s', 13, 7);
    bitmod('s', 11, 7);
}

void set_hintable_rooms(void)
{
    bitmod('s', 127, 5);
    bitmod('s', 128, 5);
    bitmod('s', 129, 5);
    bitmod('s', 130, 5);
    bitmod('s', 131, 5);
    bitmod('s', 132, 5);
    bitmod('s', 133, 5);
    bitmod('s', 134, 5);
    bitmod('s', 135, 5);
    bitmod('s', 136, 5);
    bitmod('s', 137, 5);
    bitmod('s', 138, 5);
    bitmod('s', 139, 5);
    bitmod('s', 140, 5);
    bitmod('s', 141, 5);
    bitmod('s', 142, 5);
    bitmod('s', 143, 5);
    bitmod('s', 162, 5);
    bitmod('s', 163, 5);
    bitmod('s', 164, 5);
    bitmod('s', 165, 5);
    bitmod('s', 166, 5);
    bitmod('s', 167, 5);
    bitmod('s', 168, 5);
    bitmod('s', 169, 5);
    bitmod('s', 186, 5);
    bitmod('s', 190, 5);
    bitmod('s', 198, 5);
    bitmod('s', 255, 5);
    bitmod('s', 256, 5);
    bitmod('s', 257, 5);
    bitmod('s', 258, 5);
    bitmod('s', 259, 5);
    bitmod('s', 260, 5);
    bitmod('s', 261, 5);
    bitmod('s', 262, 5);
    bitmod('s', 263, 5);
    bitmod('s', 264, 5);
    bitmod('s', 104, 5);
    bitmod('s', 99, 5);
    bitmod('s', 110, 5);
    bitmod('s', 187, 5);
    bitmod('s', 179, 5);
    bitmod('s', 178, 5);
    bitmod('s', 180, 5);
    bitmod('s', 282, 5);
    bitmod('s', 283, 5);
    bitmod('s', 284, 5);
    bitmod('s', 285, 5);
    bitmod('s', 286, 5);
    bitmod('s', 287, 5);
    bitmod('s', 288, 5);
    bitmod('s', 289, 5);
    bitmod('s', 290, 5);
    bitmod('s', 291, 5);
    bitmod('s', 292, 5);
    bitmod('s', 293, 5);
    bitmod('s', 294, 5);
    bitmod('s', 295, 5);
    bitmod('s', 296, 5);
    bitmod('s', 297, 5);
    bitmod('s', 298, 5);
    bitmod('s', 299, 5);
    bitmod('s', 300, 5);
    bitmod('s', 301, 5);
    bitmod('s', 302, 5);
    bitmod('s', 303, 5);
    bitmod('s', 304, 5);
    bitmod('s', 305, 5);
    bitmod('s', 306, 5);
    bitmod('s', 307, 5);
    bitmod('s', 308, 5);
    bitmod('s', 309, 5);
    bitmod('s', 310, 5);
    bitmod('s', 311, 5);
    bitmod('s', 312, 5);
    bitmod('s', 313, 5);
    bitmod('s', 314, 5);
    bitmod('s', 315, 5);
    bitmod('s', 316, 5);
    bitmod('s', 317, 5);
    bitmod('s', 318, 5);
    bitmod('s', 211, 5);
}

void set_water_rooms(void)
{
    bitmod('s', 94, 6);
    bitmod('s', 92, 6);
    bitmod('s', 95, 6);
    bitmod('s', 98, 6);
    bitmod('s', 124, 6);
    bitmod('s', 174, 6);
    bitmod('s', 191, 6);
    bitmod('s', 192, 6);
}

void set_thrower_rooms(void)
{
    make_thrower(105);
    make_thrower(107);
    make_thrower(108);
    make_thrower(111);
    make_thrower(112);
    make_thrower(115);
    make_thrower(120);
    make_thrower(121);
    make_thrower(123);
    make_thrower(125);
    make_thrower(142);
    make_thrower(152);
    make_thrower(153);
    make_thrower(157);
    make_thrower(171);
    make_thrower(174);
    make_thrower(177);
    make_thrower(191);
    make_thrower(192);
    make_thrower(189);
    make_thrower(194);
    make_thrower(199);
    make_thrower(200);
    make_thrower(207);
    make_thrower(208);
    make_thrower(216);
    make_thrower(217);
    make_thrower(220);
    make_thrower(221);
    make_thrower(222);
    make_thrower(279);
    make_thrower(280);
    make_thrower(281);
    make_thrower(319);
    make_thrower(330);
    make_thrower(332);
}

void set_valued_objects(void)
{
    bitmod('s', 43, 2);
    bitmod('s', 44, 2);
    bitmod('s', 45, 2);
    bitmod('s', 46, 2);
    bitmod('s', 47, 2);
    bitmod('s', 48, 2);
    bitmod('s', 49, 2);
    bitmod('s', 50, 2);
    bitmod('s', 52, 2);
    bitmod('s', 54, 2);
    bitmod('s', 57, 2);
    bitmod('s', 58, 2);
    bitmod('s', 59, 2);
    bitmod('s', 60, 2);
    bitmod('s', 62, 2);
    bitmod('s', 67, 2);
    bitmod('s', 65, 2);
    bitmod('s', 66, 2);
    bitmod('s', 63, 2);
    bitmod('s', 64, 2);
    bitmod('s', 41, 2);
    bitmod('s', 51, 2);
    bitmod('s', 42, 2);
    bitmod('s', 55, 2);
    bitmod('s', 56, 2);
    bitmod('s', 61, 2);
}

void set_unstable_objects(void)
{
    bitmod('s', 48, 4);
    bitmod('s', 65, 4);
    bitmod('s', 70, 4);
    bitmod('s', 67, 4);
    bitmod('s', 87, 4);
    bitmod('s', 89, 4);
    bitmod('s', 41, 4);
    bitmod('s', 42, 4);
    bitmod('s', 56, 4);
    bitmod('s', 73, 4);
    bitmod('s', 61, 4);
    bitmod('s', 84, 4);
}

void set_mortal_objects(void)
{
    bitmod('s', 21, 5);
    bitmod('s', 91, 5);
    bitmod('s', 19, 5);
    bitmod('s', 6, 5);
    bitmod('s', 16, 5);
    bitmod('s', 37, 5);
    bitmod('s', 76, 5);
    bitmod('s', 77, 5);
    bitmod('s', 23, 5);
    bitmod('s', 71, 5);
    bitmod('s', 26, 5);
    bitmod('s', 90, 5);
    bitmod('s', 24, 5);
    bitmod('s', 25, 5);
    bitmod('s', 80, 8);
    bitmod('s', 89, 8);
}

void move_objects_to_starting_locations(void)
{
    apport(68, 94);
    apport(69, 94);
    apport(3, 99);
    apport(70, 101);
    apport(72, 102);
    apport(4, 105);
    apport(71, 104);
    apport(5, 173);
    apport(75, 175);
    apport(6, 110);
    apport(7, 107);
    apport(8, 180);
    apport(76, 182);
    apport(78, 185);
    apport(80, 94);
    apport(81, 94);
    apport(10, 114);
    apport(11, 111);
    apport(12, 189);
    apport(15, 120);
    apport(17, 176);
    apport(19, 202);
    apport(20, 199);
    apport(21, 199);
    apport(37, 254);
    apport(29, 207);
    apport(33, 264);
    apport(39, 175);
    apport(43, 109);
    apport(44, 108);
    apport(45, 115);
    apport(46, 116);
    apport(47, 117);
    apport(49, 172);
    apport(50, 174);
    apport(52, 176);
    apport(54, 179);
    apport(57, 180);
    apport(60, 251);
    apport(62, 254);
    apport(64, 328);
    apport(41, 331);
    apport(40, 331);
    apport(51, 266);
    apport(88, 275);
    apport(89, 274);
    apport(65, 300);
    apport(67, 323);
    apport(34, 267);
    apport(31, 324);
    apport(32, 321);
    apport(86, 265);
    apport(23, 277);
    apport(35, 246);
    apport(36, 246);
    apport(55, 246);
    apport(42, 269);
    apport(30, 209);
    apport(13, 211);
    apport(56, 213);
    apport(66, 250);
    apport(84, 249);
    apport(24, 218);
    apport(73, 223);
    apport(61, 194);
    apport(25, 192);
}

void set_schizoid_objects(void)
{
    bitmod('s', 3, 3);
    bitmod('s', 4, 3);
    bitmod('s', 7, 3);
    bitmod('s', 11, 3);
    bitmod('s', 15, 3);
    bitmod('s', 19, 3);
    bitmod('s', 20, 3);
    bitmod('s', 21, 3);
    bitmod('s', 22, 3);
    bitmod('s', 31, 3);
    bitmod('s', 34, 3);
    bitmod('s', 13, 3);
    bitmod('s', 29, 3);
    bitmod('s', 24, 3);
}

void wake_up_and_greet_user(void)
{
    set('V', 477, 'c', 35, NULL, NULL);
    set('V', 478, 'c', 25, NULL, NULL);
    move(92, -1);
    value[465] = 484;
    *bitword(465) = -1;
    value[462] = irand(10);
    value[462] += 15;
    set('V', 447, 'c', 300, NULL, NULL);
    value[461] = irand(5);
    value[461] += 4;
    if (irand(100) < 5) {
        value[475] = irand(400);
        value[475] += 100;
    } else {
        set('V', 475, 'c', 0, NULL, NULL);
    }
    set('E', 62, 'c', 1, NULL, NULL);
    set('E', 13, 'c', 8, NULL, NULL);
    set('E', 84, 'c', 1, NULL, NULL);
    set('V', 452, 'c', 0, NULL, NULL);
    set('V', 468, 'c', 7, NULL, NULL);
    say(0, 485, 0);
    special(5, &value[448]);
    if (value[448] > 0) {
        value[449] = 757;
        *bitword(449) = -1;
        value[449] += value[448];
        say(2, 449, 0);
        if (value[448] == 1) {
            special(6, &value[449]);
        }
        special(9, &value[449]);
        say(0, 483, 0);
        say(0, 760, 0);
        if (query(761)) {
            say(0, 483, 0);
            say(0, 484, 0);
            say(0, 483, 0);
            make_portable(evar(458));
        } else {
            say(0, 762, 0);
            finita();
        }
    }
    if (query(561)) {
        say(0, 487, 0);
    }
    say(0, 483, 0);
}

void p24(void)
{
    if (bitest(evar(458), 1)) {
        blob_chases_you();
    }
    if (!(bitest(evar(446), 0))) {
        if ((isnear(91, -1, -1)) && irand(100) < value[477]) {
            p166();
        }
        return;
    }
    if (((bitest(evar(443), 8)) &&
         (location[91] == value[443])) && !(bitest(evar(458), 5))) {
        move(value[443], -1);
        bitmod('c', evar(446), 0);
        say(0, 488, 0);
        return;
    }
    if (isnear(13, -1, -1)) {
        p433();
    }
    bitmod('c', evar(446), 0);
    value[479] = irand(6);
    value[448] = 796;
    *bitword(448) = -1;
    value[479] += value[448];
    if (irand(100) < 5) {
        value[479] = 802;
        *bitword(479) = -1;
    }
    value[459] += 1;
    if (bitest(evar(458), 0)) {
        set('V', 448, 'c', 30, NULL, NULL);
    } else {
        set('V', 448, 'c', 800, NULL, NULL);
    }
    if (value[459] == value[448]) {
        say(0, 763, 0);
        p204();
    }
    if (isnear(69, -1, -1)) {
        if (value[69] == 1) {
            value[447] -= 1;
            if ((value[447] == 40) || value[447] == 0) {
                p318();
            }
        }
    }
    if (!((location[90] == 333))) {
        apport(90, value[442]);
        if (value[90] > -1) {
            say(0, 848, 0);
        }
        value[90] += 1;
    }
    if (((bitest(evar(442), 1)) &&
         bitest(evar(446), 1)) || bitest(evar(446), 2)) {
        set('V', 449, 'c', 0, NULL, NULL);
    } else {
        set('V', 449, 'c', 1, NULL, NULL);
    }
    if (((isnear(69, -1, -1)) && value[69] == 1) || bitest(evar(442), 0)) {
        say(2, 442, 0);
        if (bitest(evar(442), 12)) {
            say(0, 576, 0);
        }
        if (bitest(evar(442), 1)) {
            set('V', 450, 'c', 2, NULL, NULL);
        } else {
            set('V', 450, 'c', 3, NULL, NULL);
        }
        bitmod('s', evar(442), 1);
        if (!((location[91] == 333))) {
            if (!((bitest(evar(442), 4)) || bitest(evar(442), 2))) {
                apport(91, value[442]);
            }
        }
        *bitword(448) = -1;
        value[448] = FOBJ - 1;
        while (++value[448] <= LOBJ) {
            if ((isnear(value[448], -1, -1)) && !have(value[448], -1, -1)) {
                bitmod('s', evar(448), 1);
                if ((value[449] == 1) && !(bitest(evar(448), 7))) {
                    set('V', 449, 'c', 0, NULL, NULL);
                    say(0, 483, 0);
                }
                say(2, 448, 0);
            }
        }
        if (have(37, -1, -1)) {
            say(0, 636, 0);
        }
    } else {
        if (((bitest(evar(443), 0)) ||
             irand(100) < 75) || bitest(evar(458), 6)) {
            say(0, 509, 0);
        } else {
            say(0, 516, 0);
            p124();
        }
    }
    bitmod('c', evar(458), 6);
    if ((value[HERE] == 118)) {
        if (irand(100) < 35) {
            say(0, 497, 0);
        }
    }
    if (isnear(90, -1, -1)) {
        value[90] += 1;
        if (value[90] > 6) {
            p124();
        }
    }
    if (!(bitest(evar(442), 4))) {
        value[462] -= value[450];
        if (value[462] < 1) {
            p113();
        }
    }
    if (isnear(91, -1, -1)) {
        set('V', 478, 'c', 50, NULL, NULL);
        p166();
    }
    if ((((location[69] == 334)) && (value[HERE] == 92)) && value[463] < 4) {
        say(0, 688, 0);
        set('V', 464, 'c', 1, NULL, NULL);
        p204();
    }
}

void p25(void)
{
    if (bitest(evar(442), 5)) {
        value[467] += 1;
        if (((value[467] > 30) && !(bitest(evar(442), 7))) || value[467] > 50) {
            p242();
        }
    } else {
        set('V', 467, 'c', 0, NULL, NULL);
    }
    if (value[16] == 16) {
        apport(16, value[442]);
    }
    if (bitest(evar(446), 5)) {
        bitmod('c', evar(446), 5);
        set('V', 452, 'c', 0, NULL, NULL);
        *bitword(448) = -1;
        value[448] = FOBJ - 1;
        while (++value[448] <= LOBJ) {
            if ((have(value[448], -1, -1)) && !(bitest(evar(448), 9))) {
                value[452] += 1;
            }
        }
    }
    set('V', 478, 'c', 25, NULL, NULL);
    set('V', 477, 'c', 35, NULL, NULL);
    input(0);
    value[460] -= 1;
    value[457] += 1;
    if (value[446] == 0) {
        longjmp(loop_back, 1);
    }
    if (value[444] == 9999) {
        say(0, 557, 0);
        longjmp(loop_back, 1);
    }
    if (KEY(336)) {
        p458();
    }
    if (value[446] > 1) {
        if (value[445] == 9999) {
            value[448] = 338;
            *bitword(448) = -1;
            if (value[444] > value[448]) {
                say(12, 556, 445);
                longjmp(loop_back, 1);
            }
        }
    }
    if (value[442] < LPROC && value[442] >= 0)
        (*procs[value[442]]) ();
    else
        pcall(value[442]);
    if (bitest(evar(444), 13)) {
        if ((value[HERE] == value[444])) {
            say(0, 635, 0);
        } else {
            say(0, 634, 0);
        }
    } else {
        if (value[444] < LPROC && value[444] >= 0)
            (*procs[value[444]]) ();
        else
            pcall(value[444]);
        if (bitest(evar(444), 15)) {
            if (isnear(value[444], -1, -1)) {
                say(12, 714, 444);
                bitmod('s', evar(446), 4);
            } else {
                say(12, 522, 444);
            }
        } else {
            p42();
        }
    }
    longjmp(loop_back, 1);
}
