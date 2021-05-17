#!/usr/bin/env python3
# -*- coding: utf-8 -*-


import ibm_db_dbi as dbi
import argparse
import sys

if __name__ == '__main__':
        parser = argparse.ArgumentParser()
        parser.add_argument('-s','--sql',help='sql command to run')
        args = parser.parse_args()
        
        if args.sql is None:
                parser.print_help(sys.stderr)
                sys.exit(1)

        sqlcmd = str(args.sql)
        print("sqlcmd="+sqlcmd)
        try:
            conn = dbi.connect()
            cur = conn.cursor()
            cur.execute(sqlcmd)
            if cur._result_set_produced:
                    rlist = cur.fetchall()
                    for onerecord in rlist:
                        print(onerecord)
            conn.commit()
        except Exception as err:
            print('ERROR:  ' + str(err))