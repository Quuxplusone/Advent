/* Adventure4+ - copyleft @ M.L.Arnautov 1991 */
#include <stdio.h>
#define NOVARARGS
#ifdef __50SERIES
#ifdef BUFFERS
#undef BUFFERS
#endif 
extern int time (); 
#define g0 "i"
#define h0 "o"
#define unlink delete
#else 
#ifdef vms
#define unlink delete
#include <time.h>
#else 
#include <sys/types.h>
#endif 
extern time_t time (); 
#define g0 "rb"
#define h0 "wb"
#endif 
extern char *strncpy (); extern void exit (); FILE *a0; FILE *log_file;
char *u0 = NULL; 
#define x0(X) putchar(X); if (log_file) (void)fputc(X,log_file); 
#define i0(X) if (log_file) (void)fputc(X,log_file); (void)putchar(X++);
#define k0(X) printf(X); if (log_file) (void)fprintf(log_file,X);
#define g1(X,Y) printf(X,Y); if (log_file) (void)fprintf(log_file,X,Y);
#include "advkern.h"
int v0; jmp_buf n0; int e0 [e1]; int j0 [n1 + 1]; short o0 [c0 * (n1 - r0
+ 1)]; short r1 [j1 * (q0 - w0 + 1)]; short b0 [x1 * (e1 - d0 + 1)]; char
c1 [161] = "\n"; char h1 [161] = "\n"; char u1 [161]; int k1, r2; int o1;
char b1 [20]; char x2 [20]; char l0 [20]; char v1 [20]; long l1; long e2
= 0; 
#include "autod3.h"
#include "autod2.h"
#include "autod0.h"
#ifdef i1
int g2 = 1; 
#endif 
#ifdef i2
#ifdef BUFFERS
#undef BUFFERS
#endif 
char j2 [l2]; 
#else 
#define e3 1024
#ifdef MSD
#ifdef BUFFERS
#if BUFFERS == 0
#undef BUFFERS
#endif 
#else 
#define BUFFERS 31
#endif 
#endif 
#ifdef BUFFERS
char j2 [BUFFERS * e3]; long s0 [BUFFERS]; long i3 [BUFFERS]; long x3 [BUFFERS];
#else 
char j2 [e3]; long s0; long i3; 
#endif 
#endif 
char *i4 [100]; short t0 [100]; char z0 [100]; short r3; 
#define w1 73
long r4; long l3; long n2; 
#ifdef y0
long p0; long t1; 
#endif 
char p1 [w1 + 40]; char *k2 = p1; int t2 = -1; int c2 [40]; int q1 [40];
int c3; int t3; int j3; int v2; int d1; int m0; char q2 [16]; int g3; int
x4; char *c4; 
#ifndef NOVARARGS
int k3 (a1) int a1; { int *z1; z1 = &a1; while (*z1 >= 0) { if (m1 (*z1))
return (1); else z1 += 1; } return (0); } int p2 (a1) int a1; { int *z1;
z1 = &a1; while (*z1 >= 0) { if (!m1 (*z1)) return (0); else z1 += 1; }
return (1); } 
#else 
int k3 (q3,b2,t4,j4,s1,y1,c5,y2,e4,l4,l5,v3,t5,w2,q4,b3) int q3,b2,t4,j4,s1,y1,c5,y2,e4,l4,l5,v3,t5,w2,q4,b3;
{ if (q3 == -1) return (0); if (m1 (q3)) return (1); if (b2 == -1) return
(0); if (m1 (b2)) return (1); if (t4 == -1) return (0); if (m1 (t4)) return
(1); if (j4 == -1) return (0); if (m1 (j4)) return (1); if (s1 == -1) return
(0); if (m1 (s1)) return (1); if (y1 == -1) return (0); if (m1 (y1)) return
(1); if (c5 == -1) return (0); if (m1 (c5)) return (1); if (y2 == -1) return
(0); if (m1 (y2)) return (1); if (e4 == -1) return (0); if (m1 (e4)) return
(1); if (l4 == -1) return (0); if (m1 (l4)) return (1); if (l5 == -1) return
(0); if (m1 (l5)) return (1); if (v3 == -1) return (0); if (m1 (v3)) return
(1); if (t5 == -1) return (0); if (m1 (t5)) return (1); if (w2 == -1) return
(0); if (m1 (w2)) return (1); if (q4 == -1) return (0); if (m1 (q4)) return
(1); if (b3 == -1) return (0); if (m1 (b3)) return (1); return (0); } int
p2 (q3,b2,t4,j4,s1,y1,c5,y2,e4,l4,l5,v3,t5,w2,q4,b3) int q3,b2,t4,j4,s1,y1,c5,y2,e4,l4,l5,v3,t5,w2,q4,b3;
{ if (q3 == -1) return (1); if (!m1 (q3)) return (0); if (b2 == -1) return
(1); if (!m1 (b2)) return (0); if (t4 == -1) return (1); if (!m1 (t4)) return
(0); if (j4 == -1) return (1); if (!m1 (j4)) return (0); if (s1 == -1) return
(1); if (!m1 (s1)) return (0); if (y1 == -1) return (1); if (!m1 (y1)) return
(0); if (c5 == -1) return (1); if (!m1 (c5)) return (0); if (y2 == -1) return
(1); if (!m1 (y2)) return (0); if (e4 == -1) return (1); if (!m1 (e4)) return
(0); if (l4 == -1) return (1); if (!m1 (l4)) return (0); if (l5 == -1) return
(1); if (!m1 (l5)) return (0); if (v3 == -1) return (1); if (!m1 (v3)) return
(0); if (t5 == -1) return (1); if (!m1 (t5)) return (0); if (w2 == -1) return
(1); if (!m1 (w2)) return (0); if (q4 == -1) return (1); if (!m1 (q4)) return
(0); if (b3 == -1) return (1); if (!m1 (b3)) return (0); return (1); } 
#endif 
int d2 (i5) int i5; { int p3; e2 = (((e2 << 10) + e2) / 13) & 32767; return
(p3 = e2 % i5); } 
#define d3 100
char c6 (v4) long v4; { 
#ifdef i2
#ifdef w3
return (j2 [v4]); 
#else 
return (j2 [v4] ^ q2 [v4 % g3]); 
#endif 
#else 
#ifdef BUFFERS
int i6; char *l6; int y3; long j5; void h2 (); y3 = 0; l6 = j2; for (i6
= 0; i6 < BUFFERS; i6++) { if (v4 >= s0 [i6] && v4 < i3 [i6]) goto f0; l6
+= e3; } for (i6 = 0; i6 < BUFFERS; i6++) { if (i3 [i6] == 0) goto j6; if
(x3 [y3] > x3 [i6]) y3 = i6; } i6 = y3; j6: l3++; l6 = j2 + e3 * i6; j5
= (v4 / e3) * e3 ; if (fseek (a0, j5, 0)) h2 (); s0 [i6] = j5; i3 [i6] =
fread (l6, sizeof (char), e3, a0) + j5; 
#ifdef y0
(void) printf ("Wanted %ld.  Buffer %d: from %ldK.\n", v4, i6, j5 / e3);
#endif 
if (s0 [i6] > i3 [i6]) h2 (); f0: x3 [i6] = n2; 
#ifdef w3
return (*(l6 + v4 - s0 [i6])); 
#else 
return (*(l6 + v4 - s0 [i6]) ^ q2 [v4 % g3]); 
#endif 
#else 
void h2 (); if (v4 < s0 || v4 >= i3) { l3++; if (fseek (a0, v4, 0)) h2 ();
s0 = v4; i3 = fread (j2, sizeof (char), e3, a0) + s0 - 1; if (s0 > i3) h2
(); } 
#ifdef w3
return (j2 [v4 - s0]); 
#else 
return (j2 [v4 - s0] ^ q2 [v4 % g3]); 
#endif 
#endif 
} void h2 () { (void) k0 ("Unable to retrieve required data! Sorry...\n");
(void) fclose (a0); if (log_file) (void) fclose (log_file); exit (1); 
#endif
} void i7 (p4,y4,c7) int p4; int y4; int c7; { int i6; int y5; int b4; int
a2; int p5; int e5; int y6; 
#ifdef x5
int n3; 
#endif 
#ifdef l7
int t6; 
#endif 
int z2; int c8; int y7; char n4 [20]; char c9; char b5; char w4; long d4;
long n5; char *j7; char i8; char c6 (); n2++; v2 = 1; c9 = '\377'; b5 =
'\376'; w4 = '\375'; if (p4 < 64) y6 = 0; else { y6 = 1; p4 -= 64; } 
#ifdef l7
if (p4 < 32) t6 = 0; else { p4 -= 32; t6 = 1; } 
#endif
#ifdef x5
if (p4 < 16) n3 = 0; else { p4 -= 16; n3 = 1; } 
#endif
if (p4 < 8) a2 = 0; else { p4 -= 8; a2 = 1; } if (p4 < 4) e5 = 0; else {
p4 -= 4; e5 = 1; } if (p4 < 2) p5 = 0; else { p4 -= 2; p5 = 1; } b4 = p4;
#ifdef x5
if (t7 (v5, x5)) n3 = 1; 
#endif 
#ifdef l7
if (t7 (v5, l7)) t6 = 1; 
#endif 
if (p5) y4 = e0 [y4]; c8 = c7; if (e5 && ((c7 != i9 && c7 != o2) || b4))
c7 = e0 [c7]; if (y4 > q0) r4 = h3 [y4]; 
#ifdef x5
else if (n3) r4 = i10 [y4]; 
#endif
#ifdef l7
else if (t6) r4 = c10 [y4]; 
#endif
else if (y4 > n1 || j0 [y4] == r5) r4 = y8 [y4]; else r4 = i10 [y4]; if
((i8 = c6 (r4)) == '\0') goto l8; while (i8 != '\0') { if (i8 == c9) { z2
= c6 (++r4); n5 = r4 + 2 * z2 -1; if (!a2) { if (y4 <= q0) c7 = e0 [y4];
else c7 = y4; } 
#ifdef s2
if (c7 <= 1) 
#else 
if (c7 <= 0) 
#endif 
r4 = n5 + 1; else { r4 = r4 - 1 + 2 * 
#ifdef s2
((c7 > z2) ? z2 - 1 : c7 - 1); 
#else 
((c7 >= z2) ? z2 - 1 : c7); 
#endif 
y5 = c6 (r4 + 1); if (y5 < 0) y5 += 256; r4 = n5 + 256 * c6 (r4) + y5 +
1; } y5 = c6 (n5 + 1); if (y5 < 0) y5 += 256; n5 = n5 + 256 * c6 (n5) +
y5 + 1; i8 = c6 (r4); } else if (i8 == b5) i8 = c6 (r4 = n5); else if (i8
== w4) { if (p4) { (void) sprintf (n4, "%d", c7); j7 = n4 - 1; while (*(++j7)
!= '\0') d5 (*j7); goto x6; } else if (c7 == i9 || c7 == o2) { j7 = ((c7
== i9) ? b1 : x2) - 1; while (*(++j7) != '\0') d5 (*j7); goto x6; } else
{ i6 = (e5 && c8 <= q0) ? c8 : c7; d4 = r4; n2++; r4 = h3 [i6]; i8 = c6
(r4); if (i8 == '!') i8 = c6 (++r4); while (i8 != '\0') { d5 (i8); i8 =
c6 (++r4); } r4 = d4; } } else d5 (i8); x6: i8 = c6 (++r4); } l8: if (y6)
longjmp (n0, 1); return; } d5 (h4) int h4; { int a3; char *s3; if (h4 ==
' ' && j3 && c3 == 0) { j3 = 0; while (c6 (++r4) == ' '); r4--; return;
} if (h4 == '\n') { if (c3 > 0) { d6 (c3); (void) x0 ('\n'); } else if (!j3)
{ (void) x0 ('\n'); } t3 = '\n'; j3 = 0; return; } j3 = 0; if (h4 == ' ')
{ if (t3 != ' ') c2 [++t2] = c3; if (c3 >= w1) { while (c6 (++r4) == ' ');
r4--; d6 (c2 [t2]); (void) x0 ('\n'); j3 = 1; t3 = '\0'; return; } goto
n6; } if (t3 == ' ') q1 [t2] = c3; if (c3 >= w1) { *k2 = '\0'; if (t2 <
0) { a3 = w1; d6 (w1); } else { a3 = q1 [t2]; d6 (c2 [t2]); (void) x0 ('\n');
j3 = 1; } s3 = &p1 [a3]; while (*s3 != '\0') { *k2++ = *s3++; c3++; } }
n6: *k2++ = t3 = h4; c3++; return; } d6 (v6) int v6; { char *s3; int i6;
int j8; int a4; int n7; int a3; s3 = p1; if (c3 < w1 || v6 == w1 || t2 ==
0) while (v6-- > 0) { i0 (*s3); } else { a4 = w1 - v6; n7 = (a4 - v2) /
t2 + v2; j8 = 1 - 2 * v2; a3 = a4 % t2; if (!v2) a3 = t2 - a3; v2 = 1 -
v2; t3 = '\0'; while (v6-- > 0) { if (t3 == ' ' && *s3 != ' ') { i6 = n7;
while (i6-- > 0) { (void) x0 (' '); } if (--a3 ==0) n7 = n7 + j8; } t3 =
*s3; i0 (*s3); } } c3 = 0; k2 = p1; t2 = -1; return; } c11 (c12, g4) char
*c12; long g4; { int u2; if (c6 (g4) == '!') g4++; for (u2 = 1; u2 <= 20;
u2++) if ((*c12++ = c6 (g4++)) == '\0') return; } void w5 (g5) int g5; {
int q5; int k4; long d7; int m2; char *b6; *v7 (i9) = -1; *v7 (o2) = -1;
#ifdef b7
if (v0) { e6 (2, d1, m0); if (v0) return; } else { k1 = e0 [i9]; r2 = e0
[o2]; } 
#else 
k1 = e0 [i9]; r2 = e0 [o2]; 
#endif 
m2 = (r3 != 0 && i4 [r3] && z0 [r3] == ','); if (z0 [r3] == '.') { e0 [i9]
= -1; e0 [o2] = -1; } g6: if (i4 [r3] == NULL) { if (u1 [0] != '\0' && u1
[0] != '\n') (void) strncpy (h1, u1, 160); c1 [0] = '\0'; while (c1 [0]
== '\0' || c1 [0] == '\n') { 
#ifdef i1
g2 = 1; 
#endif 
#ifdef y0
(void) printf ("\n(Locates: demanded %ld (+%ld), faults %ld (+%ld))", n2,
n2 - t1, l3, l3 - p0); t1 = n2; p0 = l3; 
#endif 
if (g5) (void) i7 (0, g5, 0); if (c3 > 0) { if (*(k2 - 1) != ' ') d5 (' ');
d6 (c3); } else { (void) k0 ("\n? "); } (void) fgets (c1, 160, stdin); if
(log_file) (void) fprintf (log_file, c1); 
#ifdef vms
(void) putchar ('\n'); 
#endif 
(void) strncpy (u1, c1, 160); } (void) x0 ('\n'); b8: u3 (); r3 = 0; e0
[i9] = -1; e0 [o2] = -1; e0 [v5] = 0; } g7: u4 (&q5, &k4, &d7); r3++; if
(q5 == w6 && (z0 [r3] == ' ' || z0 [r3] == ',')) goto g7; 
#ifdef i1
if (k4 == i1) { if (g2) { if (h1 [0] == '\n') goto g6; if (l1) { k1 = o1;
(void) strncpy (b1, v1, 20); if (l1) s4 ('s', v5, f1); } (void) strncpy
(c1, h1, 160); (void) strncpy (u1, h1, 160); goto b8; } else { r3--; while
(z0 [++r3] == ' '); e0 [i9] = k1; e0 [o2] = r2; return; } } else if (g2)
{ if (l1 = t7 (v5, f1)) { o1 = k1; (void) strncpy (v1, b1, 20); } } 
#endif 
if (t7 (v5, f1) && k4 <= q0 || m2) { e0 [i9] = k1; e0 [o2] = k4; if (k4
== i11 || k4 == c13) (void) strncpy (x2, i4 [r3 - 1], 20); else (void) c11
(x2, d7); e0 [v5] = 2; goto z3; } e0 [i9] = k4; if (k4 == i11 || k4 == c13)
(void) strncpy (b1, i4 [r3 - 1], 20); else (void) c11 (b1, d7); e0 [v5]
= 1; b9: if (z0 [r3] == ' ') { u4 (&q5, &k4, &d7); r3++; if (q5 == w6) goto
b9; e0 [o2] = k4; b6 = (k4 == i11 || k4 == c13) ? i4 [r3 - 1] : &j2 [d7];
if (k4 == i11 || k4 == c13) (void) strncpy (x2, i4 [r3 - 1], 20); else (void)
c11 (x2, d7); e0 [v5] = 2; } z3: 
#ifdef i1
g2 = 0; 
#endif 
s4 ('c', v5, f1); if (e0 [i9] == i11 || e0 [i9] == c13 || e0 [o2] == i11
|| e0 [o2] == c13) i4 [r3] = NULL; else if (e0 [v5] == 2 && (t7 (e0 [o2],
y9)) && !(t7 (e0 [i9], y9))) { k1 = e0 [i9]; e0 [i9] = e0 [o2]; e0 [o2]
= k1; (void) strncpy (l0, b1, 20); (void) strncpy (b1, x2, 20); (void) strncpy
(x2, l0, 20); } b1 [19] = '\0'; x2 [19] = '\0'; if (z0 [r3] == ' ') { if
(strcmp (i4 [r3], "and") == 0 && z0 [++r3] == ' ') z0 [r3] = ','; else while
(z0 [++r3] == ' '); } return; } int y10 (g5) int g5; { char s5 [10]; char
*n8; int l9 = 0; if (g5 >= 0) i7 (0, g5, 0); if (c3 > 0) { if (*(k2 - 1)
!= ' ') d5 (' '); d6 (c3); } else { (void) k0 ("? "); } m3: (void) fgets
(s5, 10, stdin); if (log_file) (void) fprintf (log_file, s5); n8 = s5; if
(*n8 == '\0' || *n8 == '\n') return (1); while (*n8 == ' ') n8++; if (*n8
== 'y' || *n8 == 'Y') return (1); if (*n8 == 'n' || *n8 == 'N') return (0);
if (l9) { (void) k0 ( "(OK, smartass... I'll assume you mean YES - so there!)\n\n");
return (1); } (void) k0 ( "Eh? Do me a favour and answer yes or no!\nWhich will it be? ");
l9 = 1; goto m3; } int b10 (p4, c14) int p4; int c14; { static char i12
[80]; char r6 [15]; char q6 [32]; FILE *u5; int e7, z4; long g8; static
int t8; static long i13; void adv_hours (); void adv_news (); switch (p4)
{ case 1: case 2: e7 = e0 [o2]; m3: if (e7 >= 0) (void) strncpy (r6, x2,
14); else { if (p4 == 1) { (void) k0 ("Name to save game under: "); } else
{ (void) k0 ("Name of saved game to restore: "); } (void) fgets (r6, 14,
stdin); if (log_file) (void) fprintf (log_file, r6); if (r6 [0] == '\0'
|| r6 [0] == '\n') { (void) k0 ("Changed your mind, eh?  Fine by me...\n");
e0 [c14] = 0; return (0); } } adv_save_path (r6, i12); if ((u5 = fopen (i12,
g0)) != NULL) { if (p4 == 2) goto p6; (void) fclose (u5); (void) k0 ("There's already a game dumped under that name.\n");
(void) k0 ("Do you really mean to overwrite it?\n"); if (!y10 (-1)) { e7
= -1; goto m3; } (void) k0 ("As you wish...\n"); } if (p4 == 2) { e0 [c14]
= 1; return (0); } if ((u5 = fopen (i12, h0)) == NULL) { e0 [c14] = 1; return
(1); } (void) time (&g8); (void) fprintf (u5, "%s\n", TITLE); (void) fwrite
(&g8, sizeof (long), 1, u5); e7 = r0; (void) fwrite (&e7, sizeof (int),
1, u5); e7 = n1; (void) fwrite (&e7, sizeof (int), 1, u5); e7 = q0; (void)
fwrite (&e7, sizeof (int), 1, u5); e7 = w7; (void) fwrite (&e7, sizeof (int),
1, u5); e7 = e1; (void) fwrite (&e7, sizeof (int), 1, u5); (void) fwrite
(e0, sizeof (int), sizeof (e0) / sizeof(int), u5); (void) fwrite (j0, sizeof
(int), sizeof (j0) / sizeof (int), u5); (void) fwrite (o0, sizeof (short),
sizeof (o0) / sizeof (short), u5); (void) fwrite (r1, sizeof (short), sizeof
(r1) / sizeof (short), u5); (void) fwrite (b0, sizeof (short), sizeof (b0)
/ sizeof (short), u5); (void) fclose (u5); if (ferror (u5)) { e0 [c14] =
1; return (1); } e0 [c14] = 0; return (0); p6: z4 = 0; (void) fscanf (u5,
"%s", q6); (void) fgetc (u5); if (strcmp (q6, TITLE) == 0) { (void) fread
(&g8, sizeof (long), 1, u5); (void) fread (&e7, sizeof (int), 1, u5); if
(e7 != r0) z4++; (void) fread (&e7, sizeof (int), 1, u5); if (e7 != n1)
z4++; (void) fread (&e7, sizeof (int), 1, u5); if (e7 != q0) z4++; (void)
fread (&e7, sizeof (int), 1, u5); if (e7 != w7) z4++; (void) fread (&e7,
sizeof (int), 1, u5); if (e7 != e1) z4++; } if (z4) { e0 [c14] = 2; (void)
fclose (u5); return (0); } (void) fread (e0, sizeof (int), sizeof (e0) /
sizeof (int), u5); (void) fread (j0, sizeof (int), sizeof (j0) / sizeof
(int), u5); (void) fread (o0, sizeof (short), sizeof (o0) / sizeof (short),
u5); (void) fread (r1, sizeof (short), sizeof (r1) / sizeof (short), u5);
(void) fread (b0, sizeof (short), sizeof (b0) / sizeof (short), u5); if
(ferror (u5)) { (void) k0 ( "Oops!! Dump file incomplete! Sorry - got to crash...\n");
(void) fclose (u5); if (log_file) (void) fclose (log_file); exit (1); }
(void) fclose (u5); i13 = g8; e0 [c14] = 0; return (0); case 3: e0 [c14]
= unlink (i12); return (0); case 4: adv_news (); return (0); case 5: adv_hours
(); return (0); case 6: t8 = e0 [c14]; return (0); case 7: e0 [c14] = t8;
return (0); case 8: (void) time (&g8); e0 [c14] = 1 + (g8 - i13) / 60; return
(0); case 9: case 10: e0 [(p4 == 9) ? i9 : o2] = e0 [c14]; return (0); case
11: if (r0 <= e0 [c14] && e0 [c14] <= n1) e0 [c14] = 0; else e0 [c14] =
1; return (0); case 12: e0 [c14] = adv_allowed (); return (0); case 13:
e0 [c14] = adv_still_allowed (); return (0); default: (void) g1 ("GLITCH! Bad special code: %d\n",
p4); return (1); } } e6 (p4, d8, q5) int p4; int d8; int q5; { int i6; int
k5; int a1; if (p4 == 0 && e0 [v5] != 1) return; k5 = -1; 
#ifdef b7
a1 = (p4 == 2) ? v0 + 1 : r0; if (a1 > n1) goto b11; for (i6 = a1; i6 <=
n1; i6++) 
#else 
for (i6 = r0; i6 <= n1; i6++) 
#endif 
#ifdef m4
if ((j0 [i6] == d8 || (d8 != r5 && t7 (i6, m4) && j0 [i6] + 1 == d8)) &&
#else
if (j0 [i6] == d8 && 
#endif
(q5 < 0 || t7 (i6, q5))) { if (k5 >= 0) return; k5 = i6; if (p4) break;
} if (k5 >= 0) { e0 [o2] = k5; (void) c11 (x2, h3 [k5]); x2 [19] = '\0';
e0 [v5] = 2; 
#ifdef b7
if (p4 > 0) v0 = k5; if (p4 == 1) { d1 = d8; m0 = q5; } 
#endif 
return; } 
#ifdef b7
b11: if (p4 == 1 && e0 [v5] == 2) e0 [v5] = 1; if (p4 > 0) v0 = 0; 
#endif 
return; } int q7 () { long h5; int i6; unsigned long c15; (void) k0 ("[A-code kernel version 10.05; MLA, 01 Apr 94]\n");
adv_data_path (j2); if ((a0 = fopen (j2, g0)) == NULL) { (void) printf ("Sorry, can't find the %s data file.\n",
TITLE); if (log_file) (void) fclose (log_file); exit (1); } 
#ifdef i2
(void) printf ("\nInitialising the universe - please wait...\n\n"); h5 =
fread (j2, sizeof (char), l2+1, a0); (void) clearerr (a0); (void) fclose
(a0); if (h5 != l2) { (void) printf ("Wrong data file length!\n"); return
(1); } 
#else 
#ifdef BUFFERS
i3 [0] = fread (j2, sizeof (char), e3, a0) - 1; s0 [0] = 0; 
#else 
i3 = fread (j2, sizeof (char), e3, a0) - 1; s0 = 0; 
#endif 
#endif 
q2 [0] = j2 [1]; g3 = 1; while (g3 < 16) { if ((q2 [g3] = q2 [g3 - 1] ^
j2 [g3 + 1]) == '\0') break; g3++; } if (strcmp (q2, TITLE) != 0) { (void)
printf ("Data file does not belong to this program!\n"); return (1); } i4
[0] = NULL; for (i6 = r0; i6 <= n1; i6++) s4 ('s', i6, d9); for (i6 = w0;
i6 <= q0; i6++) s4 ('s', i6, e8); for (i6 = d0; i6 <= w7; i6++) s4 ('s',
i6, y9); if (u0 && *u0) if ((log_file = fopen (u0, "a+")) == NULL) (void)
printf ("(Sorry, unable to open log file...)\n"); else (void) fprintf (log_file,
"\n<%s: random seed %lu>\n", TITLE, e2); return (0); } main (argc, argv)
int argc; char *argv []; { if (argc > 1) while (--argc) { if ((e2 = atoi
(argv [argc])) == 0); u0 = argv [argc]; } if (e2 == 0) (void) time (&e2);
e2 %= 32768L; if (q7 () != 0) { (void) printf ("Sorry, unable to set up the world.\n");
return (1); } x7 (); (void) setjmp (n0); if (x4) { return (0); if (log_file)
(void) fclose (log_file); } while (1) u6 (); } u3 () { short d10; char p7
(); r3 = 0; c4 = c1; while (1) { i4 [r3] = NULL; if ((z0 [r3] = p7 ()) ==
'\n') return; i4 [r3] = c4; d10 = 0; while (*c4 != ' ' && *c4 != ',' &&
*c4 != ';' && *c4 != '.' && *c4 != '\n') { if (*c4 >= 'A' && *c4 <= 'Z')
*c4 += 'a' - 'A'; c4++; d10++; } t0 [r3] = d10; r3++; } } char p7 () { char
w8; w8 = ' '; while(*c4 == ' ' || *c4 == ',' || *c4 == ';' || *c4 == '.'
|| *c4 == '\n') { if (*c4 != ' ') { w8 = *c4; if (w8 == ';') w8 = '.'; }
*c4 = '\0'; if (w8 == '\n') return w8; c4++; } return w8; } u4 (q5, k4,
d7) int *q5; int *k4; long *d7; { int s6, x8, j9; int g9; int i14; long
a5; char *b6; long d11; s6 = -1; j9 = d12 + 1; while (j9 > s6 + 1) { n2++;
x8 = (s6 + j9) / 2; if (c6 (a5 = b12 [x8]) == '!') a5++; b6 = i4 [r3]; while
(c6 (a5) == *b6) if (*b6 != '\0') { b6++; a5++; } else break; if (c6 (a5)
< *b6 && *b6 != '\0') s6 = x8; else j9 = x8; } *k4 = i11; s6++; j9 = d12;
g9 = i11; while (s6 < j9) { b6 = i4 [r3]; if (c6 (a5 = b12 [s6]) == '!')
{ a5++; i14 = 1; } else i14 = 0; d11 = a5; while (*b6 == c6 (d11)) if (*b6
== '\0') break; else { d11++; b6++; } if (*b6 != '\0') break; if (!i14 ||
c6 (d11) == '\0') { *q5 = a6 [s6]; *k4 = j10 [s6]; *d7 = p8 [s6]; if (c6
(d11) == '\0') 
#ifdef s2
{ if (d11 - a5 <= 2) { b6 = i4 [r3]; while (++s6 < j9) if (*k4 == j10 [s6]
&& *b6 == c6 (b12 [s6])) *d7 = p8 [s6]; } return; } 
#else 
return; 
#endif 
if (g9 != i11 && *k4 != g9) { *k4 = c13; return; } g9 = *k4; if (c6 (a5)
== '\0') break; } s6++; } return; } int g10 (b2,t4) int b2; int t4; { if
(m5 (b2,t4)) return (1); if (q8 (b2,t4)) return (1); return (0); } int m5
(b2,t4) int b2; int t4; { if (b2 > n1) return (0); if (j0 [b2] != r5) return
(0); if (t4 < 0) return (1); if (t4 < 1024) { if (e0 [b2] == t4) return
(1); } else if (t7 (b2, t4 - 1024)) return (1); return (0); } int q8 (b2,t4)
int b2; int t4; { if (b2 > n1) return (0); if (t4 != -1) if (t4 < 1024)
{ if (e0 [b2] != t4) return (0); } else if (!(t7 (b2, t4 - 1024))) return
(0); if (j0 [b2] == e0 [w9]) return (1); 
#ifdef m4
if (!(t7 (b2, m4))) return (0); if (j0 [b2] + 1 == e0 [w9]) return (1);
#endif
return (0); } 
#ifndef NOVARARGS
p9 (m6, a7, l10) int m6, a7, l10; { int *z1; int h6; int b2; int w10; if
(a7 < 0) goto s7; z1 = &l10; h6 = 0; while (!h6) { if ((w10 = *z1++) < 0)
{ w10 = -w10; h6 = 1; } if (m1 (w10)) goto s7; } return; 
#else 
p9 (m6,a7,l10,u7,v8,l11,h7,s8,v9,p10,t9,h8,b13,v10,a8,b14) int m6,a7,l10,u7,v8,l11,h7,s8,v9,p10,t9,h8,b13,v10,a8,b14;
{ int w10; if (a7 < 0) goto s7; if ((w10 = l10) < 0) w10 = -w10; if (m1
(w10)) goto s7; else if (l10 < 0) return; if ((w10 = u7) < 0) w10 = -w10;
if (m1 (w10)) goto s7; else if (u7 < 0) return; if ((w10 = v8) < 0) w10
= -w10; if (m1 (w10)) goto s7; else if (v8 < 0) return; if ((w10 = l11)
< 0) w10 = -w10; if (m1 (w10)) goto s7; else if (l11 < 0) return; if ((w10
= h7) < 0) w10 = -w10; if (m1 (w10)) goto s7; else if (h7 < 0) return; if
((w10 = s8) < 0) w10 = -w10; if (m1 (w10)) goto s7; else if (s8 < 0) return;
if ((w10 = v9) < 0) w10 = -w10; if (m1 (w10)) goto s7; else if (v9 < 0)
return; if ((w10 = p10) < 0) w10 = -w10; if (m1 (w10)) goto s7; else if
(p10 < 0) return; if ((w10 = t9) < 0) w10 = -w10; if (m1 (w10)) goto s7;
else if (t9 < 0) return; if ((w10 = h8) < 0) w10 = -w10; if (m1 (w10)) goto
s7; else if (h8 < 0) return; if ((w10 = b13) < 0) w10 = -w10; if (m1 (w10))
goto s7; else if (b13 < 0) return; if ((w10 = v10) < 0) w10 = -w10; if (m1
(w10)) goto s7; else if (v10 < 0) return; if ((w10 = a8) < 0) w10 = -w10;
if (m1 (w10)) goto s7; else if (a8 < 0) return; if ((w10 = b14) < 0) w10
= -w10; if (m1 (w10)) goto s7; return; 
#endif 
s7: e0 [p11] = e0 [w9]; *v7 (p11) = -1; e0 [w9] = m6; *v7 (w9) = -1; 
#if defined (f2) && defined (v5)
s4 ('s', v5, f2); 
#endif
if (a7 < -2) a7 = -a7; if (a7 > 0) i7 (0, a7, 0); if (a7 != -1) longjmp
(n0, 1); return; } s9 (b2,t4) int b2,t4; { j0 [b2] = (t4 <= q0 || t4 ==
r5) ? t4 : e0 [t4]; 
#if defined (g11) && defined (v5)
s4 ('s', v5, g11); 
#endif
return; } l12 (b2,t4,j4) int b2,t4,j4; { e0 [t4] = b2 ? e0 [j4] : j4; if
(t4 < x9) return; if (b2 && j4 >= x9 && j4 <= e1) *v7 (t4) = *v7 (j4); else
if (*v7 (t4) == -1) *v7 (t4) = 0; return; } f3 (b2,t4) int b2,t4; { e0 [b2]
= t4; *v7 (b2) = -1; h3 [b2] = h3 [t4]; return; } z5 (b2,t4) int b2,t4;
{ e0 [b2] = e0 [e0 [t4]]; return; } c16 (b2,t4) int b2,t4; { e0 [e0 [b2]]
= (t4 > e1 || t4 < x9) ? t4 : e0 [t4]; return; } t10 (b2,t4) int b2,t4;
{ e0 [b2] = j0 [(t4 < x9 || t4 > e1) ? t4 : e0 [t4]]; *v7 (b2) = -1; return;
} int t11 (b2) int b2; { if (*v7 (b2) == -1) return e0 [b2]; else return
b2; } r7 () { 
#ifndef i2
(void) fclose (a0); 
#endif 
#ifdef y0
(void) printf ("\n(Locates: demanded %ld (+%ld), faults %ld (+%ld))\n",
n2, n2 - t1, l3, l3 - p0); (void) printf ("(Locate ratio %ld%%)\n", (((1000
* l3) / n2) + 5) / 10); 
#endif 
if (log_file) (void) fclose (log_file); x4 = 1; longjmp (n0, 1); } short
*v7 (m6) int m6; { short *q9; q9 = NULL; if (m6 <= n1) q9 = &o0 [c0 * (m6
- r0)]; else if (m6 <= q0) q9 = &r1 [j1 * (m6 - w0)]; else if (m6 <= e1)
q9 = &b0 [x1 * (m6 - d0)]; return (q9); } s4 (m6, a7, l10) char m6; int
a7; int l10; { short *a9; a9 = v7 (a7); while (l10 > 15) { a9++; l10 -=
16; } if (m6 == 's') *a9 |= 1 << l10; else *a9 &= ~(1 << l10); return; }
int t7 (m6, a7) int m6; int a7; { short *a9; a9 = v7 (m6); while (a7 > 15)
{ a9++; a7 -= 16; } return (*a9 & 1 << a7); } void g12 () { v0 = 0; i4 [r3]
= NULL; return; } 
