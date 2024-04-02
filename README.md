---

# SQLite-Wrapper

SQLite-Wrapper simplifies the use of the SQLite3 database library in C++ projects by providing a user-friendly wrapper class. This project encapsulates SQLite3's functionality, making database operations more straightforward and intuitive for developers.

## Features

- **Ease of Use:** By abstracting the complexity of SQLite3, this wrapper offers a simplified interface for database operations.
- **Bundled SQLite3:** The SQLite3 library is included within the project, ensuring a seamless setup and integration.

## Getting Started

### Prerequisites

Before integrating SQLite-Wrapper into your project, ensure you have a C++ compiler and standard development tools installed on your system.

### Installation

1. Clone the repository or download the source code.
2. Copy the `sqlite3` directory along with `SQLite.h` and `SQLite.cpp` files into your project directory.

### Usage

With the provided `Main.cpp` file showcasing tests for the SQLite-Wrapper library, here's how the "Usage" section of your README can be structured:

---

## Usage

### Basic Operations

1. **Initialization**: Create an instance of the `SQLite` class by specifying the database file path and the desired `OpenType`. For a new database, use `OpenType::Create`.

   ```cpp
   SQLite sql("your_database.db");
   ```

2. **Creating Tables**: Define your table schema by specifying column names and data types using the `SQLite::Colum` struct. Use the `CreateTable` or `CreateTableWithKey` method to create a new table.

   ```cpp
   std::vector<SQLite::Colum> columns = {
       {"name", SQLite::DataType::TEXT},
       {"multi", SQLite::DataType::INTEGER},
       {"address", SQLite::DataType::TEXT},
       {"base", SQLite::DataType::DOUBLE}
   };

   sql.CreateTableWithKey("YourTableName", columns);
   ```

3. **Inserting Data**: Insert data into your table by defining a list of `SQLite::Values` that match the columns in your table schema.

   ```cpp
   std::vector<SQLite::Values> values = {
       {"name", "'John Doe'"},
       {"multi", "10"},
       {"address", "'123 Main St'"},
       {"base", "1.23"}
   };

   sql.Insert("YourTableName", values);
   ```

4. **Querying Data**: Retrieve data from your table by specifying the column names you wish to retrieve and any optional SQL clauses (e.g., `WHERE` conditions).

   ```cpp
   std::vector<std::string> columns = {"name", "multi", "base", "address"};
   std::vector<SQLite::Row> rows = sql.QueryData("YourTableName", columns);
   ```

5. **Updating Data**: Update existing data in your table by specifying the new values and an optional `WHERE` clause.

   ```cpp
   std::vector<SQLite::Values> updateValues = {
       {"name", "'Jane Doe'"},
       {"multi", "20"},
       {"address", "'456 Elm St'"},
       {"base", "4.56"}
   };

   sql.Update("YourTableName", updateValues, "name='John Doe'");
   ```

6. **Deleting Data**: Remove data from your table using an optional `WHERE` clause to specify which records should be deleted.

   ```cpp
   sql.Delete("YourTableName", "name='Jane Doe'");
   ```

7. **Closing the Database**: Ensure you close the database connection when done.

   ```cpp
   sql.Close();
   ```

---

This section gives users a quick start on how to perform basic database operations with your SQLite-Wrapper library. If there are more advanced features or specific scenarios you would like to cover, feel free to add those details or let me know if you need further assistance!

## License

SQLite-Wrapper is released under the MIT License. This allows for a wide range of uses, from private to commercial, provided that the license and copyright notice are included with any substantial portions of the software.

For more details, see the LICENSE file in the project repository.

---