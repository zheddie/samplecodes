// Removes Spaces at the left of the string
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <qwcrssts.h>                   // QWCRSSTS API
//#include <qusec.h>                      // Qus_EC_t
#include <sqlcli.h>
#include <ctype.h>                      // For toupper()
#include <stddef.h>                     // For NULL
//#include <qtocnetsts.cleinc>            // QtocRtvTCPA API
typedef enum {
        SQLCliConnNoConnection  = 0
        ,SQLCliConnEnvCreated   = 1
        , SQLCliConnReady       = 2
        , SQLCliMiscStmtReady   = 3
        , SQLCliFetchStmtReady  = 4
        , SQLCliEnvReady        = 5
    } SQLCliStatus;

void ltrim(char *s)
{
    int l = 0, p = 0, k = 0;
    l = strlen(s);
    if( l == 0 ) return;
    p = 0;
    while(s[p] == ' ' || s[p] == '\t')  p++;
    if( p == 0 ) return;
    while(s[k] != '\0') s[k++] = s[p++];
    return;
}

// Removes Spaces at the right of the string
void rtrim(char *s)
{
    int l = 0, p = 0;
    l = strlen(s);
    if( l == 0 ) return;
    p = l - 1;
    while(s[p] == ' ' || s[p] == '\t')
    {
        s[p--] = '\0';
        if( p < 0 ) break;
    }
    return;
}

    SQLHENV             xHenv;
    SQLHDBC             xHdbc;
    SQLHSTMT            xMiscHstmt;
    SQLHSTMT            xFetchHstmt;
    SQLHSTMT            xUpdateDeleteHstmt;

void initCliConnection()
{

    // This function sets up SQL CLI envoronment.
    static SQLINTEGER     sqltrue     = SQL_TRUE;
    static SQLINTEGER     sqlfalse    = SQL_FALSE;
    static SQLINTEGER     sqlnocommit = SQL_TXN_NO_COMMIT;

    SQLRETURN rc = SQLAllocEnv(&xHenv);
    SQLCliStatus xCLIStatus;
    if (rc == SQL_SUCCESS)
    {

        // Ensure the naming convention for the environment is SQL naming. if not something is wrong and we are under another environment
        long curSysNaming = -1;
        SQLINTEGER stringLength = -1;
        rc = SQLGetEnvAttr(xHenv, SQL_ATTR_SYS_NAMING, &curSysNaming, sizeof(curSysNaming), &stringLength);
        if (rc == SQL_SUCCESS && curSysNaming == SQL_FALSE)
        {

            xCLIStatus = SQLCliConnEnvCreated;

            // Allocate SQL connection
            rc = SQLAllocConnect(xHenv, &xHdbc);

            if (rc == SQL_SUCCESS)
            {
            	
    rc = SQLSetConnectAttr( xHdbc, SQL_ATTR_AUTOCOMMIT,&sqlfalse,0);
    if ( rc != SQL_SUCCESS &&
         rc != SQL_SUCCESS_WITH_INFO)
    {
      printf("SQLSetConnectAttr SQL_ATTR_AUTOCOMMIT\n");
    }
    
    rc = SQLSetConnectAttr( xHdbc, SQL_ATTR_COMMIT,&sqlnocommit,0);
    if ( rc != SQL_SUCCESS &&
         rc != SQL_SUCCESS_WITH_INFO )
    {
      printf("SQLSetConnectAttr SQL_ATTR_COMMIT\n");
    }
    
    rc = SQLSetConnectAttr( xHdbc, SQL_ATTR_DBC_SYS_NAMING,&sqltrue,0);
    if ( rc != SQL_SUCCESS &&
         rc != SQL_SUCCESS_WITH_INFO )
    {
      printf("SQLSetConnectAttr SQL_ATTR_DBC_SYS_NAMING\n");
    }
    
    rc = SQLSetConnectAttr( xHdbc, SQL_ATTR_2ND_LEVEL_TEXT,&sqltrue,0);
    if ( rc != SQL_SUCCESS &&
         rc != SQL_SUCCESS_WITH_INFO )
    {
      printf("SQLSetConnectAttr SQL_ATTR_2ND_LEVEL_TEXT\n");
    }

                // Connect to the local IBM i database
                rc = SQLConnect(xHdbc, NULL, 0, NULL, 0, NULL, 0);
								//rc = SQLConnect(xHdbc, NULL, SQL_NTS, NULL, SQL_NTS, NULL, SQL_NTS);
                if (rc == SQL_SUCCESS)
                {

                    // Set the isolation option. We will use commit to manage backout from error so the code must commit transactions after success.
                    SQLINTEGER iso_option = SQL_TXN_READ_COMMITTED/*SQL_TXN_NO_COMMIT*/;
                    rc = SQLSetConnectAttr(xHdbc, SQL_TXN_ISOLATION, &iso_option, sizeof(iso_option));

                    if (rc == SQL_SUCCESS)
                    {
                        xCLIStatus = SQLCliConnReady;

                        // Allocate 3 SQL statements. xMiscHstat will be used for DDL and insert statements, xFetchHstmt will be used to read data from the file and can have a cursor associated with it across calls, and xUpdateDeleteHstmt is used for delete and update operations.

                        rc = SQLAllocStmt (xHdbc, &xMiscHstmt);

                        if (rc == SQL_SUCCESS)
                        {
                            xCLIStatus = SQLCliMiscStmtReady;

                            rc = SQLAllocStmt (xHdbc, &xFetchHstmt);

                            if (rc == SQL_SUCCESS)
                            {

                                SQLINTEGER fetchOnlyAttr = SQL_FALSE;
                                rc = SQLSetStmtAttr(xFetchHstmt, SQL_ATTR_FOR_FETCH_ONLY, &fetchOnlyAttr, 0);

                                if (rc == SQL_SUCCESS)
                                {
                                    xCLIStatus = SQLCliFetchStmtReady;

                                    rc = SQLAllocStmt (xHdbc, &xUpdateDeleteHstmt);

                                    if (rc == SQL_SUCCESS)
                                    {
                                        xCLIStatus = SQLCliEnvReady;
                                    }
                                    else
                                    {
                                        printf( "initCliConnection() error: SQLAllocStmt xUpdateDeleteHstmt rc = %d\n", rc );
                                    }
                                }
                                else
                                {
                                    printf( "initCliConnection() error: SQLSetStmtAttr xFetchHstmt rc = %d\n", rc );
                                }
                            }
                            else
                            {
                                printf(  "initCliConnection() error: SQLAllocStmt xFetchHstmt rc = %d\n", rc );
                            }
                        }
                        else
                        {
                            printf(  "initCliConnection() error: SQLAllocStmt xMiscHstmt rc = %d\n", rc );
                        }
                    }
                    else
                    {
                        printf(  "initCliConnection() error: SQLSetConnectAttr(xHdbc, SQL_TXN_ISOLATION rc = %d\n", rc );
                    }
                }
                else
                {
                    printf(  "initCliConnection() error: SQLConnect rc = %d\n", rc );
                }
            }
            else
            {
                printf(  "initCliConnection() error: SQLAllocConnect rc = %d\n", rc );
            }
        }
        else
        {
            printf(  "initCliConnection() error: naming convention is not SQL, rc = %d\n", rc );
        }
    }
    else
    {
       printf(  "initCliConnection() error: SQLAllocEnv rc = %d\n", rc );
    }
}

void dumpSQLErrorToLogger(SQLHSTMT *stmt, char *msgString)
{

    SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
    SQLCHAR sqlstate[SQL_SQLSTATE_SIZE + 1];
    SQLINTEGER sqlcode;
    SQLSMALLINT length;

    SQLRETURN sqlRC = SQLError(xHenv, xHenv, *stmt,
                               sqlstate,
                               &sqlcode,
                               buffer,
                               SQL_MAX_MESSAGE_LENGTH + 1,
                               &length);

    if (sqlRC == SQL_SUCCESS)
    {
        printf("%s sqlcode = %d, sqlstate = %s, message = %s.\n", msgString, sqlcode, sqlstate, buffer );
    }
    else
    {
        printf("%s --- SQL ERROR INFORMATION COULD NOT BE RETRIEVED.\n", msgString);
    }
}


SQLRETURN prepareAndExecuteSqlStmt(char * sqlStatement, SQLHSTMT * hstmt)
{

    printf( "prepareAndExecuteSqlStmt - starting, statement follows: %s\n", sqlStatement);

    SQLRETURN rc = SQLPrepare(*hstmt, sqlStatement, strlen(sqlStatement));
    if (rc == SQL_SUCCESS)
    {
        printf( "prepareAndExecuteSqlStmt - SQLPrepare rc = %d\n", rc );

         SQLSMALLINT nCols = 0;
         rc = SQLNumResultCols (*hstmt,&nCols);
        
        if ( rc != SQL_SUCCESS )
        { 
        	printf( "prepareAndExecuteSqlStmt - SQLNumResultCols failed rc = %d\n", rc );
        }
        else
        	printf( "prepareAndExecuteSqlStmt - SQLNumResultCols successful rc = %d\n", rc );
        	
        char szColName[256];
    SQLSMALLINT cbColName = 0;
    SQLSMALLINT  SqlType = 0;
    SQLINTEGER   ColDef = 0;
    SQLSMALLINT  Scale = 0;
    SQLSMALLINT  Nullable = 0;
    for(SQLSMALLINT nColIndex = 1;nColIndex<=nCols;nColIndex++)
    {
        {
            rc = SQLDescribeCol (*hstmt,
                            nColIndex,
                            szColName,
                            256,
                            &cbColName,
                            &SqlType,
                            &ColDef,
                            &Scale,
                            &Nullable);
        }
        if ( rc != SQL_SUCCESS )
        { 
        	printf( "prepareAndExecuteSqlStmt - SQLDescribeCol failed rc = %d\n", rc );
        }
        else
        	printf( "prepareAndExecuteSqlStmt - SQLDescribeCol successful rc = %d\n", rc );
        	
          printf("Col: %d, Name: %s, SqlType: %d, ColDef: %d, Scale: %d, Nullable: %d\n",nColIndex,szColName,SqlType,ColDef,Scale,Nullable);
       } 	
       
        rc = SQLExecute(*hstmt);
        if (rc == SQL_SUCCESS ||
            rc == SQL_SUCCESS_WITH_INFO)
        {
            printf( "prepareAndExecuteSqlStmt - SQLExecute rc = %d\n", rc );
            if (rc == SQL_SUCCESS_WITH_INFO)
            {
                dumpSQLErrorToLogger(hstmt, "prepareAndExecuteSqlStmt: SQLExecute INFO - ");
            }
        }
        else
        {
            dumpSQLErrorToLogger(hstmt, " prepareAndExecuteSqlStmt: SQLExecute - ");
        }
    }
    else
    {
        dumpSQLErrorToLogger(hstmt, " prepareAndExecuteSqlStmt: SQLPrepare - ");
    }

    if (rc == SQL_SUCCESS)
    {
       printf( "prepareAndExecuteSqlStmt - successful\n");
    }
    else
    {
       printf( "prepareAndExecuteSqlStmt - failed, rc = %d\n", rc);
    }

    return rc;
}

void MrdbUpperCaseAndPadWithZeros(char *pBuffer, const int pBufSize)
{
  int pad;
  for(pad=pBufSize; (pad > 0) && (pBuffer[pad-1] == 0x40); pad--)
  {
    // Pad trailing blanks with zeros
    pBuffer[pad-1] = 0;
  }
  for(int pos=pad; pos >= 0; pos--)     // Uppercase leading chars
  {
    pBuffer[pos-1] = toupper(pBuffer[pos-1]);
  }
}

/*void getHostName()

{
        // Set primary hostname to current hostname
    char primaryHostName[256] = "\0";
    char xHostName[64] = "\0";
    char xDomainName[255] = "\0";
    struct {
        Qtoc_RtvTCPA_TCPA1100_t format1100;
        Qtoc_RtvTCPA_TCPA1400_t format1400;
    } receiverVar;

    Qus_EC_t errorCode;
    unsigned long receiverVarLen = sizeof(Qtoc_RtvTCPA_TCPA1100_t) + sizeof(Qtoc_RtvTCPA_TCPA1400);
    memset(&receiverVar, 0x00, receiverVarLen);
    memset(&errorCode, 0x00, sizeof(Qus_EC_t));
    errorCode.Bytes_Provided = sizeof(Qus_EC_t);

    // Get TCP stack status & domain attributes
    QtocRtvTCPA((char*)&receiverVar     // Receiver variable
                , &receiverVarLen       // Receiver variable length
                , QTOC_TCPA1400         // Retrieve status & domain attrs
                , (char *)&errorCode);  // Return error code

    if (errorCode.Bytes_Available == 0)
    {
        // Fix up names so they are upper case & null terminated
        
        MrdbUpperCaseAndPadWithZeros((char*) &(receiverVar.format1400.Host_Name), 64);
        MrdbUpperCaseAndPadWithZeros((char*) &(receiverVar.format1400.Domain_Name), 255);
        int hostNameLen = strlen(receiverVar.format1400.Host_Name);
        int domainNameLen = strlen(receiverVar.format1400.Domain_Name);
        printf("Host_Name -%s,%d-\n", receiverVar.format1400.Host_Name,hostNameLen);
        printf("Domain_Name -%s,%d-\n", receiverVar.format1400.Domain_Name,domainNameLen);

        // Make sure host name & domain name will fit into variable in space
        if (hostNameLen + domainNameLen < 256)
        {
            memcpy(primaryHostName, &(receiverVar.format1400.Host_Name), hostNameLen);
            primaryHostName[hostNameLen] = '.';
            memcpy(&primaryHostName[hostNameLen+1], &(receiverVar.format1400.Domain_Name), domainNameLen);
            printf("hostname - %s", primaryHostName);
        }
        else
        {
            printf("MrdbSharedEngineData::createSpace - error: hostname length larger than 256.");
        }
    }
    else
    {
       printf("MrdbSharedEngineData::createSpace - error %7.7s calling QtocRtvTCPA", errorCode.Exception_Id);
    }

}*/

int main()
{
    /*Qwc_SSTS0100_t sysSts;
    Qus_EC_t       sysFbc;
    char * sqlString = "CREATE TABLE QUSRSYS.QMRDBJSON (\
        JSON_FILE VARCHAR(34) NOT NULL,\
        JSON_STR CLOB FOR SBCS DATA NOT NULL,\
        MOD_TIME timestamp)";
    memset( &sysSts, 0x00, sizeof( sysSts ) );
    sysFbc.Bytes_Provided = sizeof( sysFbc );   // Prevent exceptions

    QWCRSSTS((void *)&sysSts            // Receiver variable
             , sizeof(sysSts)           // Length of receiver variable
             , "SSTS0100"               // Format name
             , "*NO       "             // Reset status statistics
             , (char *)&sysFbc);        // Error code

    if ( sysFbc.Bytes_Available == 0 )
    {
        // Fix up name so it is stored as upper case & padded with blanks
       printf("11111 %.08s", sysSts.System_Name);
    }
    else
    {
           printf("2222 %.08s, %.07s", sysSts.System_Name);
    }
    */
    //char * sqlString = "select MIN(DTETIM) DTETIM,MAX(INTSEC) INTSEC, MIN(ETLLND)  LNDESP, SUM(ETMBTR)  LNBTRN, SUM(ETMBRV)  LNBRCV, SUM(ETMIFM + ETMCRE + ETMOWC + ETMALE + ETMCRL + ETMRBE + ETMSPI + ETMROV + ETMMEE + ETMIOV + ETMTUN + ETMBBE + ETMSQE + ETMFNT + ETMDIF - ETMUPF) LNPDUE, SUM(ETLLSP)  LNLSP,MAX(ETMDUP) LNDUP from QAPMETH where INTNUM = ? group by ETLLND order by LNDESP for fetch only optimize for all rows";
    SQLRETURN rc;
    char * sqlString1 = "set schema qpfrdata";
    //char * sqlString2 = "select * from QAPMETH";
    char * sqlString2 = "select MIN(DTETIM) DTETIM, MAX(INTSEC) INTSEC from QAPMETH";
    
    initCliConnection();

    rc = prepareAndExecuteSqlStmt(sqlString1, &xMiscHstmt);
    //rc = SQLFreeStmt();
    rc = prepareAndExecuteSqlStmt(sqlString2, &xMiscHstmt);
    
    rc = SQLDisconnect (xHdbc);
    if (rc != SQL_SUCCESS )
        printf("SQLDisconnect failed\n");
    else
    	  printf("SQLDisconnect successful\n");
    rc = SQLFreeConnect(xHdbc);    /* free first connection handle  */
    if (rc != SQL_SUCCESS )
        printf("SQLFreeConnect failed\n");
    else
    	  printf("SQLFreeConnect successful\n");
    rc = SQLFreeEnv(xHenv);           /* free environment handle       */
    if (rc != SQL_SUCCESS )
        printf("SQLFreeEnv failed\n");
    else
    	  printf("SQLFreeEnv successful\n");
       /* typedef _Packed struct
        { 
                unsigned long CLlen;
                char CLdata[50]; 
        } CLstruct_t;
        CLstruct_t CLstruct;
        sprintf(CLstruct.CLdata,"xxx");
        // getHostName();*/
     return 0;
}
