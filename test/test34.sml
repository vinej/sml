
print("start file read string")
file = fopen("c:\sml\test\test15.sml","r")
buf = strbuf(1000)
for
  if (feof(file) == 1)
    break
  end
  fgets(buf, 999, file)
  print(buf)
next
fclose(file)

print("end\n")



