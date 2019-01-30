
print("start file read string")
file = fopen("c:\sml\test\test15.sml","r")
buf = strbuf(1000)
i = 1
for
  if (feof(file) == 1)
    break
  end
  fgets(buf, 999, file)
  printf("%d: %s",i, buf)
  i = i + 1
next
fclose(file)

print("")
print("end")

a = 1
b = 1
scanf("1 2","%d %d",a,b)
printf("a = %d, b=%d\n",a,b)

