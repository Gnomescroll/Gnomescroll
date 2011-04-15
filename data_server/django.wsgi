import os
import sys

os.environ['DJANGO_SETTINGS_MODULE'] = 'data_server.production_settings'

path = '/var/www/data_server'
if path not in sys.path:
    sys.path.append(path)

import django.core.handlers.wsgi
application = django.core.handlers.wsgi.WSGIHandler()
