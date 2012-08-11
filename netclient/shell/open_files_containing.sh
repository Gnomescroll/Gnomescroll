geany -i `grep -r "$1" * | cut -f 1 -d : | grep pp$`

