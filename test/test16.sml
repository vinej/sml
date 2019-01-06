v1 = vector(2)
vector_put(v1,4,7)

plot_init("wingcc")
plot_env(1,10,1,10,1,0)
plot_line(2,v1,v1)
plot_pause(1)
plot_end(1)
