engines = ['buffer', 'drm', 'fb', 'software_generic', 'software_x11', 'wayland_egl', 'wayland_shm']
static_buffers = []

evas_static_build = true
evas_static_list = []

evas_include_dirs += vg_common_inc_dir

#fixed dependencies by efl
png = dependency('libpng')
tiff = dependency('libtiff-4')
giflib = cc.find_library('gif')

#we need to make this a optional
webp = dependency('libwebp')

subdir('image_loaders')
subdir('image_savers')
subdir('model_savers')
subdir('model_loaders')
subdir('vg_savers')
subdir('vg_loaders')

foreach engine : engines
	subdir(join_paths('engines', engine))

  mod_full_name = 'evas_engine_'+engine
  mod_install_dir = join_paths(dir_package_modules, 'engines', engine, module_arch)

  engine_include_dir = []
  engine_src = []
  engine_deps = []

  shared_library(mod_full_name, engine_src,
    include_directories : config_dir + engine_include_dir,
    dependencies : [eina, evas_pre] + engine_deps,
    install : true,
    install_dir : mod_install_dir
  )

  module_files += join_paths(mod_install_dir, 'lib'+mod_full_name+'.so')
endforeach