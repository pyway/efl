
evas_image_savers_file = [
['eet', [eet]], 
['jpeg', [jpeg]], 
['png', []], 
['tgv', [rg_etc, lz4]], 
['tiff', []], 
['webp', []]
]

foreach saver_inst : evas_image_savers_file
  saver = saver_inst[0]
  saver_deps = saver_inst[1]

  opt = '-'.join(['image-format', saver])
  if get_option(opt) == 'static'
    file = join_paths(saver, 'evas_image_save_'+saver+'.c')

    if loader == 'gif'
      loader_deps += cc.find_library('gif')
    elif loader == 'png'
      loader_deps += dependency('libpng')
    elif loader == 'tiff'
      loader_deps += dependency('libtiff-4')
    elif loader == 'webp'
      loader_deps += dependency('libwebp')
    endif

    tmp = static_library('image_saver_'+saver, file,
      include_directories : config_dir,
      dependencies : [evas_pre] + saver_deps
    )

    evas_static_list += [declare_dependency(
      sources: file,
      dependencies: saver_deps,
    )]

    config_h.set('EVAS_STATIC_BUILD_'+saver.to_upper(), '1')
  else
    message('Image saver '+saver+' disabled')
  endif
endforeach
