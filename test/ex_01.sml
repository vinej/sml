import('plplotdll.dll')

def(plot1)
    print('string plot1')
    for(i,0,59,1)
        g_x[i] = g_xoff + g_xscale * ( i + 1.0 ) / 60.0
        g_y[i] = g_yoff + g_yscale * pow( g_x[i], 2.0 )
    next
    xmin = g_x[0]
    xmax = g_x[59]
    ymin = g_y[0]
    ymax = g_y[59]
	print(xmin,' ',xmax,' ', ymin,' ', ymax)
	i = 0
    for (i,0,5,1)
        g_xs[i] = g_x[i * 10 + 3]
        g_ys[i] = g_y[i * 10 + 3]
    next
	print('second for')
	print('starting plotting #1')
    plcol0( 1 )
    plenv( xmin, xmax, ymin, ymax, 0, 0 )
    plcol0( 2 )
    pllab( "(x)", "(y)", "PLplot Example 1 - y=x" )
    plcol0( 4 )
    plpoin( 6, g_xs, g_ys, 9 )
    plcol0( 3 )
    plline( 60, g_x, g_y )
enddef


# global variables used un sub routines
g_x = vector(101)
vset_zero(g_x)
g_y = vector(101)
vset_zero(g_y)
g_xs = vector(6)
vset_zero(g_xs)
g_ys = vector(6)
vset_zero(g_ys)
g_xscale = 0.0
g_yscale = 0.0
g_xoff = 0.0
g_yoff = 0.0

print('starting...')
print('... plstart')
plstart("wingcc", 2, 2 )
print('... plfont')
plfontld( 0 )
print('setting global variables...')
g_xscale = 6.0
g_yscale = 1.0
g_xoff   = 0.0
g_yoff   = 0.0

print('plot 1: 1/2')
exe(plot1)

plspause(1)
plend()