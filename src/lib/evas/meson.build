evas_deps = [eo, eet, eina, efl, emile, ector, ecore]
pub_eo_file_target = []

pub_legacy_eo_files = [

]

foreach eo_file : pub_legacy_eo_files
  pub_eo_file_target += custom_target('eolian_gen_' + eo_file,
    input : eo_file,
    output : [eo_file + '.h', eo_file + '.legacy.h',],
    install : true,
    install_dir : dir_package_include,
    command : [eolian_gen, '-I', meson.current_source_dir(), eolian_include_directories,
                           '-o', 'h:' + join_paths(meson.current_build_dir(), eo_file + '.h'),
                           '-o', 'c:' + join_paths(meson.current_build_dir(), eo_file + '.c'),
                           '-o', 'l:' + join_paths(meson.current_build_dir(), eo_file + '.legacy.h'),
                           '-gchl', '@INPUT@'])

endforeach

pub_eo_files = [
  
]

foreach eo_file : pub_eo_files
  pub_eo_file_target += custom_target('eolian_gen_' + eo_file,
    input : eo_file,
    output : [eo_file + '.h'],
    install : true,
    install_dir : dir_package_include,
    command : [eolian_gen, '-I', meson.current_source_dir(), eolian_include_directories,
                           '-o', 'h:' + join_paths(meson.current_build_dir(), eo_file + '.h'),
                           '-o', 'c:' + join_paths(meson.current_build_dir(), eo_file + '.c'),
                           '-gch', '@INPUT@'])
endforeach


pub_eo_types_files = [

]

foreach eo_file : pub_eo_types_files
  pub_eo_file_target += custom_target('eolian_gen_' + eo_file,
    input : eo_file,
    output : [eo_file + '.h'],
    install : true,
    install_dir : dir_package_include,
    command : [eolian_gen, '-I', meson.current_source_dir(), eolian_include_directories,
                           '-o', 'h:' + join_paths(meson.current_build_dir(), eo_file + '.h'),
                           '-gh', '@INPUT@'])
endforeach

eolian_include_directories += ['-I', meson.current_source_dir()]

evas_header_src = [

]

evas_include_directories = [
  include_directories('.'),
  vg_common_inc_dir
]

evas_src = [
   'main.c'
]

evas_src += vg_common_src

evas_deps += dependency('freetype2')

subdir('include')
subdir('common')
subdir('canvas')
subdir('gesture')
subdir('filters')
subdir('cache')
subdir('common3d')
subdir('file')
subdir('vg')

evas_lib = library('evas',
    evas_src, pub_eo_file_target,
    dependencies: [evas_deps, m, draw, valgrind, libunibreak],
    include_directories : config_dir + evas_include_directories,
    link_with : evas_static_list,
    install: true,
)

evas = declare_dependency(
  include_directories: [include_directories('.')],
  link_with: evas_lib,
  sources : pub_eo_file_target,
  dependencies: [eina],
)

#install_data(pub_eo_files,
#  install_dir: join_paths(eolian_include_dir, package_version_name)
#)

install_headers(evas_header_src,
  install_dir : dir_package_include,
)