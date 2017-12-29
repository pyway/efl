
evas_image_savers_file = [
['eet', [eet]], 
['jpeg', [jpeg]], 
['png', [png]], 
['tgv', [rg_etc, lz4]], 
['tiff', [tiff]], 
['webp', []]
]

foreach loader_inst : evas_image_savers_file
	loader = loader_inst[0]
	loader_deps = loader_inst[1]

	file = join_paths(loader, 'evas_image_save_'+loader+'.c')

	evas_static_list += static_library('image_saver_'+loader, file,
		include_directories : config_dir + [include_directories(join_paths('..','..','..','lib','evas','include')), include_directories(join_paths('..','..','..','lib','evas'))],
		dependencies : [evas_pre] + loader_deps
	)

	config_h.set('EVAS_STATIC_BUILD_'+loader.to_upper(), '1')
endforeach
