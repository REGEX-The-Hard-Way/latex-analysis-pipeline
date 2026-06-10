make scanner
make sent_split
./scanner.out sound1.tex tex$ 2> sidecar.csv | ./sent_split.out > sents.txt

