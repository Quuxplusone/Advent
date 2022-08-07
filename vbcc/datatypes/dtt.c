from)
{
 DTFTYPE to;
 int i;
 from&=(((DTTTYPE)1)<<BITSIZE)-1;
 for(i=0;i<sizeof(to.a);i++){
   to.a[i]=from&255;
   from>>=8;
 }
 return to;
}
