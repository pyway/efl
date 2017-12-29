evas_static_build = true
evas_static_list = []

#fixed dependencies by efl
png = dependency('libpng')
tiff = dependency('libtiff-4')
giflib = cc.find_library('gif')

#we need to make this a optional
webp = dependency('libwebp')

engines = ['buffer']
static_buffers = []

subdir('image_loaders')
subdir('image_savers')
subdir('model_savers')
subdir('model_loaders')
subdir('vg_savers')
subdir('vg_loaders')

foreach engine : engines
	subdir(join_paths('engines', engine))
endforeach