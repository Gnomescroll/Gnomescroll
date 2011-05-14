cd server/
python run.py &> /dev/null &
echo 'server/run.py started'
cd ../netclient/
python gameloop.py
