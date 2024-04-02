#include "SQLite.h"
#include <iostream>

void RemoveChar(std::string& str, char ch)
{
	bool finished = false;
	while (!finished)
	{
		size_t pos = str.find_first_of(ch);
		if (pos != std::string::npos)
			str.erase(pos, 1);
		else
			finished = true;
	}
}


SQLite::SQLite(std::string_view database_path, OpenType o_type)
{
	int flag = SQLITE_OPEN_READONLY;
	if (o_type == OpenType::Create)
		flag = SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE;
	else if (o_type == OpenType::Write)
		flag = SQLITE_OPEN_READWRITE;

	int rc = sqlite3_open_v2(database_path.data(), &m_db, flag, nullptr);

	if (rc) 
	{
		m_error = 1;
	}
}

SQLite::~SQLite()
{
	Close();
}

void SQLite::Delete(std::string table_name, std::string clause)
{
	std::string stm = "DELETE FROM ";
	RemoveChar(table_name, ' ');
	stm += table_name;
	if (!clause.empty())
	{
		stm += " WHERE ";
		stm += clause;
		stm += ";";
	}
	m_mutex.lock();
	sqlite3_exec(m_db, stm.c_str(), 0, 0, nullptr);
	m_mutex.unlock();
}

void SQLite::Close()
{
	sqlite3_close(m_db);
}

std::vector<SQLite::Colum> SQLite::GetColums(std::string table_name)
{
	RemoveChar(table_name, ' ');
	sqlite3_stmt* stm;
	std::string sqlq = "PRAGMA table_info(";
	sqlq += table_name;
	sqlq += ")";
	int rc = sqlite3_prepare_v2(m_db, sqlq.c_str(), -1, &stm, NULL);
	if (rc != SQLITE_OK) 
	{
		std::cerr << "Error preparing statement: " << sqlite3_errmsg(m_db) << std::endl;
		return std::vector<Colum>();
	}

	std::vector<Colum> column_names;
	while (sqlite3_step(stm) == SQLITE_ROW) {
		std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stm, 1));
		std::string type = reinterpret_cast<const char*>(sqlite3_column_text(stm, 2));
		DataType dt = TranslatecolumType(type);
		column_names.emplace_back(name, dt);
	}

	sqlite3_finalize(stm);

	return column_names;
}

void SQLite::Update(std::string table_name, std::vector<SQLite::Values> values, std::string clause)
{
	std::string stm = "UPDATE ";
	RemoveChar(table_name, ' ');
	stm += table_name;
	stm += " SET ";
	for (int i = 0; i < values.size(); i++)
	{
		RemoveChar(values[i].colum_name, ' ');
		stm += values[i].colum_name;
		stm += " = ";
		stm += values[i].value;
		if (i + 1 != values.size())
			stm += ",";
	}
	if (!clause.empty())
	{
		stm += " WHERE ";
		stm += clause;
		stm += ";";
	}
	char* errmsg = nullptr;
	m_mutex.lock();
	sqlite3_exec(m_db, stm.c_str(), 0, 0, &errmsg);
	m_mutex.unlock();
}

std::vector<SQLite::Row> SQLite::QueryData(std::string table_name, std::vector<std::string> colums, std::string clause)
{
	std::vector<SQLite::Row> ret;
	RemoveChar(table_name, ' ');
	std::string stm = "SELECT ";
	for (int i = 0; i < colums.size(); i++)
	{
		RemoveChar(colums[i], ' ');
		stm += colums[i];
		if (i + 1 != colums.size())
			stm += ",";
	}
	stm += " FROM ";
	stm += table_name;
	if (!clause.empty())
	{
		stm += " WHERE ";
		stm += clause;
		stm += ';';
	}
	sqlite3_stmt* stmt;
	m_mutex.lock();
	if (sqlite3_prepare_v2(m_db, stm.c_str(), -1, &stmt, NULL) != SQLITE_OK) 
	{
		std::cerr << "Error preparing statement: " << sqlite3_errmsg(m_db) << std::endl;
		m_mutex.unlock();
		return std::vector<Row>();
	}

	while (sqlite3_step(stmt) == SQLITE_ROW) 
	{
		std::vector<Data> cols;
		int col_num = sqlite3_column_count(stmt);
		for (int i = 0; i < col_num; i++)
		{
			int type = sqlite3_column_type(stmt, i);
			std::string col_name = sqlite3_column_name(stmt, i);
			switch (type)
			{
			case SQLITE_TEXT:
				cols.emplace_back(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i))), DataType::TEXT, col_name);
				break;
			case SQLITE_BLOB:
				cols.emplace_back(sqlite3_column_blob(stmt, i), DataType::VOID, col_name);
				break;
			case SQLITE_FLOAT:
				cols.emplace_back(sqlite3_column_double(stmt, i), DataType::DOUBLE, col_name);
				break;
			case SQLITE_INTEGER:
				cols.emplace_back(sqlite3_column_int(stmt, i), DataType::INTEGER, col_name);
				break;
			default:
				break;
			}
		}
		ret.push_back(cols);
	}

	sqlite3_finalize(stmt);
	m_mutex.unlock();
	return ret;
}

void SQLite::CreateTableWithKey(std::string table_name, std::vector<Colum> colums)
{
	std::string stm = "CREATE TABLE IF NOT EXISTS ";
	RemoveChar(table_name, ' ');
	stm += table_name;
	stm += " (id INTEGER PRIMARY KEY";
	for (int i = 0; i < colums.size(); i++)
	{
		RemoveChar(colums[i].name, ' ');
		stm += ",";
		stm += colums[i].name;
		stm += " ";
		stm += TranslatecolumType(colums[i].type);
	}
	stm += ");";
	m_mutex.lock();
	sqlite3_exec(m_db, stm.c_str(), 0, 0, nullptr);
	m_mutex.unlock();
}

void SQLite::CreateTable(std::string table_name, std::vector<Colum> colums)
{
	std::string stm = "CREATE TABLE IF NOT EXISTS ";
	RemoveChar(table_name, ' ');
	stm += table_name;
	stm += " (";
	RemoveChar(colums[0].name, ' ');
	stm += colums[0].name;
	stm += " ";
	stm += TranslatecolumType(colums[0].type);
	stm += " UNIQUE";
	if (colums.size() > 1)
	{
		stm += ",";
	}
	for (int i = 1; i < colums.size(); i++)
	{
		RemoveChar(colums[i].name, ' ');
		stm += colums[i].name;
		stm += " ";
		stm += TranslatecolumType(colums[i].type);
		if (i + 1 != colums.size())
			stm += ",";
	}
	stm += ");";

	m_mutex.lock();
	sqlite3_exec(m_db, stm.c_str(), 0, 0, nullptr);
	m_mutex.unlock();
}

void SQLite::Insert(std::string table_name, std::vector<Values> values)
{
	std::string stm = "INSERT OR IGNORE INTO ";
	RemoveChar(table_name, ' ');
	stm += table_name;
	stm += " (";
	for (int i = 0; i < values.size(); i++)
	{
		RemoveChar(values[i].colum_name, ' ');
		stm += values[i].colum_name;
		if (i + 1 != values.size())
			stm += ",";
	}
	stm += ") VALUES (";
	for (int i = 0; i < values.size(); i++)
	{
		stm += values[i].value;
		if (i + 1 != values.size())
			stm += ",";
	}
	stm += ");";
	m_mutex.lock();
	sqlite3_exec(m_db, stm.c_str(), 0, 0, nullptr);
	m_mutex.unlock();
}

void SQLite::ReIndex(std::string table_name)
{
	std::string stm = "REINDEX";
	if (!table_name.empty())
	{
		stm += ' ';
		RemoveChar(table_name, ' ');
		stm += table_name;
	}
	stm += ";";
	m_mutex.lock();
	sqlite3_exec(m_db, stm.c_str(), 0, 0, nullptr);
	m_mutex.unlock();
}

std::string SQLite::TranslatecolumType(DataType type)
{
	switch (type)
	{
	case SQLite::DataType::INTEGER:
		return "INTEGER";
		break;
	case SQLite::DataType::DOUBLE:
		return "REAL";
		break;
	case SQLite::DataType::TEXT:
		return "TEXT";
		break;
	case SQLite::DataType::VOID:
		return "BLOB";
		break;
	default:
		return std::string();
		break;
	}
}

void SQLite::Vacuum()
{
	m_mutex.lock();
	sqlite3_exec(m_db, "VACUUM;", 0, 0, nullptr);
	m_mutex.unlock();
}

SQLite::DataType SQLite::TranslatecolumType(const std::string& type)
{
	if (type == "INT" || type == "INTEGER" || type == "INTIGER")
	{
		return SQLite::DataType::INTEGER;
	}
	else if (type == "REAL")
	{
		return SQLite::DataType::DOUBLE;
	}
	else if (type == "TEXT")
	{
		return SQLite::DataType::TEXT;
	}
	else if (type == "BLOB")
	{
		return SQLite::DataType::VOID;
	}
	return DataType();
}

void SQLite::Row::GetValue(int coli,int& output)
{
	int* tmp = std::get_if<int>(&colum_values[coli].data);
	if (tmp != nullptr)
		output = *tmp;
}

void SQLite::Row::GetValue(int coli, double& output)
{
	double* tmp = std::get_if<double>(&colum_values[coli].data);
	if (tmp != nullptr)
		output = *tmp;
}

void SQLite::Row::GetValue(int coli, const void* output)
{
	const void* tmp = std::get_if<const void*>(&colum_values[coli].data);
	if (tmp != nullptr)
		output = tmp;
}

void SQLite::Row::GetValue(int coli, std::string& output)
{
	std::string* tmp = std::get_if<std::string>(&colum_values[coli].data);
	if (tmp != nullptr)
		output = *tmp;
}

void SQLite::Row::GetValue(const std::string col_name, int& output)
{
	for (auto& i : colum_values)
	{
		if (i.name == col_name)
		{
			int* tmp = std::get_if<int>(&i.data);
			if (tmp != nullptr)
				output = *tmp;
			break;
		}
	}
}

void SQLite::Row::GetValue(const std::string col_name, double& output)
{
	for (auto& i : colum_values)
	{
		if (i.name == col_name)
		{
			double* tmp = std::get_if<double>(&i.data);
			if (tmp != nullptr)
				output = *tmp;
			break;
		}
	}
}

void SQLite::Row::GetValue(const std::string col_name, std::string& output)
{
	for (auto& i : colum_values)
	{
		if (i.name == col_name)
		{
			std::string* tmp = std::get_if<std::string>(&i.data);
			if (tmp != nullptr)
				output = *tmp;
			break;
		}
	}
}

void SQLite::Row::GetValue(const std::string col_name, const void* output)
{
	for (auto& i : colum_values)
	{
		if (i.name == col_name)
		{
			const void* tmp = std::get_if<const void*>(&i.data);
			if (tmp != nullptr)
				output = tmp;
			break;
		}
	}
}

SQLite::DataType SQLite::Row::GetType(int coli)
{
	return colum_values[coli].type;
}
