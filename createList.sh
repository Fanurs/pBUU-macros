DIR=$1

cd $1

for i in {0..2999};
do
  printf -v SET "%03d" ${i}
  ls ${SET}/*.COR >> list
done
