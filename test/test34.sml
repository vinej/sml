print("start file read string")
file = file.open("c:\sml\test\test15.sml","r")
i = 1
for
  if (file.eof(file) == 1)
    break
  end
  buf =""
  file.fgets(buf, file)
  file.printf("%d: %s",i, buf)
  i = i + 1
next
file.close(file)

print("")
print("end")

a = 1
b = 1
file.printf("Saisir une code: ")
file.sscanf("1 2","%d %d",a,b)
file.printf("a=%d, b=%d\n",a,b)


