## uses flask script to do routine maintaince from the command line

# manage.py

from flaskext.script import Manager

from myapp import app

manager = Manager(app)

@manager.command
def hello():
    print "hello"

if __name__ == "__main__":
    manager.run()
