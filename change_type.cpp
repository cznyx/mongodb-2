/* ����MongoDB���ݿ��ĵ���ԭ���ֶε��������� 
 * �Ȼ�ȡ�ĵ� BSONObj
 * �ڻ�ȡ�ֶ� BSONElement
 * �ж�ԭ�ֶ����� BSONElement.type()
 * �� BSONType ���Ƚ�
 */

// system

// std
#include <string>
#include <vector>
#include <iostream>

// mongodb
#include "mongo/client/dbclient.h"

// boost
#include <boost/lexical_cast.hpp>

using namespace std;

int main( int argc, const char **argv ) 
{
    string errmsg;
    mongo::DBClientConnection conn;
    
    // mongo���ݿ��ַ
    const char host[] = "10.0.110.13:27017";
    
    // �ĵ�����
    const char ns[] = "termcloud.log";
    
    if ( ! conn.connect(host , errmsg ) ) 
    {
        cout << "couldn't connect : " << errmsg << endl;
        return EXIT_FAILURE;
    }
   
    // ָ���ĵ����� 
    vector<string> actions;
    actions.push_back("library_uncollect");
    actions.push_back("library_collect");
    actions.push_back("library_share");
    actions.push_back("library_unsharedo");
   
    for (std::vector<string>::iterator it = actions.begin() ; it != actions.end(); ++it)
    {
        std::cout << "Query Action Type is " << *it << endl;
       
       	// query
        // �����ĵ����ͱ����ĵ�
        auto_ptr<mongo::DBClientCursor> cursor = conn.query(ns, 
            QUERY("action_type" << (*it).c_str()));
        if (!cursor.get()) 
        {
            cout << "query error" << endl;
            return EXIT_FAILURE;
        }
    	
    	string str_lib_id;
    	int create_time = 0;
    	int library_id = 0;
    	
        while ( cursor->more() ) 
        {
            cout <<  endl;
            cout <<  endl;
            
            cout << "has element, action type : " << *it << "." << endl;
        	
            mongo::BSONObj obj = cursor->next();
            try
            {
            	mongo::BSONElement element = obj.getFieldDotted("library_id");
            	
            	if(element.type() == mongo::NumberInt)
            	{
            		// NumberInt 
            		// cout << "Well " << endl;
            	}
            	else if(element.type() == mongo::String)
            	{
            		// String 
            		str_lib_id = obj["library_id"].String();
                	create_time = obj["create_time"].Int();
                	cout << "Query Mongo:"  << endl;
                	cout << "library_id  =" << str_lib_id  << endl;
                	cout << "create_time =" << create_time << endl;
                	
                	library_id = boost::lexical_cast<int>(str_lib_id);
                
                	conn.update( ns , 
                    	mongo::BSONObjBuilder().append( "create_time" , create_time).obj() , BSON("$set"<<BSON("library_id"<<library_id)));
                    		
                    cout << "Done! " << *it << endl;
            	}
            }
            catch(mongo::MsgAssertionException &e)
            {
               
                cout << "error : mongo::obj['library_id'].String(), library_id=" << endl;
                cout << "----------------------------------------------------------------------------" << endl;
                cout << obj.jsonString() << endl;
                cout << "----------------------------------------------------------------------------" << endl;
                cout << "error : " << e.what() << endl;
                
                continue;
            }
            catch(...)
            {
                
                cout << "error : boost::lexical_cast<int>, library_id=" << str_lib_id << endl;
                
                continue;
            }
        }
    }
    
    return 0;
}

