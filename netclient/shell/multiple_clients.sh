run="./client"
for ((c=0; c<$1; c++ ))
do
	($run) &
done
