evas_vg_loaders_file = ['eet', 'svg']

foreach loader : evas_vg_loaders_file
	file = join_paths(loader, 'evas_vg_load_'+loader+'.c')
	evas_static_list += static_library('vg_loader_'+loader, file,
		include_directories : config_dir + evas_include_dirs,
		dependencies : evas_deps + [vg_common]
	)
endforeach
