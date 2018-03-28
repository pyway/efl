
evas_image_mods_file = [
['eet', [eet]], 
['jpeg', [jpeg]], 
['png', []], 
['tgv', [rg_etc, lz4]], 
['tiff', []], 
['webp', []]
]

foreach mod_inst : evas_image_mods_file
  mod = mod_inst[0]
  mod_deps = mod_inst[1]

  opt = '-'.join(['image-format', mod])
  file = join_paths(mod, 'evas_image_save_'+mod+'.c')
  if get_option(opt) == 'disabled'
    message('Image saver '+mod+' disabled')
  else
    if mod == 'gif'
      mod_deps += cc.find_library('gif')
    elif mod == 'png'
      mod_deps += dependency('libpng')
    elif mod == 'tiff'
      mod_deps += dependency('libtiff-4')
    elif mod == 'webp'
      mod_deps += dependency('libwebp')
    endif

    if get_option(opt) == 'static'
      tmp = static_library('image_saver_'+mod, file,
        include_directories : config_dir,
        dependencies : [evas_pre] + mod_deps
      )

      evas_static_list += [declare_dependency(
        sources: file,
        dependencies: mod_deps,
      )]

    else
      mod_dir = join_paths([dir_package_modules, 'image_savers', mod, module_version])
      name = '_'.join(['image_saver', mod])
      built_mod = shared_module(name, file,
                    include_directories: config_dir,
                    name_prefix        : '',
                    dependencies       : [ mod_deps ],
                    install_dir        : mod_dir,
                    install            : true,
                    link_args          : '-Wl,--unresolved-symbols=ignore-all'
                   )
      module_files += built_mod.full_path()
    endif
  endif
endforeach
