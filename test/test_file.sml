
test~get = "12"
test~put = "zz"
test~zz = "zz"

print(test)

buf = buf.new(4);
assert.true( buf.len(buf) == 4, "buffer len is set")

# test file.open
f = file.open("t.dat","w+");

assert.true( file.isopen(f) == 1, "file.isopen = 1")

# get the current pos
pos = file.tell(f)
assert.true( pos == 0, "file.tell = 0")

file.write( "test", 1, 4, f)
pos = file.tell(f)
assert.true( pos == 4, "file.tell == 4")

file.rewind(f)
pos = file.tell(f)
assert.true( pos == 0, "file.tell == 0")

# return to the beginning of the file
file.seek(f, 2, SEEK_SET)
pos = file.tell(f)
assert.true( pos == 2, "file.tell == 2")

buf.clr(buf)
file.read( buf, 1, 2, f)
assert.true( strcmp(buf,"st") == 0, "bug == st")

buf.free(buf)
assert.true( buf.len(buf) == 0, "free buffer len == 0")

file.clearerr(f)
isend = file.eof(f)
assert.true( isend == 0, "isend == 0")

iserr = file.error(f)
assert.true( iserr == 0, "iserr == 0")

file.flush(f)
iserr = file.error(f)
assert.true( iserr == 0, "iserr == 0 after flush")

file.close(f)
assert.true( file.isopen(f) == 0, "file.isopen = 0")

fex = file.exist("t.dat")
assert.true( fex == 1, "file.exist == 1")

file.rename("t.dat", "tt.dat")

fex = file.exist("t.dat")
assert.true( fex == 0, "file.exist == 0")

fex = file.exist("tt.dat")
assert.true( fex == 1, "file.exist == 1")

file.remove("tt.dat")
fex = file.exist("tt.dat")
assert.true( fex == 0, "file.exist == 0")

ftmp = file.tmp()
file.write( "test", 1, 4, ftmp)
#assert.true( count == 4, "file.write count == 4")
file.close(ftmp)

tnmae = file.tmpname()
print(tnmae)

# test file.printf 1
if(file.exist("t2.dat") == 1)
  file.remove("t2.dat")
end
fex = file.exist("t2.dat")
assert.true( fex == 0, "file.exist == 0")

f2 = file.open("t2.dat","w+")

file.fprintf(f2, "test %d", 12)
file.flush(f2);
file.rewind(f2)
buf2 = buf.new(100)
file.read(buf2, 1, 7, f2)
assert.true( strcmp(buf2, "test 12") == 0, "compare test 12")
buf.free(buf2)
file.close(f2)

# test file.printf 2
if(file.exist("t2.dat") == 1)
  file.remove("t2.dat")
end
fex = file.exist("t2.dat")
assert.true( fex == 0, "file.exist == 0")

f2 = file.open("t2.dat","w+")

file.fprintf(f2, "test %d:%s", 12, "JY")
file.flush(f2);
file.rewind(f2)
buf2 = buf.new(100)
file.read(buf2, 1, 10, f2)
assert.true( strcmp(buf2, "test 12:JY") == 0, "compare test 12:JY")
buf.free(buf2)
file.close(f2)

# test file.printf 3
if(file.exist("t2.dat") == 1)
  file.remove("t2.dat")
end
fex = file.exist("t2.dat")
assert.true( fex == 0, "file.exist == 0")

f2 = file.open("t2.dat","w+")
file.fprintf(f2, "test %d:%s:%f", 12, "JY", 1.1)
file.flush(f2);
file.rewind(f2)
buf2 = buf.new(100)
file.read(buf2, 1, 19, f2)
assert.true( strcmp(buf2, "test 12:JY:1.100000") == 0, "compare test 12:JY:1.100000")
buf.free(buf2)
file.close(f2)

buf2 = buf.new(1024)
file.sprintf(buf2, "test %d:%s:%f", 12, "JY", 1.1)
assert.true( strcmp(buf2, "test 12:JY:1.100000") == 0, "compare test 12:JY:1.100000")
buf.free(buf2)

file.printf("Success: compare test %d:%s:%f\n", 12, "JY", 1.1)
file.printf("Success: compare test\n")

print("enter the number 2")
n2 = 1
file.scanf("%d", n2)
assert.true( n2 == 2, "scanf n2 == 2")

print("enter the number 2.5")
n2 = 1.1
file.scanf("%lf", n2)
assert.true( n2 == 2.5, "scanf n2 == 2.5")

print("enter zz")
n2 = ""
file.scanf("%s", n2)
assert.true( n2 == "zz", "scanf n2 == zz")


n2 = 1
file.sscanf("2","%d", n2)
assert.true( n2 == 2, "sscanf n2 == 2")

n2 = 1.1
file.sscanf("2.5","%lf", n2)
assert.true( n2 == 2.5, "sscanf n2 == 2.5")

n2 = ""
file.sscanf("zz", "%s", n2)
assert.true( n2 == "zz", "sscanf n2 == zz")

f3 = file.open("t3.dat", "w")
file.write( "2", 1, 1, f3)
file.close(f3)
n2 = 1
f3 = file.open("t3.dat", "r")
file.fscanf(f3,"%d", n2)
assert.true( n2 == 2, "fscanf n2 == 2")
file.close(f3)

f3 = file.open("t3.dat", "w")
file.write( "2.5", 1, 3, f3)
file.close(f3)
n2 = 1.1
f3 = file.open("t3.dat", "r")
file.fscanf(f3,"%lf", n2)
assert.true( n2 == 2.5, "fscanf n2 == 2.5")
file.close(f3)

f3 = file.open("t3.dat", "w")
file.write( "zz", 1, 2, f3)
file.close(f3)
n2 = ""
f3 = file.open("t3.dat", "r")
file.fscanf(f3, "%s", n2)
assert.true( n2 == "zz", "fscanf n2 == zz")

f3 = file.open("t3.dat", "w")
file.write( "2", 1, 1, f3)
file.close(f3)
f3 = file.open("t3.dat", "r")
c = file.fgetc(f3)
assert.true( c == 50, "fgetc c == 50")
file.close(f3)

f3 = file.open("t3.dat", "w")
file.write( "zzz", 1, 3, f3)
file.close(f3)
n2 = "  "
f3 = file.open("t3.dat", "r")
file.fgets(n2,f3)
assert.true( strcmp(n2, "zzz") == 0, "fgets == zzz")
file.close(f3)


