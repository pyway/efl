evas_vg_savers_file = ['eet', 'svg']

foreach loader : evas_vg_savers_file
	file = join_paths(loader, 'evas_vg_save_'+loader+'.c')
	evas_static_list += static_library('vg_saver_'+loader, file,
		include_directories : config_dir + evas_include_dirs,
		dependencies : [eo, ector, emile, freetype, vg_common, eet]
	)
endforeach