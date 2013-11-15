#include <string>

#include "mongo/client/dbclient.h"

using namespace std;

int main( int argc, const char **argv ) 
{
	string errmsg;
    mongo::DBClientConnection conn;

    if ( ! conn.connect("10.0.110.13:27017" , errmsg ) ) 
    {
        cout << "couldn't connect : " << errmsg << endl;
        return EXIT_FAILURE;
    }
    
    const char * ns = "termcloud.operation";
    // Insert 
    mongo::BSONObjBuilder daily_user_bson;
    daily_user_bson.append("user", 15);
    daily_user_bson.append("register", 9);
    daily_user_bson.append("iol", 5);
    
    mongo::BSONObjBuilder total_user_bson;
    total_user_bson.append("user", 1025);
    total_user_bson.append("register", 859);
    total_user_bson.append("iol", 555);
    
    mongo::BSONObjBuilder daily_term_bson;
    daily_term_bson.append("library", 145);
    daily_term_bson.append("term", 3652447);
    
    mongo::BSONObjBuilder total_term_bson;
    total_term_bson.append("library", 1245786);
    total_term_bson.append("term", 41256878);
    
    mongo::BSONObjBuilder users_bson;
    users_bson.append("total", total_user_bson.obj());
   	users_bson.append("daily", daily_user_bson.obj());
    
    mongo::BSONObjBuilder terms_bson;
    terms_bson.append("total", total_term_bson.obj());
   	terms_bson.append("daily", daily_term_bson.obj());
    
    mongo::BSONObjBuilder obj;
    obj.append( "stat_time" , "20131102" );
    obj.append( "users" , users_bson.obj() );
    obj.append( "terms" , terms_bson.obj() );
    
    conn.insert( ns , obj.obj() );
   
    
    // update  
    // 更新子文档中的单个数据项 
    conn.update( ns , mongo::BSONObjBuilder().append( "stat_time" , "20131102" ).obj() , BSON("$set"<<BSON("users.daily.iol"<<66)));
  
    // 更新整个子文档 
    // step.1 创建子文档并赋值 
    mongo::BSONObjBuilder total_term_bson;
    total_term_bson.append("library", 6000);
    total_term_bson.append("term", 66660000);
    // step.2 更新子文档
    conn.update( ns , mongo::BSONObjBuilder().append( "stat_time" , "20131102" ).obj() , BSON("$set"<<BSON("terms.total"<<total_term_bson.obj())));
    
    // 更新一个不存在的子文档
    mongo::BSONObjBuilder total_collect_bson;
    total_collect_bson.append("library", 2015);
    total_collect_bson.append("term", 58000);
    total_collect_bson.append("library_time", 8060);
    total_collect_bson.append("term_time", 400000);
    
    mongo::BSONObjBuilder daily_collect_bson;
    daily_collect_bson.append("library_time", 2015);
    daily_collect_bson.append("term_time", 58000);
    daily_collect_bson.append("library_cancel_time", 860);
    daily_collect_bson.append("term_cancel_time", 8000);
    
    mongo::BSONObjBuilder collecting_bson;
    collecting_bson.append( "total" , total_collect_bson.obj() );
    collecting_bson.append( "daily" , daily_collect_bson.obj() );
    
    conn.update( ns , mongo::BSONObjBuilder().append( "stat_time" , "20131102" ).obj() , BSON("$set"<<BSON("collecting"<<collecting_bson.obj())));
   
   	// query
    // 查询一个嵌套子文档中的某个字段内容
    auto_ptr<mongo::DBClientCursor> cursor = conn.query(ns, 
        QUERY("stat_time"<<"20131114"));
    if (!cursor.get()) 
    {
        cout << "query error" << endl;
        return 0;
    }
	
	int yestoday_total = 0;
	
    while ( cursor->more() ) 
    {
    	cout << "has element. " << endl;
    	
        mongo::BSONObj obj = cursor->next();
        try
        {
            yestoday_total = ((obj.getObjectField("terms")).getObjectField("total"))["has_library_user"].Int();
        }
        catch(mongo::MsgAssertionException &e)
        {
            cout << "error : " << e.what() << endl;
            continue;
        }
    }
    
    cout << "data = " << yestoday_total << endl;
    
    return 0;
}
