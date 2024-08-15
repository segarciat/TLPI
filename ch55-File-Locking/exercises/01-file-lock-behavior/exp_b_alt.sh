initial_exclusive_duration=5
duration=1

./t_flock README.md x $initial_exclusive_duration &

for i in $(seq 1 10)
do
  ./t_flock README.md x $duration &
  ./t_flock README.md s $duration &
done

