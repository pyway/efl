#this is the list of dependencies and include directories we need for using evas before evas can be defined
freetype = dependency('freetype2')

evas_include_dirs = [
  include_directories(join_paths('..','evas')), 
  include_directories(join_paths('..','evas','include')), 
  include_directories(join_paths('..','evas','common3d','save_load')),
]

evas_deps = [eet, emile, ector, freetype]

evas_pre = declare_dependency(
	include_directories: evas_include_dirs,
	dependencies: evas_deps,
)


eolian_include_directories += ['-I', join_paths(meson.source_root(), 'src', 'lib', 'evas', 'canvas')]
eolian_include_directories += ['-I', join_paths(meson.source_root(), 'src', 'lib', 'evas', 'include')]
