for i in {0..2999}
do
  printf -v SET "%03d" $i
  cat ${SET}/run-in-progress.stdout | grep -e "tim E/" -e "PI  " -e "DL/N*" | awk '{if ($1 == "tim") printf $3" "; else if ($1 == "DL/N*") printf $2" "$3" "$4" "$5" "; else print $2" "$3" "$4}' > ${SET}/piDeltaYield
done
