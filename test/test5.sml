
r1 = 12
r6 = r1 * 2
print(r6)
r2 = 'test'

c1 = complex(2,3)

v1 = vector_alloc(3)
v2 = vector_alloc(3)

m1 = matrix_alloc(2,2)


matrix_set_all(m1,3)
print(m1)
matrix_write('mat.dat', m1)
matrix_set_all(m1,2)
print(m1)
matrix_read('mat.dat', m1)
print(m1)


vector_set(v1,0,4)
vector_set(v1,1,5)
vector_set(v1,2,6)

#print(v1)
vector_write('v1.dat',v1)
vector_read('v1.dat',v1)
#print(v1)

vector_free(v1)
vector_free(v2)
