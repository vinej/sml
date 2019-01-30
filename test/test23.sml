import('plplotdll.dll')

v1 = vector(2)
vput(v1,4,7)

plinit("wingcc")
plenv(1,10,1,10,1,0)

plctime(2019,1,12,16,26,34,ctime)
print(ctime)
plbtime(p.year,p.month,p.day,p.hour,p.min,p.sec,ctime)
print('time')
print(p.year,'-',p.month,'-',p.day,' ',p.hour,':',p.min,':',p.sec)

plline(2,v1,v1)
plspause(1)
plend()

