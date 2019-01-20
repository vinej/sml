v1 = vector(2)
vector_put(v1,4,7)

plinit("wingcc")
plenv(1,10,1,10,1,0)
plline(2,v1,v1)
plspause(1)
plend(1)
