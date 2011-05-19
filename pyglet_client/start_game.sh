echo "Running setup.sh"
./setup.sh
echo "Setup.sh complete"
echo
cd server/
tmpfile_prefix="lastserverpid"
function runserver {
    tmpfile_ext="XXXXXXXX.tmp"
    tmpfile_prefix=$1
    python run.py &> /dev/null &
    rm ${tmpfile_prefix}*
    t=$(mktemp ${tmpfile_prefix}${tmpfile_ext})
    echo "$!" > "$t"
}
echo $1
arg1="-r"
if [ "$1" = "-r" ]
then
    echo "restarting server"
    started="restarted"
    runserver $tmpfile_prefix
else
    cmd="cat ${tmpfile_prefix}* 2>&1"
    p=$($cmd)
    echo $p
    fe=$($cmd | grep "No such file")
    if [ "$fe" != "" ]
    then
        p=""
    fi
    if [ ! $p ]
    then
        e=1
    else
        echo "killing $p"
        kill -0 $p
        e=$?
    fi
    echo $e
    if [ "$e" -eq "0" ]
    then
        started="already running"
    else
        started="started"
        runserver $tmpfile_prefix
    fi
fi
echo "server $started"
cd ../netclient/
python gameloop.py
