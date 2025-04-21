/com_.*\[undefined\]/ {printf "common(%s,\"%s\",0%s);\n", substr($1,2),$1,$5;}
