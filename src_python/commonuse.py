import pickle

def saveModule(objName,filename):
    fw = open(filename,'wb')
    pickle.dump(objName,fw)
    fw.close()
    
def readModule(filename):
    fr = open(filename,"rb")
    return pickle.load(fr)