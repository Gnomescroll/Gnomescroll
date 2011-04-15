import os
import sys

sys.path.insert(0, '/var/www/data_server')
sys.path.insert(0, '/var/www/')
os.environ['DJANGO_SETTINGS_MODULE'] = 'data_server.settings'

os.environ['PYTHON_EGG_CACHE'] = '/tmp/.python-eggs'

import django.core.handlers.wsgi
application = django.core.handlers.wsgi.WSGIHandler()
