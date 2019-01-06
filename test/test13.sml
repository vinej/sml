def(sum, _a, _b)
  for(_c,1,3,1)
    print(_c)
  next(_c)
  _a + _b
end

b = 0
for(a,1,4,1)
  b = b + 1
  
  for(c,1,2,1)
    if (c==2)
		print(c)
	endif
  next(c)
next(a)

print(b)

print(exe(sum,4,5))

