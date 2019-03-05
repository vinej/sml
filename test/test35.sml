print("TEST 00")
file.gets(test)
print(test)

print("TEST 01")
a = 1
b = 1
file.sscanf("1 2","%d %d",a,b)
file.printf("a = %d, b=%d\n",a,b)
file.printf("a = %d, b=%d\n",a,b)
file.printf("a = %d, b=%d\n",a,b)

print("TEST 02")
c = 1
d = 1
file = file.open('c:\sml\test\scan.txt',"r")
file.fscanf(file,"%d %d",c,d)
file.printf("c=%d, d=%d\n",c,d)
file.close(file)

print("TEST 03")
e = 1;f = 1;s = ' '
file.scanf("%d %d %[0-9a-zA-Z ]",e,f,s)
file.printf("e=%d, f=%d s=%s\n",e,f,s)

print("TEST 04")
tmp = file.tmpname()
print(tmp)

print("TEST 05")
file = file.open('c:\sml\test\out.txt',"w")
file.fprintf(file,"%d %d",c,d)
file.close(file)

print("TEST 07")
file = file.open('c:\sml\test\out.txt',"r")
tt = ""
file.fgets(tt, file)
file.close(file)
print(tt)
