echo "THIS SCRIPT DOES NOT WORK"
exit

for fn in "$@"
do
  echo "Converting ${fn} to mono"
  sox "${fn}" -c 1 "${fn}" avg
done
