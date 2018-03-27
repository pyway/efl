evas_image_loaders_file = [
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

foreach loader_inst : evas_image_loaders_file
	loader = loader_inst[0]
	loader_deps = loader_inst[1]


  opt = '-'.join(['image-format', loader])
  if get_option(opt) == 'static'
  	file = join_paths(loader, 'evas_image_load_'+loader+'.c')

    if loader == 'gif'
      loader_deps += cc.find_library('gif')
    elif loader == 'png'
      loader_deps += dependency('libpng')
    elif loader == 'tiff'
      loader_deps += dependency('libtiff-4')
    elif loader == 'webp'
      loader_deps += dependency('libwebp')
    endif


  	tmp = static_library('image_loader_'+loader, file,
  		include_directories : config_dir,
  		dependencies : [evas_pre] + loader_deps
  	)

    evas_static_list += [declare_dependency(
      sources: file,
      dependencies: loader_deps,
    )]

  	config_h.set('EVAS_STATIC_BUILD_'+loader.to_upper(), '1')
    config_h.set('BUILD_LOADER_'+loader.to_upper(), '1')
  else
    message('Image loader '+loader+' disabled')
  endif
endforeach
