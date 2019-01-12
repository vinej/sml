

;

def(t)
 p = p + 23
enddef

p = 12  # c'est une remarque
exe(t)

#test

#debut du traitement
i = 3   #compteur
for
  if(p == 35)
    print(p)
    p = p * 30
    print(p)
    if(p > 6)
		p = p * 4
		print(p)
    else
        p = p * 12
		print(p)
    end
  else
    p = p * 4
    print(p)
  end
  i = i - 1
  if(i == 0)
    break
  end
next
print(i)
p*2
