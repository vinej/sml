import('plplotdll.dll')

v1 = vector(2)
vput(v1,4,7)

plinit("wingcc")
plgfont(f,s,w)
print(f,' ',s,' ',w)
plgpage(xp,yp,xleng,yleng,xoff,yoff)
print(xp,' ',yp,' ',xleng,' ',yleng,' ',xoff,' ',yoff)
plenv(1,10,1,10,1,0)
pllab("time","space","Space evolution throught time")
plline(2,v1,v1)
plspause(1)
plend(1)
