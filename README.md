sml
sml : simple math language

Language exemple
```
def(add, v1, v2)
  v1 + v2
enddef

print('Start of program')

sum = exe(add,3,4)
print('the total is ',sum)
print('Sin of 60', sin(60))

for(j,1,10,1)
  print(j)
next

# like do while
j = 1
for  
  j = j + 1
  if(j == 5)
    print(j)
    break
  end
next

# while do
j = 1
for
  if(j == 10)
    print(j)
    break
  else
    if (j == 3)
      j = 7
    end
  end
  j = j + 1
next

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

vector_write('v1.dat',v1)
vector_read('v1.dat',v1)

c1 = complex(2,3)
c2 = complex(4,3)
c2 = complex_sqrt(c2)
c3 = complex_add(c1,c2)
print(c3)

```
