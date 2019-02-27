def(test,a,b)
  a + b
enddef


def(test2,a,b,c)
  a + b + c
enddef
 
c = exe(test,2,3)
print("result= ", c)

c = exe(test2,2,3,6)
print("result= ", c)