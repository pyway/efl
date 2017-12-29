engine_src = files([
  'evas_engine.c',
  'evas_engine.h',
  'evas_x_egl.c',
  'evas_x_egl.h',
  'Evas_Engine_Software_X11.h',
  'evas_xlib_swapbuf.c',
  'evas_xlib_outbuf.c',
  'evas_xlib_buffer.c',
  'evas_xlib_color.c',
  'evas_xlib_main.c',
  'evas_xlib_swapper.c',
  'evas_xlib_image.c',
  'evas_xlib_dri_image.c',
  'evas_xlib_outbuf.h',
  'evas_xlib_swapbuf.h',
  'evas_xlib_buffer.h',
  'evas_xlib_color.h',
  'evas_xlib_swapper.h',
  'evas_xlib_image.h',
  'evas_xlib_dri_image.h'
])

#xcb is DEAD ... we only use xlib
config_h.set('BUILD_ENGINE_SOFTWARE_XLIB', '1')

engine_deps = []
