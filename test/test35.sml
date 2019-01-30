print("TEST 00")
gets(test)
print(test)

print("TEST 01")
a = 1
b = 1
sscanf("1 2","%d %d",a,b)
printf("a = %d, b=%d\n",a,b)
printf("a = %d, b=%d\n",a,b)
printf("a = %d, b=%d\n",a,b)

print("TEST 02")
c = 1
d = 1
file = fopen('c:\sml\test\scan.txt',"r")
fscanf(file,"%d %d",c,d)
printf("c=%d, d=%d\n",c,d)
fclose(file)

print("TEST 03")
e = 1;f = 1;s = ' '
scanf("%d %d %[0-9a-zA-Z ]",e,f,s)
printf("e=%d, f=%d s=%s\n",e,f,s)

print("TEST 04")
tmpnam(tmp)
print(tmp)

print("TEST 05")
file = fopen('c:\sml\test\out.txt',"w")
fprintf(file,"%d %d",c,d)
fclose(file)

print("TEST 07")
file = fopen('c:\sml\test\out.txt',"r")
fgets(tt, file)
fclose(file)
print(tt)
