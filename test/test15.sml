def(test, out) 
	j = 0
	aa = 1
	for(aa,1,1000000)
		j =  aa
	next
	print(j)
	if (out == 0)
		exe(test,1)
	end
enddef

exe(test,0)
print('end')

