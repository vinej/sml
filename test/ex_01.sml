def(plot1)
    for(i,0,59,1)
		x = g_xoff + g_xscale * ( i + 1 ) / 60.0
        vset(g_x, i, x)
		y = g_yoff + g_yscale * pow( vget(g_x,i), 2.0 )
        vset(g_y, i, y)
    next
	print(g_x)
	print(g_y)
	print('first for')
    xmin = vmin(g_x)
    xmax = vmax(g_x)
    ymin = vmin(g_y)
    ymax = vmax(g_y)
	print(xmin,' ',xmax,' ', ymin,' ', ymax)

    for (i,0,5,1)
        vset(g_xs,i,vget(g_x, i * 10 + 3))
        vset(g_ys,i,vget(g_y, i * 10 + 3))
    next
	print(g_xs)
	print(g_ys)

	print('second for')
	print('starting plotting #1')
    plcol0( 1 )
    plenv( xmin, xmax, ymin, ymax, 0, 0 )
    plcol0( 2 )
    pllab( "(x)", "(y)", "#frPLplot Example 1 - y=x#u2" )
    plcol0( 4 )
    plpoin( 6, g_xs, g_ys, 9 )
    plcol0( 3 )
    plline( 60, g_x, g_y )
enddef

def(plot2)
    plcol0( 1 )
    plenv( -2.0, 10.0, -0.4, 1.2, 0, 1 )
    plcol0( 2 )
    pllab( "(x)", "sin(x)/x", "#frPLplot Example 1 - Sinc Function" )
    for (i,0,100,1)
        vset(g_x, i, ( i - 19.0 ) / 6.0)
        vset(g_y, i, 1.0)
        if ( vget(g_x,i) != 0.0 )
            vset(g_y, i, sin( vget(g_x,i) / vget(g_x,i)))
		end
    next
    plcol0( 3 )
    plwidth( 2 )
    plline( 100, g_x, g_y )
    plwidth( 1 )
enddef

def(plot3)
    space0 = 0
    mark0 = 0
    space1 = 1500
    mark1 = 1500
    pladv( 0 )
    plvsta()
    plwind( 0.0, 360.0, -1.2, 1.2 )
    plcol0( 1 )
    plbox( "bcnst", 60.0, 2, "bcnstv", 0.2, 2 )
    plstyl( 1, mark1, space1 )
    plcol0( 2 )
    plbox( "g", 30.0, 0, "g", 0.2, 0 )
    plstyl( 0, mark0, space0 )
    plcol0( 3 )
    pllab( "Angle (degrees)", "sine", "#frPLplot Example 1 - Sine function" )
    for (i,0,100,1)
        vset(g_x, i, 3.6 * i)
        vset(g_y, i, sin( vget(g_x,i) * m_pi / 180.0 ))
    next	
    plcol0( 4 )
    plline( 101, g_x, g_y )
enddef

# global variables used un sub routines
g_x = vector(60)
vset_zero(g_x)
g_y = vector(60)
vset_zero(g_y)
g_xs = vector(6)
vset_zero(g_xs)
g_ys = vector(6)
vset_zero(g_ys)
g_xcale - 0.0
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

print('plot 1/2')
exe(plot1)


#g_xscale = 1.0;
#g_yscale = 0.0014;
#g_yoff   = 0.0185;

#digmax = 5
#plsyax( digmax, 0 )

#print('plot 2/2')
#exe(plot1)
#print('plot 2')
#exe(plot2)
#print('plot 3')
#exe(plot3)
plspause(1)
plend()
