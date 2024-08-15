START=0
STOP=40000
STEP=5000
FILE_TO_LOCK=test_lock_file

awk 'BEGIN {printf "%8s  %s\n", "N", "SYS_TIME"}'
echo "--------------------------------"
for n in $(seq $START $STEP $STOP)
do
  sys_time=$({ time ./acquire_write_lock $FILE_TO_LOCK $n; } |& grep "sys" | awk '{print $2}')
  awk -v byte_pos="$n" -v system_time="$sys_time" 'BEGIN {printf "%8d  %s\n", byte_pos, system_time }'
done
