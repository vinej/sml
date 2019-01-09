
def(sum, _a, _b)
  _a + _b / 12.34
enddef
b=33
for(b,1,10,1)
  j = exe(sum,1.0,2.0)+b
  print(j)
next
print(j)
print('end')
 



