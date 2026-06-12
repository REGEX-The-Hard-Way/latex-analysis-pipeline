import pandas as pd

with open("sidecar.tok", "r") as f:
    data = f.read()
import re

output = []
for ix in range(len(data.splitlines())):
    resp = re.split(
        "{<filepath:|>,filepath_id:|,token_id:|,parent_id:|,offset:|,length:|,type:|,<tok:",
        data.splitlines()[ix],
    )
    if resp[0] == "":
        resp = resp[1:]
        output.append(resp)
import pandas as pd

df = pd.DataFrame(output)
df.columns = [
    "filepath",
    "filepath_id",
    "token_id",
    "parent_id",
    "offset",
    "length",
    "type",
    "tok",
]
refs = df[df.type == "ref"]
refs.tok = refs.tok.apply(lambda x: re.sub(">}$", "", str(x)))
unique_refs = set(refs["tok"].tolist())
unique_refs = [x.replace("\\ref{", "") for x in unique_refs]
unique_refs = set([re.sub("}$", "", str(x)) for x in unique_refs])
# print(unique_refs)

labels = df[df.type == "label"]
labels.tok = labels.tok.apply(lambda x: re.sub(">}$", "", str(x)))
unique_labels = set(labels["tok"].tolist())
unique_labels = [x.replace("\\label{", "") for x in unique_labels]
unique_labels = set([re.sub("}$", "", str(x)) for x in unique_labels])
# print(unique_labels)

print("unresolved", unique_labels.difference(unique_refs))
print("unresolved", unique_refs.difference(unique_labels))

cites = df[df.type == "cite"]
cites.tok = cites.tok.apply(lambda x: re.sub(">}$", "", str(x)))
unique_cites = set(cites["tok"].tolist())
unique_cites = [x.replace("\\cite{", "") for x in unique_cites]
unique_cites = set([re.sub("}$", "", str(x)) for x in unique_cites])
remove = set()
add2 = set()
for cite in unique_cites:
    if "," in cite:
        for c in cite.split(","):
            add2.add(c)
    remove.add(cite)
for cite in remove:
    unique_cites.remove(cite)

for cite in add2:
    unique_cites.add(cite)
print(unique_cites)

resp = re.findall(r"\\begin{thebibliography}.*?\\end{thebibliography}", data, re.DOTALL)
resp = resp[0]
resp = re.split(r"(\\bibitem{.*?})", resp)
bibitems = [x.replace("\\bibitem{", "") for x in resp if "bibitem" in str(x)]
bibitems = [re.sub("\}$", "", str(x)) for x in bibitems]
unique_bibitems = set(bibitems)

print("unresolved", unique_cites.difference(unique_bibitems))
print("unresolved", unique_bibitems.difference(unique_cites))
