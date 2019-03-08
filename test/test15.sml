def(test,a,b) 
  c = 12
  d = 12
  e = c + d
  print(e)
  a + b
enddef


j = 0
a = 1
for(a,1,1000000)
  j =  a
next
print(j)

print(exe(test,2,3))
print('end')

