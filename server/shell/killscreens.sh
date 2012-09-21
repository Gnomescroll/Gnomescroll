screens=`screen -ls | grep -e "[0-9]\{1,10\}\." | grep "$1" | cut -f 2 | sed -e 's/^[ \t]*//' | cut -d "." -f 1`
for screen in $screens; do
  screen -X -S "${screen}" quit
done;
