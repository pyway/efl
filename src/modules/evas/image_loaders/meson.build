
evas_image_loaders_file = [
  ['bmp', []],
  ['eet', [eet]],
  ['generic', [rt]],
  ['gif', [giflib]],
  ['ico', []],
  ['jpeg', [jpeg]],
  ['pmaps', []],
  ['png', [png]],
  ['psd', []],
  ['tga', []],
  ['tgv', [rg_etc, lz4]],
  ['tiff', [tiff]],
  ['wbmp', []],
  ['webp', [webp]],
  ['xpm', []],
]

foreach loader_inst : evas_image_loaders_file
	loader = loader_inst[0]
	loader_deps = loader_inst[1]

	file = join_paths(loader, 'evas_image_load_'+loader+'.c')

	evas_static_list += static_library('image_loader_'+loader, file,
		include_directories : config_dir + [include_directories(join_paths('..','..','..','lib','evas','include')), include_directories(join_paths('..','..','..','lib','evas'))],
		dependencies : [evas_pre] + loader_deps
	)

	config_h.set('EVAS_STATIC_BUILD_'+loader.to_upper(), '1')
endforeach
