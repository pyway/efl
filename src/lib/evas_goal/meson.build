evas_lib = library('evas', 
    'empty.c',
    link_with : [evas_pre_lib, evas_static_list],
    dependencies: [evas_pre],
    install: true,
)

evas = declare_dependency(
   dependencies : [eina, ecore]
)

#install_data(pub_eo_files,
#  install_dir: join_paths(eolian_include_dir, package_version_name)
#)

install_headers(evas_header_src,
  install_dir : dir_package_include,
)