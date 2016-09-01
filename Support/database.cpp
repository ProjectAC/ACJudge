#include "database.h"
#include "../Lib/lib.h"

using namespace std;
using namespace sql;
using namespace sql::mysql;
using namespace ACJudge;

Database::Database(string server, string username, string password)
{
	driver = get_mysql_driver_instance();
	conn = driver->connect(server, username, password);
	state = conn->createStatement();
}

Submission Database::get_latest_submission()
{
	string SQL = "SELECT * FROM `oj_commit` WHERE ORDER BY `oj_commit`.`time`  DESC LIMIT 1";
	Submission s = {0, 0, Language::NONE};

	ResultSet *res = state->executeQuery(SQL);
	if (!(res->next()))
		return s;
	
	s.sid = res->getUInt64("sid");
	switch (res->getInt("language"))
	{
	case 0:
		s.language = Language::CPP;
		break;
	case 1:
		s.language = Language::CPP11;
		break;
	case 2:
		s.language = Language::C;
		break;
	case -1:
		s.language = Language::NONE;
		break;
	}

	return s;
}

Task Database::get_task(ID tid)
{
	Task t;
	t.tid = tid;

	ResultSet *res = state->executeQuery("SELECT * FROM `oj_commit` WHERE ORDER BY `oj_commit`.`time`  DESC LIMIT 1");
	if (!(res->next()))
		return t;

	res = state->executeQuery("SELECT * FROM `test_info` WHERE test_info`.`aid` = " + i2ss(tid));
	while (res->next())
		t.data.push_back({res->getUInt64("did"), res->getUInt("time"), res->getUInt("memory"), res->getUInt("score")});

	switch (res->getInt("type"))
	{
	case 0:
		t.type = TaskType::OI;
		break;
	case 1:
		t.type = TaskType::ACM;
		break;
	}

	switch (res->getInt("language"))
	{
	case 0:
		t.language = Language::CPP;
		break;
	case 1:
		t.language = Language::CPP11;
		break;
	case 2:
		t.language = Language::C;
		break;
	case -1:
		t.language = Language::NONE;
		break;
	}

	return t;
}

void Database::set_grades(ID sid, Grades grades)
{
	ID did = 1;
	for (auto &grade : grades)
		state->execute("INSERT INTO `oj_result` (`did`, `score`, `time`, `memory`, `result`) VALUES(" + i2ss(did) + ", " + i2ss(grade.score) + ", " + i2ss(grade.res.time) + ", " + i2ss(grade.res.space) + ", " + i2ss(grade.res.ret) + ");"),
		did++;
}