#include "iBlob.h"
#include <iostream>
#include "engine/TSS.h"
#include "iBlobOracleStore.h"
using namespace std;

static char *username = (char *) "aist";
static char *password = (char *) "aist11g";

static OCIEnv *envhp;
static OCIError *errhp;
static sword status;


static	OCISession *authp = (OCISession *) 0;
static	OCIServer *srvhp;
static	OCISvcCtx *svchp;
static	OCIStmt	*stmthp;
static	OCIDefine *defnp = (OCIDefine *) 0;
static	OCIDefine *defnp2 = (OCIDefine *) 0;
static	OCILobLocator *mylob;


static uint myid;


static void checkerr(OCIError *errhp, sword status)
{
    text errbuf[512];
    ub4 buflen;
    sb4 errcode;
    string er;

    if (status == OCI_SUCCESS) return;

    switch (status)
    {
        case OCI_SUCCESS_WITH_INFO:
            cout << "Error - OCI_SUCCESS_WITH_INFO" << endl;
            OCIErrorGet ((dvoid *) errhp, (ub4) 1, (text *) NULL, &errcode,
                    errbuf, (ub4) sizeof(errbuf), (ub4) OCI_HTYPE_ERROR);
            er=(char*)errbuf;
            cout << "Error -" << er << endl;
            break;
        case OCI_NEED_DATA:
            cout << "Error - OCI_NEED_DATA" << endl;
            break;
        case OCI_NO_DATA:
            cout << "Error - OCI_NO_DATA" << endl;
            break;
        case OCI_ERROR:
            OCIErrorGet ((dvoid *) errhp, (ub4) 1, (text *) NULL, &errcode,
                    errbuf, (ub4) sizeof(errbuf), (ub4) OCI_HTYPE_ERROR);
            er=(char*)errbuf;
            cout << "Error -" << er << endl;
            break;
        case OCI_INVALID_HANDLE:
            cout << "Error - OCI_INVALID_HANDLE" << endl;
            break;
        case OCI_STILL_EXECUTING:
            cout << "Error - OCI_STILL_EXECUTING" << endl;
            break;
        case OCI_CONTINUE:
            cout << "Error - OCI_CONTINUE" << endl;
            break;
        default:
            cout << "Error - " << status << endl;
            break;
    }
}




int prepareBLOB_In_DB(string connectionString,string username, string password)
{
    string sqlQuery;
    //create environment and 	//allocate handles and data structures
    (void) OCIInitialize((ub4) OCI_DEFAULT, (dvoid *)0,
            (dvoid * (*)(dvoid *, size_t)) 0,
            (dvoid * (*)(dvoid *, dvoid *, size_t))0,
            (void (*)(dvoid *, dvoid *)) 0 );

    (void) OCIEnvInit( (OCIEnv **) &envhp, OCI_DEFAULT, (size_t) 0,
            (dvoid **) 0 );

    (void) OCIHandleAlloc( (dvoid *) envhp, (dvoid **) &errhp, OCI_HTYPE_ERROR,
            (size_t) 0, (dvoid **) 0);

    /* server contexts */
    (void) OCIHandleAlloc( (dvoid *) envhp, (dvoid **) &srvhp, OCI_HTYPE_SERVER,
            (size_t) 0, (dvoid **) 0);

    (void) OCIHandleAlloc( (dvoid *) envhp, (dvoid **) &svchp, OCI_HTYPE_SVCCTX,
            (size_t) 0, (dvoid **) 0);

    //(void) OCIServerAttach( srvhp, errhp, (text *)"phoenix.cise.ufl.edu:1521/orcl", strlen("phoenix.cise.ufl.edu:1521/orcl"), 0);

    (void) OCIServerAttach( srvhp, errhp, (text *)connectionString.c_str(), strlen(connectionString.c_str()), 0);
    /* set attribute server context in the service context */
    (void) OCIAttrSet( (dvoid *) svchp, OCI_HTYPE_SVCCTX, (dvoid *)srvhp,
            (ub4) 0, OCI_ATTR_SERVER, (OCIError *) errhp);

    (void) OCIHandleAlloc((dvoid *) envhp, (dvoid **)&authp,
            (ub4) OCI_HTYPE_SESSION, (size_t) 0, (dvoid **) 0);

    (void) OCIAttrSet((dvoid *) authp, (ub4) OCI_HTYPE_SESSION,
            (dvoid *) username.c_str(), (ub4) strlen((char *)username.c_str()),
            (ub4) OCI_ATTR_USERNAME, errhp);

    (void) OCIAttrSet((dvoid *) authp, (ub4) OCI_HTYPE_SESSION,
            (dvoid *) password.c_str(), (ub4) strlen((char *)password.c_str()),
            (ub4) OCI_ATTR_PASSWORD, errhp);

    //connect to server and being session
    checkerr(errhp, OCISessionBegin ( svchp,  errhp, authp, OCI_CRED_RDBMS,
                (ub4) OCI_DEFAULT));

    (void) OCIAttrSet((dvoid *) svchp, (ub4) OCI_HTYPE_SVCCTX,
            (dvoid *) authp, (ub4) 0,
            (ub4) OCI_ATTR_SESSION, errhp);

    checkerr(errhp, OCIHandleAlloc( (dvoid *) envhp, (dvoid **) &stmthp,
                OCI_HTYPE_STMT, (size_t) 0, (dvoid **) 0));


/*
    //first clear all rows  
    checkerr(errhp, OCIStmtPrepare(stmthp, errhp, (text *)"DELETE FROM test_iblobs",
                (ub4)strlen("DELETE FROM test_iblobs"),
                (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT));

    checkerr(errhp, OCIStmtExecute(svchp, stmthp, errhp, (ub4) 1, (ub4) 0,
                (CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_COMMIT_ON_SUCCESS));

    uint rows_deleted;

    checkerr(errhp, OCIAttrGet ( stmthp, OCI_HTYPE_STMT, &rows_deleted, 0, 
                OCI_ATTR_ROW_COUNT, errhp )); 

    cout << rows_deleted << " rows were deleted from the 'TEST_IBLOBS' table " << endl;

    sqlQuery = "INSERT INTO usstates (id, state) VALUES(1, region(EMPTY_BLOB()))";

    //cout << "sqlQuery = " << sqlQuery << endl;
    checkerr(errhp, OCIStmtPrepare(stmthp, errhp, (text *)(sqlQuery.c_str()),
                (ub4)strlen(sqlQuery.c_str()), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT));

    status = OCIStmtExecute(svchp, stmthp, errhp, (ub4) 1, (ub4) 0,
            (CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT);

    if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO)
    {
        cout << "Inserted empty blob with id 1" << endl;

    }else{
        text errbuf[512];
        ub4 buflen;
        sb4 errcode;

        OCIErrorGet (errhp, (ub4) 1, (text *) NULL, &errcode,
                errbuf, (ub4) sizeof(errbuf), (ub4) OCI_HTYPE_ERROR);

        string er=(char*)errbuf;
        cout << "Error: " << er << endl;
        throw runtime_error("Unable to insert tuple.");
    }

    checkerr(errhp, OCITransCommit(svchp, errhp, (ub4)0));
*/
    //allocate lob descriptor
    if (OCIDescriptorAlloc((dvoid *) envhp, (dvoid **) &mylob,
                (ub4)OCI_DTYPE_LOB, (size_t) 0, (dvoid **) 0))
    {
        cout << "FAILED: OCIDescriptorAlloc()" << endl;
        return OCI_ERROR;
    }

    //issue SQL and process data
    sqlQuery = "SELECT id, s.state.data FROM usstates s where id=1 FOR UPDATE";


    checkerr(errhp, OCIStmtPrepare(stmthp, errhp, (text *)sqlQuery.c_str(),
                (ub4)strlen(sqlQuery.c_str()),
                (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT));

    checkerr(errhp, OCIDefineByPos(stmthp, &defnp, errhp, 1, (dvoid *) &myid,
                (sword) sizeof(int), SQLT_INT, (dvoid *) 0, (ub2 *)0,
                (ub2 *)0, OCI_DEFAULT));


    checkerr(errhp, OCIDefineByPos(stmthp, &defnp2, errhp, (ub4) 2,
                (dvoid *) &mylob, (sb4) -1, (ub2) SQLT_BLOB,
                (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) OCI_DEFAULT));



    // execute and fetch
    if (status = OCIStmtExecute(svchp, stmthp, errhp, (ub4) 1, (ub4) 0,
                (CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT))
    {
        if (status == OCI_NO_DATA)
            cout << "No data returned!" << endl;
        else
        {
            checkerr(errhp, status);
            if (envhp)
                (void) OCIHandleFree((dvoid *) envhp, OCI_HTYPE_ENV);
            return OCI_ERROR;
        }
    }else

        if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO)
        {

            ub4 loblen = 0;
            cout << "Retrieved tuple with ID: " << myid << endl;
        }
    return 1;
    ///////////////////////////////////////////////////////
    //
}

void closeConnection(){
    OCILobClose(svchp, errhp, mylob);
    checkerr(errhp, OCITransCommit(svchp, errhp, (ub4)0));
    if (envhp)
        (void) OCIHandleFree((dvoid *) envhp, OCI_HTYPE_ENV);

}

int main(int argc, char* argv[])
{
    // ESTABLISH CONNECTION TO THE DATABASE
    prepareBLOB_In_DB(string("phoenix.cise.ufl.edu:1521/orcl"), string(username), string(password));


    // DECLARE A GRAMMAR FOR THE NEW DATA TYPE
    char* grammarFile = "/home/aistdev/TSS/Segment.tss";

    cout<<mylob<<endl;
    //iBlobStore * store = new iBlobOracleStore(mylob, errhp, svchp);
    iBlobStore * store = new iBlobOracleStore(mylob, errhp, svchp);
    iBlob p (store, false);

    p.printStats();
    // CREATE A TSS OBJECT AND PROVIDE THE GRAMMAR
    try{
        TSS t(grammarFile, true);
        cout << "DEFINE DATA SEGMENTS " << endl;

        // DEFINE SOME SEGMENT DATA
        double f0OSeg0[] = {1.0,3.0,1.5,0.5};
        double f0OSeg1[] = {1.5,0.5,3.0,0.5};
        double f0OSeg2[] = {3.0,0.5,3.0,2.0};
        double f0OSeg3[] = {3.0, 2.0, 1.0,3.0};
        double f0HSeg0[] = {2.0,1.0, 2.5,1.0};
        double f0HSeg1[] = {2.5,1.0,2.0,2.0};
        double f0HSeg2[] = {2.0,2.0,2.0,1.0};

        double f1OSeg0[] = {5.0,1.0,7.0,1.0};
        double f1OSeg1[] = {7.0,1.0,6.0,3.0};
        double f1OSeg2[] = {6.0,3.0,5.0,1.0};


        //Path testPath = t.createPath();
        // CREATE SOME REQUIRED PATHS
        //region.vFace[0].vocycle.vOseg[0]
        Path pface0 = t.createPath("region.vface[0]", &p);
        Path pface0O = pface0+"vocycle";
        Path pface0OSeg0 = pface0O + "vOseg[0]";
        Path pface0OSeg0lpt = pface0OSeg0 + "lpt";
        cout<<"isList = "<<pface0OSeg0lpt.isList()<<endl;
        cout<<"isBase = "<<pface0OSeg0lpt.isBO()<<endl;
        Path pface0OSeg0lpt0 = pface0OSeg0lpt+ "[0]";
        cout<<"isList = "<<pface0OSeg0lpt0.isList()<<endl;
        cout<<"isBase = "<<pface0OSeg0lpt0.isBO()<<endl;

        Path pface0OSeg0lpt1 = pface0OSeg0lpt+ "[1]";
        cout<<"isList = "<<pface0OSeg0lpt1.isList()<<endl;
        cout<<"isBase = "<<pface0OSeg0lpt1.isBO()<<endl;

        Path pface0OSeg0lpt2 = pface0OSeg0lpt+ "[2]";
        cout<<"isList = "<<pface0OSeg0lpt2.isList()<<endl;
        cout<<"isBase = "<<pface0OSeg0lpt2.isBO()<<endl;

        Path pindex = t.createPath("region.vindex[0]", &p);

        cout<<"Paths constructed"<<endl;

        // WRITE VALUES TO THE PATHS
        try{
  //          cout<<pface0OSeg0lpt.set(f0OSeg0,sizeof(f0OSeg0)/sizeof(double));
  //          cout<<pface0OSeg0lpt0.set(f0OSeg0[3]);
  //          cout<<pface0OSeg0lpt.append(f0OSeg1, sizeof(f0OSeg1)/sizeof(double));
  //          cout<<"Setting reference "<<endl;
  //          cout<<pindex.set(pface0OSeg0)<<endl;
  //          cout<<"Setting reference done ! "<<endl;
        }
        catch(string s)
        {
            cout<<s<<endl;
            cout<<"error";
            return 0;
        }
        catch(char *s)
        {
            cout<<s;
            return 0;
        }

        cout<<endl<<"Reading back"<<endl;
        double *p;

        //     pface1OSeg2.removeObj();

        // READ BACK THE VALUE FROM face[0].outerCycle.Segment[1]

        try{
            //pindex = pindex + "lpt";
            //pindex.readDoubleArray(p,sizeof(p));
            //cout << t.readString(pregionlabel);
            int size;
            //Path pindexlpt = pindex + "lpt";
            pface0OSeg0lpt.read(p, size);
            for(int i = 0; i < size; i++)
                cout<<"p[] "<<p[i]<<endl;

            closeConnection();
            return 0;
        }
        catch(char const *s)
        {
            cout<<s<<endl;
            cout<<"Error in reading"<<endl;
            return 0;
        }
        catch(string s)
        {
            cout<<s;
            return 0;
        }

        cout<<"("<<p[0]<<","<<p[1]<<") - ("<<p[2]<<","<<p[3]<<")"<<endl;

    }
    catch(string s)
    {
        cout<<s<<endl;
        return 0;
    }
    catch(char *s)
    {
        cout<<s<<endl;
        return 0;
    }
}
