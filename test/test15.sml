def(test) 
	for(aa,1,1000000)
		j =  aa
	next
	print(j)
	if (out > 1)
		exe(test,out-1)
	end
	print('end')
enddef

exe(test)
print('end')

