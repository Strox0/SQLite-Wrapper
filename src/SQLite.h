#pragma once
#include "sqlite3.h"
#include <string>
#include <vector>
#include <variant>

class SQLite
{
public:
	SQLite(std::string_view database_path);

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
	void CreateTableKey(std::string table_name, std::vector<Colum> colums);
	void Insert(std::string table_name, std::vector<Values> values);
	void Close();
	std::vector<Colum> GetColums(std::string table_name);

	std::vector<Row> QueryData(std::string table_name,std::vector<std::string> collums);

	~SQLite();

private:

	std::string TranslatecolumType(DataType type);
	DataType TranslatecolumType(const std::string& type);

	sqlite3* m_db = nullptr;
};