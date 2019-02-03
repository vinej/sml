print('start')
r1 = 12
r6 = r1 * 2
print(r6)
r2 = 'test'

c1 = complex(2,3)

v1 = vector(3)
v2 = vector(3)

m1 = matrix(2,2)

mset_all(m1,3)
print(m1)
mwrite('mat.dat', m1)
mset_all(m1,2)
print(m1)
mread('mat.dat', m1)
print(m1)


vset(v1,0,4)
vset(v1,1,5)
vset(v1,2,6)

#print(v1)
vwrite('v1.dat',v1)
vread('v1.dat',v1)
#print(v1)

vfree(v1)
vfree(v2)
