a = 1
b = 1
sscanf("1 2","%d %d",a,b)
printf("a = %d, b=%d\n",a,b)

c = 1
d = 1
file = fopen('c:\sml\test\scan.txt',"r")
fscanf(file,"%d %d",c,d)
printf("c = %d, d=%d\n",c,d)
fclose(file)

e = 1
f = 1
s = ' '
scanf("%d %d %[0-9a-zA-Z ]",e,f,s)
printf("e=%d, f=%d s=%s\n",e,f,s)

file = fopen('c:\sml\test\out.txt',"w")
fprintf(file,"%d %d",c,d)
fclose(file)


