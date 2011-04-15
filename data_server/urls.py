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
    
    
    (r'^/$', 'data_server.interface.views.index'),
    #tile
	(r'^tile/list$', 'data_server.tile.list_tile'),
	(r'^tile/edit/(?P<tile_id>\d+)$', 'data_server.tile.edit_tile'),
	
	#tileset
	(r'^tileset/list$', 'data_server.tile.list_tileset'),
	(r'^tileset/edit/(?P<tileset_id>\d+)$', 'data_server.tile.edit_tileset'),
)
