buf = newbuffer(4);
assert_true( lenbuffer(buf) == 4, "buffer len is set")

# test fopen
f = fopen("t.dat","w+");

assert_true( fisopen(f) == 1, "fisopen = 1")

# get the current pos
pos = ftell(f)
assert_true( pos == 0, "ftell = 0")

count = fwrite( "test", 1, 4, f)
assert_true( count == 4, "fwrite count == 4")

pos = ftell(f)
assert_true( pos == 4, "ftell == 4")

frewind(f)
pos = ftell(f)
assert_true( pos == 0, "ftell == 0")

# return to the beginning of the file
fseek(f, 2, SEEK_SET)
pos = ftell(f)
assert_true( pos == 2, "ftell == 2")

clrbuffer(buf)
qte = fread( buf, 1, 2, f)
assert_true( qte == 2, "qte == 2")
assert_true( strcmp(buf,"st") == 0, "bug == st")

freebuffer(buf)
assert_true( lenbuffer(buf) == 0, "free buffer len == 0")

fclearerr(f)
isend = feof(f)
assert_true( isend == 0, "isend == 0")

iserr = ferror(f)
assert_true( iserr == 0, "iserr == 0")

fflush(f)
iserr = ferror(f)
assert_true( iserr == 0, "iserr == 0 after flush")

fclose(f)
assert_true( fisopen(f) == 0, "fisopen = 0")

fexist = fexist("t.dat")
assert_true( fexist == 1, "fexist == 1")

frename("t.dat", "tt.dat")

fex = fexist("t.dat")
assert_true( fex == 0, "fexist == 0")

fex = fexist("tt.dat")
assert_true( fex == 1, "fexist == 1")

fremove("tt.dat")
fex = fexist("tt.dat")
assert_true( fex == 0, "fexist == 0")

ftmp = ftmpfile()
count = fwrite( "test", 1, 4, ftmp)
assert_true( count == 4, "fwrite count == 4")
fclose(ftmp)

tnmae = ftmpnam()
print(tnmae)

# test fprintf 1
if(fexist("t2.dat") == 1)
  fremove("t2.dat")
end
fex = fexist("t2.dat")
assert_true( fex == 0, "fexist == 0")

f2 = fopen("t2.dat","w+")

fprintf(f2, "test %d", 12)
fflush(f2);
frewind(f2)
buf2 = newbuffer(100)
qte = fread(buf2, 1, 7, f2)
assert_true( qte == 7, "qte == 7")
assert_true( strcmp(buf2, "test 12") == 0, "compare test 12")
freebuffer(buf2)
fclose(f2)

# test fprintf 2
if(fexist("t2.dat") == 1)
  fremove("t2.dat")
end
fex = fexist("t2.dat")
assert_true( fex == 0, "fexist == 0")

f2 = fopen("t2.dat","w+")

fprintf(f2, "test %d:%s", 12, "JY")
fflush(f2);
frewind(f2)
buf2 = newbuffer(100)
qte = fread(buf2, 1, 10, f2)
assert_true( qte == 10, "qte == 10")
assert_true( strcmp(buf2, "test 12:JY") == 0, "compare test 12:JY")
freebuffer(buf2)
fclose(f2)

# test fprintf 3
if(fexist("t2.dat") == 1)
  fremove("t2.dat")
end
fex = fexist("t2.dat")
assert_true( fex == 0, "fexist == 0")

f2 = fopen("t2.dat","w+")
fprintf(f2, "test %d:%s:%f", 12, "JY", 1.1)
fflush(f2);
frewind(f2)
buf2 = newbuffer(100)
qte = fread(buf2, 1, 19, f2)
assert_true( qte == 19, "qte == 19")
assert_true( strcmp(buf2, "test 12:JY:1.100000") == 0, "compare test 12:JY:1.100000")
freebuffer(buf2)
fclose(f2)

