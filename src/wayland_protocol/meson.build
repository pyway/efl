wayland_scanner_bin = find_program('wayland-scanner')

wayland_protocols = dependency('wayland-protocols')

wl_code_gen = generator(wayland_scanner_bin,
  arguments: ['code','@INPUT@','@OUTPUT@'],
  output: '@BASENAME@-protocol.c', )

wl_client_header_gen = generator(wayland_scanner_bin,
  arguments: ['client-header','@INPUT@','@OUTPUT@'],
  output: '@BASENAME@-client-protocol.h', )

wl_server_header_gen = generator(wayland_scanner_bin,
  arguments: ['server-header','@INPUT@','@OUTPUT@'],
  output: '@BASENAME@-server-protocol.h', )

wl_protocol_local = [
  'efl-aux-hints.xml',
  'efl-hints.xml',
  'session-recovery.xml',
  'teamwork.xml',	
  'www.xml'
]

wl_protocol_sys = [
  join_paths('xdg-shell', 'xdg-shell-unstable-v6.xml'),
  join_paths('linux-dmabuf', 'linux-dmabuf-unstable-v1.xml'),
]

foreach sys_protocol : wl_protocol_sys
	wl_protocol_local += [ 
		join_paths(wayland_protocols.get_pkgconfig_variable('pkgdatadir'), 'unstable',
			sys_protocol)]
endforeach

gen_obj = []
gen_obj += wl_code_gen.process(wl_protocol_local)
gen_obj += wl_client_header_gen.process(wl_protocol_local)
gen_obj += wl_server_header_gen.process(wl_protocol_local)

wayland_protocol_lib = static_library('wayland_protocol',
    gen_obj,
)

wayland_protocol = declare_dependency(
	include_directories: include_directories('.'),
	link_with: wayland_protocol_lib,
	sources: gen_obj,
)