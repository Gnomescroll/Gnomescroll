run="./run"
for ((c=0; c<$1; c++ ))
do
	($run) &
    sleep 2
done
