#include "stdafx.h"
#include <math.h>

double ke_constants(const char * name) {
	if (strcmp(name, "m_pi") == 0) return	M_PI;
	else if (strcmp(name, "m_e") == 0) return M_E;
	else if (strcmp(name, "m_radian") == 0) return 1;
	else if (strcmp(name, "m_degree") == 0) return 2;
	else if (strcmp(name, "m_gradian") == 0) return 3;
	return 0;
}

/*
#define PLESC_SET_RGB                   1  // obsolete
#define PLESC_ALLOC_NCOL                2  // obsolete
#define PLESC_SET_LPB                   3  // obsolete
#define PLESC_EXPOSE                    4  // handle window expose
#define PLESC_RESIZE                    5  // handle window resize
#define PLESC_REDRAW                    6  // handle window redraw
#define PLESC_TEXT                      7  // switch to text screen
#define PLESC_GRAPH                     8  // switch to graphics screen
#define PLESC_FILL                      9  // fill polygon
#define PLESC_DI                        10 // handle DI command
#define PLESC_FLUSH                     11 // flush output
#define PLESC_EH                        12 // handle Window events
#define PLESC_GETC                      13 // get cursor position
#define PLESC_SWIN                      14 // set window parameters
#define PLESC_DOUBLEBUFFERING           15 // configure double buffering
#define PLESC_XORMOD                    16 // set xor mode
#define PLESC_SET_COMPRESSION           17 // AFR: set compression
#define PLESC_CLEAR                     18 // RL: clear graphics region
#define PLESC_DASH                      19 // RL: draw dashed line
#define PLESC_HAS_TEXT                  20 // driver draws text
#define PLESC_IMAGE                     21 // handle image
#define PLESC_IMAGEOPS                  22 // plimage related operations
#define PLESC_PL2DEVCOL                 23 // convert PLColor to device color
#define PLESC_DEV2PLCOL                 24 // convert device color to PLColor
#define PLESC_SETBGFG                   25 // set BG, FG colors
#define PLESC_DEVINIT                   26 // alternate device initialization
#define PLESC_GETBACKEND                27 // get used backend of (wxWidgets) driver - no longer used
#define PLESC_BEGIN_TEXT                28 // get ready to draw a line of text
#define PLESC_TEXT_CHAR                 29 // render a character of text
#define PLESC_CONTROL_CHAR              30 // handle a text control character (super/subscript, etc.)
#define PLESC_END_TEXT                  31 // finish a drawing a line of text
#define PLESC_START_RASTERIZE           32 // start rasterized rendering
#define PLESC_END_RASTERIZE             33 // end rasterized rendering
#define PLESC_ARC                       34 // render an arc
#define PLESC_GRADIENT                  35 // render a gradient
#define PLESC_MODESET                   36 // set drawing mode
#define PLESC_MODEGET                   37 // get drawing mode
#define PLESC_FIXASPECT                 38 // set or unset fixing the aspect ratio of the plot
#define PLESC_IMPORT_BUFFER             39 // set the contents of the buffer to a specified byte string
#define PLESC_APPEND_BUFFER             40 // append the given byte string to the buffer
#define PLESC_FLUSH_REMAINING_BUFFER    41 // flush the remaining buffer e.g. after new data was appended
#define PLTEXT_FONTCHANGE               0 // font change in the text stream
#define PLTEXT_SUPERSCRIPT              1 // superscript in the text stream
#define PLTEXT_SUBSCRIPT                2 // subscript in the text stream
#define PLTEXT_BACKCHAR                 3 // back-char in the text stream
#define PLTEXT_OVERLINE                 4 // toggle overline in the text stream
#define PLTEXT_UNDERLINE                5 // toggle underline in the text stream
#define ZEROW2B                         1
#define ZEROW2D                         2
#define ONEW2B                          3
#define ONEW2D                          4
#define PLSWIN_DEVICE    1              // device coordinates
#define PLSWIN_WORLD     2              // world coordinates
#define PL_X_AXIS        1              // The x-axis
#define PL_Y_AXIS        2              // The y-axis
#define PL_Z_AXIS        3              // The z-axis
#define PL_OPT_ENABLED      0x0001      // Obsolete
#define PL_OPT_ARG          0x0002      // Option has an argument
#define PL_OPT_NODELETE     0x0004      // Don't delete after processing
#define PL_OPT_INVISIBLE    0x0008      // Make invisible
#define PL_OPT_DISABLED     0x0010      // Processing is disabled
#define PL_OPT_FUNC      0x0100         // Call handler function
#define PL_OPT_BOOL      0x0200         // Set *var = 1
#define PL_OPT_INT       0x0400         // Set *var = atoi(optarg)
#define PL_OPT_FLOAT     0x0800         // Set *var = atof(optarg)
#define PL_OPT_STRING    0x1000         // Set var = optarg
#define PL_PARSE_PARTIAL              0x0000 // For backward compatibility
#define PL_PARSE_FULL                 0x0001 // Process fully & exit if error
#define PL_PARSE_QUIET                0x0002 // Don't issue messages
#define PL_PARSE_NODELETE             0x0004 // Don't delete options after
#define PL_PARSE_SHOWALL              0x0008 // Show invisible options
#define PL_PARSE_OVERRIDE             0x0010 // Obsolete
#define PL_PARSE_NOPROGRAM            0x0020 // Program name NOT in *argv[0]..
#define PL_PARSE_NODASH               0x0040 // Set if leading dash NOT required
#define PL_PARSE_SKIP                 0x0080 // Skip over unrecognized args
#define PL_FCI_MARK                   0x80000000
#define PL_FCI_IMPOSSIBLE             0x00000000
#define PL_FCI_HEXDIGIT_MASK          0xf
#define PL_FCI_HEXPOWER_MASK          0x7
#define PL_FCI_HEXPOWER_IMPOSSIBLE    0xf
#define PL_FCI_FAMILY                 0x0
#define PL_FCI_STYLE                  0x1
#define PL_FCI_WEIGHT                 0x2
#define PL_FCI_SANS                   0x0
#define PL_FCI_SERIF                  0x1
#define PL_FCI_MONO                   0x2
#define PL_FCI_SCRIPT                 0x3
#define PL_FCI_SYMBOL                 0x4
#define PL_FCI_UPRIGHT                0x0
#define PL_FCI_ITALIC                 0x1
#define PL_FCI_OBLIQUE                0x2
#define PL_FCI_MEDIUM                 0x0
#define PL_FCI_BOLD                   0x1
#define PL_MAXKEY    16
#define PL_MASK_SHIFT      0x1    // ( 1 << 0 )
#define PL_MASK_CAPS       0x2    // ( 1 << 1 )
#define PL_MASK_CONTROL    0x4    // ( 1 << 2 )
#define PL_MASK_ALT        0x8    // ( 1 << 3 )
#define PL_MASK_NUM        0x10   // ( 1 << 4 )
#define PL_MASK_ALTGR      0x20   //  ( 1 << 5 )
#define PL_MASK_WIN        0x40   // ( 1 << 6 )
#define PL_MASK_SCROLL     0x80   // ( 1 << 7 )
#define PL_MASK_BUTTON1    0x100  // ( 1 << 8 )
#define PL_MASK_BUTTON2    0x200  // ( 1 << 9 )
#define PL_MASK_BUTTON3    0x400  // ( 1 << 10 )
#define PL_MASK_BUTTON4    0x800  // ( 1 << 11 )
#define PL_MASK_BUTTON5    0x1000 // ( 1 << 12 )
#define PL_MAXWINDOWS    64     // Max number of windows/page tracked
#define PL_NOTSET    ( -42 )
#define PLESC_DOUBLEBUFFERING_ENABLE     1
#define PLESC_DOUBLEBUFFERING_DISABLE    2
#define PLESC_DOUBLEBUFFERING_QUERY      3
#define PL_BIN_DEFAULT     0x0
#define PL_BIN_CENTRED     0x1
#define PL_BIN_NOEXPAND    0x2
#define PL_BIN_NOEMPTY     0x4
#define GRID_CSA       1 // Bivariate Cubic Spline approximation
#define GRID_DTLI      2 // Delaunay Triangulation Linear Interpolation
#define GRID_NNI       3 // Natural Neighbors Interpolation
#define GRID_NNIDW     4 // Nearest Neighbors Inverse Distance Weighted
#define GRID_NNLI      5 // Nearest Neighbors Linear Interpolation
#define GRID_NNAIDW    6 // Nearest Neighbors Around Inverse Distance Weighted
#define PL_HIST_DEFAULT            0x00
#define PL_HIST_NOSCALING          0x01
#define PL_HIST_IGNORE_OUTLIERS    0x02
#define PL_HIST_NOEXPAND           0x08
#define PL_HIST_NOEMPTY            0x10
#define PL_POSITION_NULL             0x0
#define PL_POSITION_LEFT             0x1
#define PL_POSITION_RIGHT            0x2
#define PL_POSITION_TOP              0x4
#define PL_POSITION_BOTTOM           0x8
#define PL_POSITION_INSIDE           0x10
#define PL_POSITION_OUTSIDE          0x20
#define PL_POSITION_VIEWPORT         0x40
#define PL_POSITION_SUBPAGE          0x80
#define PL_LEGEND_NULL               0x0
#define PL_LEGEND_NONE               0x1
#define PL_LEGEND_COLOR_BOX          0x2
#define PL_LEGEND_LINE               0x4
#define PL_LEGEND_SYMBOL             0x8
#define PL_LEGEND_TEXT_LEFT          0x10
#define PL_LEGEND_BACKGROUND         0x20
#define PL_LEGEND_BOUNDING_BOX       0x40
#define PL_LEGEND_ROW_MAJOR          0x80
#define PL_COLORBAR_NULL             0x0
#define PL_COLORBAR_LABEL_LEFT       0x1
#define PL_COLORBAR_LABEL_RIGHT      0x2
#define PL_COLORBAR_LABEL_TOP        0x4
#define PL_COLORBAR_LABEL_BOTTOM     0x8
#define PL_COLORBAR_IMAGE            0x10
#define PL_COLORBAR_SHADE            0x20
#define PL_COLORBAR_GRADIENT         0x40
#define PL_COLORBAR_CAP_NONE         0x80
#define PL_COLORBAR_CAP_LOW          0x100
#define PL_COLORBAR_CAP_HIGH         0x200
#define PL_COLORBAR_SHADE_LABEL      0x400
#define PL_COLORBAR_ORIENT_RIGHT     0x800
#define PL_COLORBAR_ORIENT_TOP       0x1000
#define PL_COLORBAR_ORIENT_LEFT      0x2000
#define PL_COLORBAR_ORIENT_BOTTOM    0x4000
#define PL_COLORBAR_BACKGROUND       0x8000
#define PL_COLORBAR_BOUNDING_BOX     0x10000
#define PL_DRAWMODE_UNKNOWN          0x0
#define PL_DRAWMODE_DEFAULT          0x1
#define PL_DRAWMODE_REPLACE          0x2
#define PL_DRAWMODE_XOR              0x4
#define DRAW_LINEX     0x001  // draw lines parallel to the X axis
#define DRAW_LINEY     0x002  // draw lines parallel to the Y axis
#define DRAW_LINEXY    0x003  // draw lines parallel to both the X and Y axis
#define MAG_COLOR      0x004  // draw the mesh with a color dependent of the magnitude
#define BASE_CONT      0x008  // draw contour plot at bottom xy plane
#define TOP_CONT       0x010  // draw contour plot at top xy plane
#define SURF_CONT      0x020  // draw contour plot at surface
#define DRAW_SIDES     0x040  // draw sides
#define FACETED        0x080  // draw outline for each square that makes up the surface
#define MESH           0x100  // draw mesh
*/