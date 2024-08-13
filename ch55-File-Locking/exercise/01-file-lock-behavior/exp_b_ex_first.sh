initial_exclusive_duration=5
duration=1

./t_flock README.md x $initial_exclusive_duration &


# Schedule all of the exclusive requests first
for i in $(seq 1 5)
do
  ./t_flock README.md x $duration &
done

# Give previous process some time to put in their requests
sleep 1

# Schedule all of the shared requests last
for i in $(seq 1 5)
do
  ./t_flock README.md s $duration &
done

