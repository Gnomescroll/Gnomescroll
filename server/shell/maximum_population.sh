n=$1
if [ -z "$1" ]
then
	n=1
fi
cat population.log | cut -d . -f 2 | cut -d " " -f 2 | sort -n | tail -n $n

