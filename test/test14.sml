z = 2
for(a,1,10,1)
    if(a == 10.0)
	  break
	end
	for(b,1,2,1)
	  if (b == 2)
	    break
	  else
	    print('else')
	  end
	next
next
print(b)



v = 12.0
v = v & 12
print(v)

print(time_year())
print(time_month())
print(time_day())
print(time_hour())
print(time_min())
print(time_sec())

p.year = 12
p.month = 23
print(p)

a = p

