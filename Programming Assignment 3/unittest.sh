#! /bin/bash

# Compile
gcc -o filter Harville_cs390program3.c

# Prep windows text file
cp test_files/windows.txt test_files/windows2.txt
sed -i 's/$/\r/' test_files/windows.txt

# Run tests
echo "test_files/unix.txt" | ./filter
echo "test_files/windows.txt" | ./filter -u
echo "test_files/empty.txt" | ./filter
echo "test_files/5mb.txt" | ./filter
echo "idk.bin" | ./filter

mv test_files/windows2.txt test_files/windows.txt