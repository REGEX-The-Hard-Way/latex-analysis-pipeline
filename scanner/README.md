make scanner
make sent_split
./scanner.out sound1.tex tex$ 2> sidecar.tok | ./sent_split.out > clean.sent

