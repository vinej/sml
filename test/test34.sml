print("start file read string")
file = fopen("c:\sml\test\test15.sml","r")
i = 1
for
  if (feof(file) == 1)
    break
  end
  fgets(buf, file)
  printf("%d: %s",i, buf)
  i = i + 1
next
fclose(file)

print("")
print("end")

a = 1
b = 1
printf("Saisir une code: ")
sscanf("1 2","%d %d",a,b)
printf("a=%d, b=%d\n",a,b)

