vg_common_src = [
  'vg_common.c',
  'vg_common.h',
]

evas_include_dirs = [
  include_directories(join_paths('..','..','lib','evas')), 
  include_directories(join_paths('..','..','lib','evas','include')), 
  include_directories(join_paths('..','..','lib','evas','common3d','save_load')),
]

freetype = dependency('freetype2')

vg_common_lib = static_library('vg_common',
    vg_common_src,
    dependencies : [eina, eo, ector, emile, freetype, eet, evas_pre],
    include_directories : config_dir + evas_include_dirs,
    install: false,
)

vg_common = declare_dependency(
  include_directories: [include_directories('.')],
  link_with: vg_common_lib,
  dependencies: [eina],
)
