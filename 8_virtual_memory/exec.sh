echo "Compiling"
g++ page.cpp -o page
echo "Computing"
./page < in > result
echo "Plotting"
python plot.py result Result.jpg
