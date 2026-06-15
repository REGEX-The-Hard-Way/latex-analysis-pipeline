from collections import defaultdict
from nltk.tokenize import mwe
from sys import argv
from tqdm import tqdm
import pandas as pd
import re
import numpy as np
import pandas as pd
import sqlite3

conn = sqlite3.connect('/mnt/x/home/user/my_data.db')
tokenizer = mwe.MWETokenizer(separator="")


def read_file(f_name):
    with open("{}".format(f_name), "r", encoding="ISO-8859-1") as f:
        data = f.read()
    return data


#with open("vocab", "r") as f:
#    vocab = [x[:-1] for x in f.readlines()]

data = read_file(argv[1])
#tokens = set()
#for token in vocab:
#    tokens.add(r"{}".format(token))

# print(tokens)
resp = re.findall(
    "(^|\n){<filepath:(.*?)>,filepath_id:(\d+),token_id:(\d+),parent_id:(\d+),offset:(\d+),length:(\d+),type:([a-z_]+),<tok:(.*?)>}",
    data,
    re.DOTALL,
)
output = []

for ix in range(len(resp)):
    output.append([x for x in resp[ix] if x.strip()])

from tqdm import tqdm
for ix in tqdm(range(0,len(output),1000)):
    df = pd.DataFrame(output[ix:ix+1000])
    df.columns = [
        "filepath",
        "filepath_id",
        "token_id",
        "parent_id",
        "offset",
        "length",
        "type",
        "token",
    ]
    authors = df[df['type'].apply(lambda x : True if re.findall('author|footnote|email|thanks',str(x)) else False)]
    authors.to_sql(name='authors', con=conn, if_exists='append', index=False)
    print(len(authors))

exit()

# 2. Establish a connection to the SQLite database
# (It will automatically create the file if it does not exist)

# 3. Export the DataFrame to an SQL table

# 4. Always close the connection when finished
conn.close()

df = pd.DataFrame(output,)


print(df.head())
authors = df[df['type'].apply(lambda x : True if re.findall('author|equation|inline|cite|ref|affiliation|bibitem',str(x)) else False)]
authors = authors.loc[:,['filepath','token_id','token']]

print(authors)


file_name = list(set(authors.filepath.tolist()))[0]
with open(file_name,'r') as f:
    data = f.read()
    x = authors.itertuples()
    while True:
        try:
            row = next(x)
            data = data.replace(row.token,' ' + row.token_id+ ' ')
        except StopIteration:
            break
f = open(file_name+'.dev','w')
f.write(data)
f.close()




exit()



zf = df.loc[:, ["type", "token"]]





equations = zf[zf.type == "equation"]
equations = set(equations.token.tolist())
f = open("equation_output", "a+")
for eq in equations:
    f.write(eq)
    f.write("\n")
f.close()


types = [x for x in set(zf.type.tolist()) if x != "equation"]
for eq in tqdm(np.array(list(equations))):
    for typ in types:
        lst = set(zf[zf.type == typ].token.tolist())
        for xs in lst:
            if typ == "inline":
                xs = xs[1:-1]
            if xs in eq:
                tokens.add(xs)
tokens = list(tokens)
tokens = sorted(tokens, key=lambda x: -len(x))
print(tokens)
for token in tokens:
    tokenizer.add_mwe(r"{}".format(token))

zf = zf[zf.type == "equation"]
zf.drop_duplicates(inplace=True)
zf = zf.loc[:, "token"].apply(lambda x: re.sub(r"\n", r"\\\\", x))
zf.to_csv("output.csv", sep="\t", index=False)

for eq in equations:
    print(eq)
    print(tokenizer.tokenize(eq))
    inp = input()


"""
df = df.loc[:, "token"].apply(lambda x: re.sub(r"\n", r"\\\\", x))
df = df.tolist()
for ix,xs in enumerate(df):
    f= open(f'{ix}.tex','w')
    f.write('''\\documentclass[]{article}\n\\usepackage{amsmath}\n\\begin{document}\n''')
    f.write(xs)
    f.write('\n')
    f.write('\\end{document}')
    f.close()

# df.to_csv("output.csv", index=False)
"""
