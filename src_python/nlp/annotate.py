from stanza.nlp.corenlp import CoreNLPClient
client = CoreNLPClient(server='http://localhost:9000', default_annotators=['ssplit', 'tokenize', 'lemma', 'pos', 'ner'])
annotated = client.annotate('This is an example document. Here is a second sentence')
for sentence in annotated.sentences:
    print('sentence', sentence)
    for token in sentence:
        print(token.word, token.lemma, token.pos, token.ner)
