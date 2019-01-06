def(test,_a, _b)
  _a * _b * sin(3)
enddef
  
b = 1
for(a,1,10,0)
    if(a == 10)
	  break
	end
	for(b,1,2,1)
	  if (b == 2)
	    break
	  else
	    print('else')
	    continue
	  end
	next
    print(exe(test,3,4))
next
print(b)

print(m_pi)
print(m_e)

c1 = complex(1,2)
c2 = complex_add(c1,c2)
print(c1)

m1 = matrix(2,2)
print(m1)
matrix_print('test.mat', m1)
matrix_set_zero(m1)
matrix_scan('test.mat', m1)
print(m1)

v1 = vector(2)
print(v1)




