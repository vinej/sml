v1 = vector(2)
vector_put(v1,4,7)

plot_init("wingcc")
plot_env(1,10,1,10,1,0)

plot_ctime(2019,1,12,16,26,34,ctime)
print(ctime)
plot_btime(p.year,p.month,p.day,p.hour,p.min,p.sec,ctime)
print('time')
print(p.year,'-',p.month,'-',p.day,' ',p.hour,':',p.min,':',p.sec)

plot_line(2,v1,v1)
plot_spause(1)
plot_end(1)

