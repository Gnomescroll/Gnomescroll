from django.conf.urls.defaults import *

# Uncomment the next two lines to enable the admin:
# from django.contrib import admin
# admin.autodiscover()

urlpatterns = patterns('',
    # Example:
    # (r'^data_server/', include('data_server.foo.urls')),

    # Uncomment the admin/doc line below and add 'django.contrib.admindocs' 
    # to INSTALLED_APPS to enable admin documentation:
    # (r'^admin/doc/', include('django.contrib.admindocs.urls')),

    # Uncomment the next line to enable the admin:
    # (r'^admin/', include(admin.site.urls)),
    
    
    (r'^$', 'data_server.interface.views.index'),
    #tile
	(r'^tile/list$', 'data_server.tile.views.tile_list'),
	(r'^tile/edit/(?P<tile_id>\d+)$', 'data_server.tile.views.edit_tile'),
	
	(r'^tile/api/get_tile/(?P<tile_id>\d+)$', 'data_server.tile.views.json_get_tile'),
	(r'^tile/api/get_tile_visual/(?P<tile_id>\d+)$', 'data_server.tile.views.API_get_tile_visual'),
	(r'^tile/api/get_tile_id_list$', 'data_server.tile.views.API_get_tile_id_list'),	
	(r'^tile/api/get_tile_list$', 'data_server.tile.views.API_get_tile_list'),
	
	#tileset
	(r'^tileset/list$', 'data_server.tile.views.list_tileset'),
	(r'^tileset/edit/(?P<tileset_id>\d+)$', 'data_server.tile.views.edit_tileset'),
)
