module shift$main (input,output);

(******************************************************************************)
(*                                                                            *)
(*  Shift - emulate CDC FORTRAN shift function.                               *)
(*                                                                            *)
(*  Restrictions:  Cannot shift greater than 32, the VAX longword length.     *)
(*                                                                            *)
(******************************************************************************)

type bit32 = packed array [1..32] of 0..1;

[global] function shift ( val : bit32; cnt : integer ): bit32;

var loop : integer;
    temp : bit32;

begin
  if cnt > 32 then
    begin
      writeln ('Error in shift argument - shift count greater than word length');
      halt;
    end;
  if cnt > 0 then
    begin
      for loop := 1 to 32 - cnt do
        begin
          temp [loop + cnt] := val [loop];
        end;
      for loop := 1 to cnt do
        begin
          temp [loop] := val [32 - cnt + loop];
        end;
     end
   else if cnt = 0 then
     begin
       temp := val;
     end
   else
     begin
       for loop := -cnt + 1 to 31 do
         begin
           temp [loop + cnt] := val [loop];
         end;
       for loop := 31 downto 32 + cnt do
          begin
            temp [loop] := 0;
          end;
     end;
  shift := temp;
end;

end.
