cat population.log | cut -d . -f 2 | cut -d " " -f 2 | sort -n | tail -n 1

