import yake
from sys import argv
with open(argv[1],'r') as f:
    text = f.read()
# Simple usage with default parameters
kw_extractor = yake.KeywordExtractor()
keywords = kw_extractor.extract_keywords(text)

# With custom parameters
custom_kw_extractor = yake.KeywordExtractor(
    lan="en",              # language
    n=5,                   # ngram size
    dedupLim=0.9,          # deduplication threshold
    dedupFunc='seqm',      # deduplication function
    windowsSize=3,         # context window
    top=50,                # number of keywords to extract
    features=None          # custom features
)

keywords = custom_kw_extractor.extract_keywords(text)
for kw, score in keywords:
    print(f"{kw} ({score})")

