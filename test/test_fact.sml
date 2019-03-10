def(fact,n)
	if (n > 1)
		n * exe(fact,n-1)
	else
		1
	end
enddef

f = exe(fact,4)

assert.true( f == 24, "factoriel de 4 == 24")





