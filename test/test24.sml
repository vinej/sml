def(add, v1, v2)
  v1 + v2
enddef

print('Start of program')

sum = exe(add,3,4)
print('the total is ',sum)
print('Sinus of 60 =', sin(60))

# for lop from 1 to 10, increment of 1
for(j,1,10,1)
  print(j)
next

# like do while
j = 1
for  
  j = j + 1
  if(j == 5)
    print(j)
    break
  end
next

print('bug here')
j = 1
print('bug here 2')
for
  print('bug here 3')
  if(j == 10)
    print('bug here 4')
    break
    print('bug here 4b')
  end
  print('bug here 5')
  j = j + 1
next
print('bug here 6')
