﻿def(test,out) 
    j = 1
	for(aa,1,1000000)
		j =  aa
	next
	print(j)
	if (out > 1)
		exe(test,out-1)
	end
	print('exit')
enddef

exe(test,2)
print('end')

