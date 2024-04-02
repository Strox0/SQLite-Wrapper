#include <iostream>
#include "SQLite.h"
#include <vector>

struct TestDataS
{
	int multi;
	std::string name;
	std::string address;
	double base;
};

std::ostream& operator<<(std::ostream& ss, TestDataS td) {

	ss << td.name << "\t" << td.address << "\t" << td.base << "\t" << td.multi;

	return ss;
}

int main() 
{
	//non existent file test
	{
		SQLite sqpl("notexist.db", OpenType::Read);
	}
	
	//create / open file
	remove("test.db");
	SQLite sql("test.db");

	//Create columns
	{
		std::vector<SQLite::Colum> colums;
		colums.emplace_back("name", SQLite::DataType::TEXT);
		colums.emplace_back("multi", SQLite::DataType::INTEGER);
		colums.emplace_back("address", SQLite::DataType::TEXT);
		colums.emplace_back("base", SQLite::DataType::DOUBLE);

		sql.CreateTableWithKey("TestTableWithKey", colums);
	}

	//Populate table
	{
		std::string names[3] = { "'Firts name'","'Second name'","'Third name'" };
		std::string adresses[3] = { "'St 22'","'St 40'","'St 100'" };

		for (size_t i = 0; i < 3; i++)
		{
			std::vector<SQLite::Values> values;

			double p = (static_cast<double>(i) * 2023 / 100) * 0.69;

			values.push_back({ "name",names[i] });
			values.push_back({ "multi",std::to_string(i * 10) });
			values.push_back({ "address",adresses[i] });
			values.push_back({ "base",std::to_string(p) });

			sql.Insert("TestTableWithKey", values);
		}
	}

	//Query data
	{
		std::vector<std::string> clsm = { "name","multi","base","address" };
		std::vector<SQLite::Row> rows = sql.QueryData("TestTableWithKey", clsm);

		TestDataS ds[3];

		for (size_t i = 0; i < rows.size(); i++)
		{
			rows[i].GetValue("address", ds[i].address);
			rows[i].GetValue("base", ds[i].base);
			rows[i].GetValue("multi", ds[i].multi);
			rows[i].GetValue("name", ds[i].name);
		}

		std::cout << ds[0] << std::endl;
		std::cout << ds[1] << std::endl;
		std::cout << ds[2] << std::endl;
	}

	//Update data
	{
		std::vector<SQLite::Values> values;
		values.push_back({ "name","'Updated name'" });
		values.push_back({ "multi","100" });
		values.push_back({ "address","'Updated address'" });
		values.push_back({ "base","100.0" });

		sql.Update("TestTableWithKey", values, "name='Firts name'");
	}

	//Query data
	{
		std::vector<std::string> clsm = { "name","multi","base","address" };
		std::vector<SQLite::Row> rows = sql.QueryData("TestTableWithKey", clsm);

		TestDataS ds[3];

		for (size_t i = 0; i < rows.size(); i++)
		{
			rows[i].GetValue("address", ds[i].address);
			rows[i].GetValue("base", ds[i].base);
			rows[i].GetValue("multi", ds[i].multi);
			rows[i].GetValue("name", ds[i].name);
		}

		std::cout << ds[0] << std::endl;
		std::cout << ds[1] << std::endl;
		std::cout << ds[2] << std::endl;
	}

	//Delete data
	{
		sql.Delete("TestTableWithKey", "name='Updated name'");
	}

	//Query data
	{
		std::vector<std::string> clsm = { "name","multi","base","address" };
		std::vector<SQLite::Row> rows = sql.QueryData("TestTableWithKey", clsm);

		TestDataS ds[3];

		for (size_t i = 0; i < rows.size(); i++)
		{
			rows[i].GetValue("address", ds[i].address);
			rows[i].GetValue("base", ds[i].base);
			rows[i].GetValue("multi", ds[i].multi);
			rows[i].GetValue("name", ds[i].name);
		}

		std::cout << ds[0] << std::endl;
		std::cout << ds[1] << std::endl;
		std::cout << ds[2] << std::endl;
	}

	sql.Close();

	std::cin.get();
	return 0;
}