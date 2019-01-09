def(sum, _a, _b)
   _a + _b
enddef

a = exe(sum,2,3)
b = exe(sum,4,4)

print(a * b)

if (12==12)
  print("ok")
end

for
  a = a + 2
  if (a>10)
    break
  end
next

print('fin de a')
print(a)

c1 = complex(2,3)
m1 = matrix(2,2)
v1 = vector(4)

print(c1,m1,v1)

if (12==12)
  print("ok")
end

print(12,13,14)
