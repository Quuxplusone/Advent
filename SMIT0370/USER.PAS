[inherit('sys$library:starlet')]
module getuser;

type
   word_integer = [word] 0..65535;
   long_integer = [long] integer;
   char12 = packed array [1..12] of char;
    jpi_rec = packed record
       len : word_integer;
       oper : word_integer;
       adr : long_integer;
       adrl : long_integer;
       zero : long_integer;
    end;

var rts,i : integer;
    jrec : jpi_rec;

[global] procedure user( var name: char12);

begin
  jrec.len := 12;
  jrec.zero := 0;
  jrec.adr := iaddress(name);
  jrec.adrl := iaddress(i);
  jrec.oper := jpi$_username;
  rts := $getjpi(itmlst := jrec);
end;

end.
