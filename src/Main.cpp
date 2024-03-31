#include <iostream>
#include "SQLite.h"
#include <vector>

struct TestDataS
{
	int multi;
	std::string name;
	std::string adress;
	double base;
};

std::ostream& operator<<(std::ostream& ss, TestDataS td) {

	ss << td.name << "\t" << td.adress << "\t" << td.base << "\t" << td.multi;

	return ss;
}

int main() 
{
	remove("test.db");
	SQLite sql("test.db");
	
	std::vector<SQLite::Colum> colums;

	colums.emplace_back("name", SQLite::DataType::TEXT);
	colums.emplace_back("multi", SQLite::DataType::INTEGER);
	colums.emplace_back("adress", SQLite::DataType::TEXT);
	colums.emplace_back("base", SQLite::DataType::DOUBLE);

	sql.CreateTableKey("test", colums);

	std::string names[3] = {"'Firts name'","'Second name'","'Third name'"};
	std::string adresses[3] = {"'St ## 22'","'St ## 40'","'SSt ## 100'"};

	for (size_t i = 0; i < 3; i++)
	{
		std::vector<SQLite::Values> values;

		double p = (static_cast<double>(i) * 2023 / 100) * 0.69;

		values.push_back({ "base",std::to_string(p)});
		values.push_back({ "name",names[i]});
		values.push_back({ "adress",adresses[i]});
		values.push_back({ "multi",std::to_string(i*10)});

		sql.Insert("test", values);
	}

	std::vector<std::string> clsm = {"name","multi","base","adress"};
	std::vector<SQLite::Row> rows = sql.QueryData("test", clsm);
	
	TestDataS ds[3];

	for (size_t i = 0; i < rows.size(); i++)
	{
		rows[i].GetValue("adress", ds[i].adress);
		rows[i].GetValue("base", ds[i].base);
		rows[i].GetValue("multi", ds[i].multi);
		rows[i].GetValue("name", ds[i].name);
	}
	
	std::cout << ds[0] << std::endl;
	std::cout << ds[1] << std::endl;
	std::cout << ds[2] << std::endl;

	sql.Close();

	std::cin.get();
	return 0;
}