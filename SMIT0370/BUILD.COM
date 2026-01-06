$	if p1 .eqs. "LINK" then goto 10$
$	fortran 'p1' funadv
$	fortran 'p1' bug
$	fortran 'p1' carry
$	fortran 'p1' drop
$	fortran 'p1' getin
$	fortran 'p1' image_dir
$	fortran 'p1' juggle
$	fortran 'p1' ldcomn
$	fortran 'p1' minpmid
$	fortran 'p1' move
$	fortran 'p1' pspeak
$	fortran 'p1' put
$	fortran 'p1' random
$	fortran 'p1' rspeak
$	fortran 'p1' speak
$	fortran 'p1' svcomn
$	fortran 'p1' vocab
$	fortran 'p1' wizard
$	fortran 'p1' yes
$	pascal 'p1' shift
$	pascal 'p1' user
$ 10$:
$	link 'p2' funadv,bug,carry,drop,getin,image_dir,juggle,ldcomn,minpmid,-
move,pspeak,put,random,rspeak,speak,svcomn,vocab,wizard,yes,shift,user
