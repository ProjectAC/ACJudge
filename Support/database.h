//#define CPPCONN_LIB_BUILD
#define CPPCONN_PUBLIC_FUNC

#include <mysql.h>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <mysql_error.h>
#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/metadata.h>  
#include <cppconn/exception.h>
#include <cppconn/sqlstring.h>
#include "../Definations/platform.h"
#include "../Definations/types.h"

#pragma comment(lib, "libmysql.lib")
#pragma comment(lib, "mysqlclient.lib")
#pragma comment(lib, "mysqlcppconn.lib")

namespace ACJudge
{
	class Database
	{
	private:

		sql::mysql::MySQL_Driver *driver;
		sql::Connection *conn;
		sql::Statement *state;

	public:

		// [Constructor] 
		Database(std::string server, std::string username, std::string password);

		// [Interface] Get latest submission
		// This will fetch the latest submission on the currently connected server
		// In later versions this will be replaced by a queue
		Submission get_latest_submission();

		// [Interface] Get task information
		// This will query the information and data of the task 
		Task get_task(ID tid);

		// [Interface] Set grades
		// Write the grades to database after judging
		void set_grades(ID sid, Grades grades);
	};
}