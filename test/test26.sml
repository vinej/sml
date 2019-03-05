max = 1000

strbuf(buf,max)

file = file.open('c:\sml\test\ex_01.sml','r')

for
  if (file.eof(file) == 1)
    break
  end
  file.fgets(buf, file)
  if(file.error(file))
    print('error reading file')
	break
  end
  file.printf(buf)
next
file.close(file)

file.printf("Saisir un code:")
file.scanf("%s",buf)
print(buf)
file.printf("%s",buf)

strfree(buf)

mybuf=''
file.printf("saisir un code :")
file.scanf("%s",mybuf)

file.printf("%s\n",mybuf)
