
#!/bin/bash

start=$(date +%s.%N)

# files=()
# for i in {0..99}
# do
#     files+=("disk.in$i")
# done

# random_num=$((RANDOM % 110 + 1))

# # Shuffle the files array
# shuffled=($(shuf -e "${files[@]}"))

# # Select the first $random_num files
# selected=("${shuffled[@]:0:$random_num}")
./scheduler 43 disk.in33 disk.in9 disk.in37 disk.in23 disk.in32 disk.in14 disk.in4 disk.in25 disk.in26 disk.in0 disk.in35 disk.in31 disk.in6 disk.in27 disk.in3 disk.in21 disk.in19 disk.in16 disk.in24 disk.in8 disk.in28 disk.in39 disk.in7 disk.in29 disk.in34 disk.in15 disk.in2 disk.in22 disk.in11 disk.in1 disk.in30 disk.in13 disk.in18 disk.in38 disk.in20 disk.in10 disk.in12 disk.in5 disk.in36 disk.in17
# ./scheduler $random_num "${selected[@]}" > output.txt
#run the scheduler command with the selected files
# echo "called command: ./scheduler" $random_num "${selected[@]}"
# echo ${#selected[@]}

end=$(date +%s.%N)
runtime=$(echo "$end - $start" | bc)
echo "Total runtime: $runtime seconds"

# ./scheduler 1 "${selected[@]}" > output.txt
#run the scheduler command with the selected files
# echo "called command: ./scheduler 1 " "${selected[@]}"