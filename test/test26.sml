max = 1000

strbuf(buf,max)

file = fopen('c:\sml\test\ex_01.sml','r')

for
  if (feof(file) == 1)
    break
  end
  xfgets(buf, max, file)
  if(ferror(file))
    print('error reading file')
	break
  end
  printf(buf)
next
fclose(file)

printf("Saisir un code:")
xscanf("%s",buf)
print(buf)
xprintf("%s",buf)

strfree(buf)

mybuf=''
printf("saisir un code :")
scanf("%s",mybuf)

printf("%s\n",mybuf)
