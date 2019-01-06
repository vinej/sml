def(sum, _a, _b)
   _a + _b
end

a = exe(sum,2,3)
b = exe(sum,4,4)

println(a * b)

if (12==12)
  println("ok")
endif

do
  a = a + 2
  if (a>10)
    break
  endif
loop

print('fin de a')
println(a)

c1 = complex(2,3)
m1 = matrix(2,2)
v1 = vector(4)

print(c1,m1,v1)

if (12==12)
  println("ok")
endif

println(12,13,14)
