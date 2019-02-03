buffer = newbuffer(0)

print("TEST 08")
file = fopen("c:\sml\test\test15.sml", "r")
name = "kkkkk"
qte = 129
eq = ''
fscanf(file, "%s %s %d", name, eq, qte)
printf("\n%s %s %d\n",name, eq, qte)
fclose(file)


