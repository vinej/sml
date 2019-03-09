def(test) 
	for(aa,1,2)
		g_x[i] = aa
		print(g_x[i])
	next
enddef

def(test2) 
	for(aa,1,2)
		g_x[i] = aa+100
		print(g_x[i])
	next
enddef

g_x = vector(101)
vset_zero(g_x)

exe(test)
exe(test2)
print('end')

