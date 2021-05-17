from nltk.corpus import wordnet
 
syn = wordnet.synsets("pain")
print(syn[0].definition())
print(syn[0].examples())

syn = wordnet.synsets("Python")
print(syn[0].definition())
print(syn[0].examples())
