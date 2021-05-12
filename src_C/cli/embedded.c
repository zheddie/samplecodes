/*************************************************************************
** file = embedded.c
**
** Example of executing an SQL statement using CLI.
** The equivalent embedded SQL statements are shown in comments.
**
** Functions used:
**
**        SQLAllocConnect      SQLFreeConnect
**        SQLAllocEnv          SQLFreeEnv
**        SQLAllocStmt         SQLFreeStmt
**        SQLConnect           SQLDisconnect
**
**        SQLBindCol           SQLFetch
**        SQLSetParam          SQLTransact
**        SQLError             SQLExecDirect
**
**************************************************************************/
#include <stdio.h>
#include <string.h>
#include "sqlcli.h"
 
#ifndef NULL
#define NULL   0
#endif
 
int print_err (SQLHDBC    hdbc,
               SQLHSTMT   hstmt);
 
int main ()
{
    SQLHENV        henv;
    SQLHDBC        hdbc;
    SQLHSTMT       hstmt;
 
    SQLCHAR     server[] = "N128584T";
    //"sample";
    SQLCHAR     uid[30];
    SQLCHAR     pwd[30];
 
    SQLINTEGER     id;
    SQLCHAR        name[51];
    SQLINTEGER     namelen, intlen;
    SQLSMALLINT    scale;
 
    scale = 0;
 
 
    /* EXEC SQL CONNECT TO :server USER :uid USING :authentication_string; */
    SQLAllocEnv (&henv);                 /* allocate an environment handle */
 
    SQLAllocConnect (henv, &hdbc);       /* allocate a connection handle   */
 
 
    /* Connect to database indicated by "server" variable with             */
    /*    authorization-name given in "uid", authentication-string given   */
    /*    in "pwd". Note server, uid, and pwd contain null-terminated      */
    /*    strings, as indicated by the 3 input lengths set to SQL_NTS      */
    if (SQLConnect (hdbc, server, SQL_NTS, NULL, SQL_NTS, NULL, SQL_NTS)
                   != SQL_SUCCESS)
        return (print_err (hdbc, SQL_NULL_HSTMT));
 
    SQLAllocStmt (hdbc, &hstmt);         /* allocate a statement handle    */
    printf("cp0\n");
 
    /* EXEC SQL CREATE TABLE NAMEID (ID integer, NAME varchar(50));        */
    {
       SQLCHAR create[] = "CREATE TABLE example/NAMEID (ID integer, NAME varchar(50))";
 
    /* execute the sql statement                                           */
       if (SQLExecDirect (hstmt, create, SQL_NTS) != SQL_SUCCESS)
           return (print_err (hdbc, hstmt));
    }
 
  printf("cp1\n");
    /* EXEC SQL COMMIT WORK;                                               */
    SQLTransact (henv, hdbc, SQL_COMMIT);           /* commit create table */
 
 
    /* EXEC SQL INSERT INTO NAMEID VALUES ( :id, :name         */
    {
       SQLCHAR insert[] = "INSERT INTO NAMEID VALUES (?, ?)";
 
 printf("cp2\n");
    /* show the use of SQLPrepare/SQLExecute method                        */
    /* prepare the insert                                                  */
 
       if (SQLPrepare (hstmt, insert, SQL_NTS) != SQL_SUCCESS)
          return (print_err (hdbc, hstmt));
  printf("cp3\n");
    /* Set up the first input parameter "id"                               */
       intlen = sizeof (SQLINTEGER);
       SQLSetParam (hstmt, 1,
                    SQL_C_LONG, SQL_INTEGER,
                   (SQLINTEGER) sizeof (SQLINTEGER),
                    scale, (SQLPOINTER) &id,
                   (SQLINTEGER *) &intlen);
  printf("cp4\n");
       namelen = SQL_NTS;
    /* Set up the second input parameter "name"                            */
       SQLSetParam (hstmt, 2,
                    SQL_C_CHAR, SQL_VARCHAR,
                    50,
                    scale, (SQLPOINTER) name,
                    (SQLINTEGER *) &namelen);
  printf("cp5\n");
    /* now assign parameter values and execute the insert                 */
       id=500;
       strcpy (name, "Babbage");
 
       if (SQLExecute (hstmt) != SQL_SUCCESS)
          return (print_err (hdbc, hstmt));
    }
  printf("cp6\n");
 
    /* EXEC SQL COMMIT WORK;                                               */
    SQLTransact (henv, hdbc, SQL_COMMIT);        /* commit inserts         */
 
 
    /* EXEC SQL DECLARE c1 CURSOR FOR SELECT ID, NAME FROM NAMEID;         */
    /* EXEC SQL OPEN c1;                                                   */
    /* The application doesn't specify "declare c1 cursor for"             */
    {
       SQLCHAR select[] = "select ID, NAME from NAMEID";
       if (SQLExecDirect (hstmt, select, SQL_NTS) != SQL_SUCCESS)
          return (print_err (hdbc, hstmt));
    }
 
  printf("cp7\n");
    /* EXEC SQL FETCH c1 INTO :id, :name;                      */
    /* Binding first column to output variable "id"                        */
    SQLBindCol (hstmt, 1,
                SQL_C_LONG, (SQLPOINTER) &id,
               (SQLINTEGER) sizeof (SQLINTEGER),
               (SQLINTEGER *) &intlen);
 
    /* Binding second column to output variable "name"                     */
    SQLBindCol (hstmt, 2,
                SQL_C_CHAR, (SQLPOINTER) name,
               (SQLINTEGER) sizeof (name),
                &namelen);
  printf("cp8\n");
    SQLFetch (hstmt);                      /* now execute the fetch        */
    printf("Result of Select: id = %ld name = %s\n", id, name);
 
    /* finally, we should commit, discard hstmt, disconnect                */
    /* EXEC SQL COMMIT WORK;                                               */
    SQLTransact (henv, hdbc, SQL_COMMIT);  /* commit the transaction       */
 
    /* EXEC SQL CLOSE c1;                                                  */
    SQLFreeStmt (hstmt, SQL_DROP);         /* free the statement handle    */
 
    /* EXEC SQL DISCONNECT;                                                */
    SQLDisconnect (hdbc);                  /* disconnect from the database */
 
    SQLFreeConnect (hdbc);                 /* free the connection handle   */
    SQLFreeEnv (henv);                     /* free the environment handle  */
  printf("cp9\n");
    return (0);
}
 
int print_err (SQLHDBC    hdbc,
               SQLHSTMT   hstmt)
{
SQLCHAR        buffer[SQL_MAX_MESSAGE_LENGTH + 1];
SQLCHAR        sqlstate[SQL_SQLSTATE_SIZE + 1];
SQLINTEGER     sqlcode;
SQLSMALLINT    length;
 
 
        while ( SQLError(SQL_NULL_HENV, hdbc, hstmt,
                 sqlstate,
                 &sqlcode,
                 buffer,
                 SQL_MAX_MESSAGE_LENGTH + 1,
                 &length) == SQL_SUCCESS )
        {
                printf("SQLSTATE: %s Native Error Code: %ld\n",
                       sqlstate, sqlcode);
                printf("%s \n", buffer);
                printf("----------------------------- \n");
         };
 
        return(SQL_ERROR);
 
}
 