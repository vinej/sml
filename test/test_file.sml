﻿buf = newbuffer(4);
assert_true( lenbuffer(buf) == 4, "buffer len is set")

# test fopen
f = fopen("t.dat","w");

assert_true( fisopen(f) == 1, "fisopen = 1")

# get the current pos
pos = fgetpos(f)

count = fwrite( "test", 1, 4, f)
assert_true( count == 4, "fwrite count == 4")

# return to the begenning of the file
#fsetpos(f, pos)
qte = fread( buf, 1, 4, f)
print(qte)
print(buf)
assert_true( qte == 4, "fread == 4")

freebuffer(buf)
assert_true( lenbuffer(buf) == 0, "free buffer len == 0")

fclose(f)
assert_true( fisopen(f) == 0, "fisopen = 0")





