evas_image_mods_file = [
  ['bmp', []],
  ['eet', [eet]],
  ['generic', [rt]],
  ['gif', []],
  ['ico', []],
  ['jpeg', [jpeg]],
  ['pmaps', []],
  ['png', []],
  ['psd', []],
  ['tga', []],
  ['tgv', [rg_etc, lz4]],
  ['tiff', []],
  ['wbmp', []],
  ['webp', []],
  ['xpm', []],
]

foreach mod_inst : evas_image_mods_file
  mod = mod_inst[0]
  mod_deps = mod_inst[1]


  opt = '-'.join(['image-format', mod])
  file = join_paths(mod, 'evas_image_load_'+mod+'.c')
  if get_option(opt) == 'disabled'
    message('Image loader '+mod+' disabled')
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
      tmp = static_library('image_loader_'+mod, file,
        include_directories : config_dir,
        dependencies : [evas_pre] + mod_deps
      )

      evas_static_list += [declare_dependency(
        sources: file,
        dependencies: mod_deps,
      )]
      config_h.set('EVAS_STATIC_BUILD_'+mod.to_upper(), '1')
    else
      mod_dir = join_paths([dir_package_modules, 'image_loaders', mod, module_version])
      name = '_'.join(['image_loader', mod])
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
    config_h.set('BUILD_LOADER_'+mod.to_upper(), '1')
  endif
endforeach
