v1 = vector_alloc(3)
v2 = vector_alloc(3)

m1 = matrix_alloc(2,2)

matrix_set_all(m1,12)

vector_set(v1,0,4)
vector_set(v1,1,5)
vector_set(v1,2,6)

print(v1)

vector_set_all(v2,3)

#vector_zzz(12)

vector_mul(v1,v2)

i = 2
do
  val1 = vector_get(v1,1)
  val1 = vector_get(v1,0)
  val1 = vector_get(v1,2)
  if (i == 0)
    break
  endif
  i = i - 1
loop

print(val1)

val5 = vector_max(v1)

print(val5)

if (!vector_equal(v1,v2))
  print("vi is not l\"heure equal to v2")
endif

#vector_free(v1)
#vector_free(v2)

val1*2

