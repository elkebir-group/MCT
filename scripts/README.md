Example execution:

    ./convert-k-cut.py graph2.txt > tmp2.txt

    ./mCTPBF tmp2.txt -k 2 -p hello3

Results are in `hello3_cluster?.dot`.

Fixing clustering:

    ./mCTPBF tmp2.txt -k 2 -p hello3 -c 0,1,1,1,1,0
