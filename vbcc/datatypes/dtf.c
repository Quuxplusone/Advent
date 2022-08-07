from)
{
  DTTTYPE to=0;
  int i,j=0;
  for(i=sizeof(from.a)-1;i>=0;i--)
    to=(to<<8)|(from.a[i]&255);

  to<<=(sizeof(to)*8)-BITSIZE;
  to>>=(sizeof(to)*8)-BITSIZE;
  return to;
}
