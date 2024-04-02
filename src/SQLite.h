#pragma once
#include "sqlite/sqlite3.h"
#include <string>
#include <vector>
#include <variant>
#include <mutex>

//Create : Open the database file, or create it if it does not already exist.
//Read : Open the database file for reading only. Error if the file does not already exist.
//Write : Open the database file for reading and writing. Error if the file does not already exist.
enum OpenType
{
	Create,
	Read,
	Write
};


class SQLite
{
public:
	SQLite(std::string_view database_path, OpenType o_type = Create);

	enum class DataType
	{
		INTEGER,
		DOUBLE,
		TEXT,
		VOID
	};

	struct Colum
	{
		Colum(std::string name, DataType type) : name(name), type(type) {}
		std::string name;
		DataType type;
	};

	struct Values 
	{
		Values(std::string colum_name, std::string value) : colum_name(colum_name), value(value) {};
		std::string colum_name;
		std::string value;
	};
	
	struct Data 
	{
		Data(std::variant<int, double, std::string, const void*> d, DataType type,const std::string& name) : type(type),name(name) { data = d; }
		std::variant<int, double, std::string, const void*> data;
		DataType type;
		std::string name;
	};
	
	struct Row
	{
		Row(std::vector<Data> colv) : colum_values(colv) {}
	private:
		std::vector<Data> colum_values;
	public:
		void GetValue(int coli,int& output);
		void GetValue(int coli, double& output);
		void GetValue(int coli, const void* output);
		void GetValue(int coli, std::string& output);
		void GetValue(const std::string col_name, int& output);
		void GetValue(const std::string col_name, double& output);
		void GetValue(const std::string col_name, std::string& output);
		void GetValue(const std::string col_name, const void* output);
		DataType GetType(int coli);
	};

	//No whitespaces allowed
	void CreateTable(std::string table_name,std::vector<Colum> colums);

	//Creates a table with an integer primary key
	void CreateTableWithKey(std::string table_name, std::vector<Colum> colums);
	void Insert(std::string table_name, std::vector<Values> values);
	void Close();
	std::vector<Colum> GetColums(std::string table_name);

	//Returns all the data in the table from the specified columns, optionally with a 'WHERE' clause
	std::vector<Row> QueryData(std::string table_name,std::vector<std::string> collums, std::string clause = "");

	//Update data in the table in the specified columns with an optional 'WHERE' clause
	void Update(std::string table_name, std::vector<SQLite::Values> values, std::string clause = "");

	void Delete(std::string table_name, std::string clause = "");

	void ReIndex(std::string table_name = "");
	void Vacuum();

	~SQLite();

private:

	std::string TranslatecolumType(DataType type);
	DataType TranslatecolumType(const std::string& type);

	sqlite3* m_db = nullptr;
	int m_error = 0;
	std::mutex m_mutex;
};