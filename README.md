sml
sml : simple math language

Language exemple
```
def(add, v1, v2)
  v1 + v2
enddef

print('Start of program')

print(exe(add,3,4))

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

# while do
j = 1
for
  if(j == 10)
    print(j)
    break
  else
    if (j == 3)
      j = 7
    end
  end
  j = j + 1
next
```
