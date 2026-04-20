for i in $(seq 10); do
  len=$((1000 + RANDOM*1000))
  tr -dc 'a-zA-Z0-9' </dev/urandom | head -c "$len"
  echo
done    