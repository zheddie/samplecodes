import pandas as pd
#import csv 
import pymysql as pm
from sqlalchemy import create_engine
def doinsert(file,conn):
    df = pd.read_csv(file,header=None)
    pd.io.sql.to_sql(df, 'xinfadi', con=conn, index=False,if_exists="append")
    #df.to_sql('xinfadi', con=conn, index=False,if_exists="append")

if __name__ == '__main__':
    filelist = [
        'xinfadi0.csv',
        'xinfadi5k.csv',
        'xinfadi10k.csv',
        'xinfadi13k.csv',
        'xinfadi.extra.csv'
    ]
    # filelist = [
    #     'xinfadi.extra.csv'
    # ]
    path = '../qpz1.data/'
#    conn = pm.connect(host="mysqlserver2",user="root",password="passw0rd",database="ai",charset='utf8')
    #print (conn)
    conn = create_engine('mysql+pymysql://root:passw0rd@mysqlserver2:3306/ai')
    for file in filelist:
        filename = path+file
        doinsert(filename,conn)