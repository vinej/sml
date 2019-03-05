buffer = buf.new(10000)

print("TEST 08")
file = file.open("c:\sml\test\test15.sml", "r")
name = "kkkkk"
qte = 129
eq = ''
file.fscanf(file, "%s %s %d", name, eq, qte)
file.printf("\n%s %s %d\n",name, eq, qte)
file.close(file)


