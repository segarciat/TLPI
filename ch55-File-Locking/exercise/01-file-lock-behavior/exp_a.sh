interval=1
lock_duration=2 # bigger than interval

# Reseve and acquire shared lock successfully
./t_flock README.md s $lock_duration > /dev/null &

# Reseve exclusive lock and block
./t_flock README.md x $lock_duration &


# Reserve and acquire shared locks while process that requested exclusive lock is blocked
for i in $(seq 1 10)
do
  sleep $interval
  ./t_flock README.md s $lock_duration &
done;
