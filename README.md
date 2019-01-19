# SML : alpha 0.0.1

## What is SML

SML is a small math language build around 3 open source softwares

1) KLIB from Attractive Chaos
2) GNU GSL : The GNU Scientific Library
3) PLPLOT : PLplot is a cross-platform software package for creating scientific plots

At first, I was interested only to learn how to transform an INFIX expression into a POSTFIX expression because I was playing with a HP50G emulator calculator. As an exercice to re-learn more about C developement I wanted to develop a small postfix calculator.

After a while, I found the KLIB library with a more complete implementation than mine. After using the KLIB librairy, I decided that I could probably create a small math language to do the link with GSL and PLPLOT.

The result : SML is born.

So, SML it's a language with only the minimum needed to link with GSL and  PLPLOT. I want the language to be as simple as possible like Python. It is very far to be a complete language like Python, it's more like a small implementation like GNUPLOT for PLPLOT.

To conclude, SML is only a hobby for me to learn. My goal is to link all the PLPLOT functions and some of the GSL math functions (complex, vector, matrix, integration and differiation formulas.)

## WHO would like to use SML

I really don't know, because there are so many softwares around this subject, but if you program in C with GSL and PLPLOT, it could be a new usefull tool for you.

## Language exemple

You can:

- Commands separator are 'new line' or ';'
- Line continuation caracter is '\\' to create expressions spread on many lines
- The caractere '#' is used to insert comment into the code
- Functions or sub-routines are created with 'def ....  enddef' and execute with 'exe'
- Loops are created with 'for ... break ... continue ... next'
- Conditions are manage with 'if ... else ... end'
- Variables are created with the equal caracter 'var = expression'

```
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

# while do
j = 1
for
  if(j == 10)
    break
  else
    if (j == 3)
      j = 7
    end
  end
  j = j + 1
next

#matrixs & vectors
v1 = vector_alloc(3)
v2 = vector_alloc(3)

m1 = matrix_alloc(2,2)
matrix_set_all(m1,3)
print(m1)
matrix_write('mat.dat', m1)
matrix_set_all(m1,2)
print(m1)
matrix_read('mat.dat', m1)
print(m1)

#vector
vector_set(v1,0,4)
vector_set(v1,1,5)
vector_set(v1,2,6)

vector_write('v1.dat',v1)
vector_read('v1.dat',v1)

#complex
c1 = complex(2,3)
c2 = complex(4,3)
c2 = complex_sqrt(c2)
c3 = complex_add(c1,c2)
print(c3)

# plotting 32bits only version for now
v1 = vector(2)
vector_put(v1,4,7)
plot_init("wingcc")
plot_env(1,10,1,10,1,0)
plot_line(2,v1,v1)
plot_spause(1)
plot_end(1)
```
