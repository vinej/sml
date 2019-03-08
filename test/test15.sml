def(test, out) 
	j = 0
	aa = 1
	for(aa,1,1000000)
		j =  aa
	next
	print(j)
	if (out > 1)
		exe(test,out-1)
	end
enddef

exe(test,4)
print('end')

