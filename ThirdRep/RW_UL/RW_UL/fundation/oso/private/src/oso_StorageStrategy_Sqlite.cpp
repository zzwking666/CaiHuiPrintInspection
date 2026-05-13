#include "oso_StorageStrategy_Sqlite.hpp"
#include "oso_core.h"
#include <sstream>
#include <stdexcept>

namespace rw {
	namespace oso {
		bool StorageStrategy_Sqlite::save(const ObjectStoreAssembly& assembly, const std::filesystem::path& fileName) {
			sqlite3* db;
			if (sqlite3_open(fileName.string().c_str(), &db) != SQLITE_OK) {
				return false;
			}

			const char* createTableSQL = R"(
                CREATE TABLE IF NOT EXISTS ObjectStore (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    parentId INTEGER,
                    name TEXT,
                    type TEXT,
                    value TEXT
                );
            )";
			if (sqlite3_exec(db, createTableSQL, nullptr, nullptr, nullptr) != SQLITE_OK) {
				sqlite3_close(db);
				return false;
			}

			saveAssembly(db, std::make_shared<ObjectStoreAssembly>(assembly), 0);

			sqlite3_close(db);
			return true;
		}

		std::shared_ptr<ObjectStoreAssembly> StorageStrategy_Sqlite::load(const std::filesystem::path& fileName) {
			sqlite3* db;
			if (sqlite3_open(fileName.string().c_str(), &db) != SQLITE_OK) {
				throw std::runtime_error("Failed to open database");
			}

			auto assembly = std::make_shared<ObjectStoreAssembly>();
			loadAssembly(db, assembly, 0);

			sqlite3_close(db);
			return assembly;
		}

		std::string StorageStrategy_Sqlite::getFormatString(const ObjectStoreAssembly& assembly) {
			return getString(std::make_shared<ObjectStoreAssembly>(assembly));
		}

		void StorageStrategy_Sqlite::saveItem(sqlite3* db, const std::shared_ptr<ObjectStoreItem> item, int parentId) {
			const char* insertSQL = R"(
                INSERT INTO ObjectStore (parentId, name, type, value) VALUES (?, ?, ?, ?);
            )";
			sqlite3_stmt* stmt;
			if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
				throw std::runtime_error("Failed to prepare statement");
			}

			sqlite3_bind_int(stmt, 1, parentId);
			sqlite3_bind_text(stmt, 2, item->getName().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_text(stmt, 3, item->getTypeString().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_text(stmt, 4, item->getValueAsString().c_str(), -1, SQLITE_TRANSIENT);

			if (sqlite3_step(stmt) != SQLITE_DONE) {
				sqlite3_finalize(stmt);
				throw std::runtime_error("Failed to execute statement");
			}

			sqlite3_finalize(stmt);
		}

		void StorageStrategy_Sqlite::saveAssembly(sqlite3* db, const std::shared_ptr<ObjectStoreAssembly> assembly, int parentId) {
			const char* insertSQL = R"(
                INSERT INTO ObjectStore (parentId, name, type) VALUES (?, ?, 'assembly');
            )";
			sqlite3_stmt* stmt;
			if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
				throw std::runtime_error("Failed to prepare statement");
			}

			sqlite3_bind_int(stmt, 1, parentId);
			sqlite3_bind_text(stmt, 2, assembly->getName().c_str(), -1, SQLITE_TRANSIENT);

			if (sqlite3_step(stmt) != SQLITE_DONE) {
				sqlite3_finalize(stmt);
				throw std::runtime_error("Failed to execute statement");
			}

			int assemblyId = sqlite3_last_insert_rowid(db);
			sqlite3_finalize(stmt);

			for (const auto& item : assembly->getItems()) {
				if (auto objectAssembly = ObjectStoreCoreToAssembly(item)) {
					saveAssembly(db, objectAssembly, assemblyId);
				}
				else if (auto objectItem = ObjectStoreCoreToItem(item)) {
					saveItem(db, objectItem, assemblyId);
				}
				else {
					throw std::runtime_error("Unknown type which is the superclass of objectStoreCore");
				}
			}
		}

		void StorageStrategy_Sqlite::loadItem(sqlite3* db, std::shared_ptr<ObjectStoreAssembly> target, int parentId) {
			const char* selectSQL = R"(
                SELECT id, name, type, value FROM ObjectStore WHERE parentId = ? AND type != 'assembly';
            )";
			sqlite3_stmt* stmt;
			if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr) != SQLITE_OK) {
				throw std::runtime_error("Failed to prepare statement");
			}

			sqlite3_bind_int(stmt, 1, parentId);

			while (sqlite3_step(stmt) == SQLITE_ROW) {
				auto child = std::make_shared<ObjectStoreItem>();
				child->setName(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
				std::string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
				std::string value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

				if (type == "bool") {
					child->setValueFromBool(value == "true");
				}
				else if (type == "double") {
					child->setValueFromDouble(std::stod(value));
				}
				else if (type == "int") {
					child->setValueFromInt(std::stoi(value));
				}
				else if (type == "float") {
					child->setValueFromFloat(std::stof(value));
				}
				else if (type == "long") {
					child->setValueFromLong(std::stol(value));
				}
				else if (type == "string") {
					child->setValueFromString(value);
				}
				else {
					throw std::runtime_error("Unknown type");
				}

				target->addItem(child);
			}

			sqlite3_finalize(stmt);
		}

		void StorageStrategy_Sqlite::loadAssembly(sqlite3* db, std::shared_ptr<ObjectStoreAssembly> target, int parentId) {
			const char* selectSQL = R"(
                SELECT id, name FROM ObjectStore WHERE parentId = ? AND type = 'assembly';
            )";
			sqlite3_stmt* stmt;
			if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr) != SQLITE_OK) {
				throw std::runtime_error("Failed to prepare statement");
			}

			sqlite3_bind_int(stmt, 1, parentId);

			while (sqlite3_step(stmt) == SQLITE_ROW) {
				int assemblyId = sqlite3_column_int(stmt, 0);
				auto child = std::make_shared<ObjectStoreAssembly>();
				child->setName(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));

				target->addItem(child);
				loadAssembly(db, child, assemblyId);
				loadItem(db, child, assemblyId);
			}

			sqlite3_finalize(stmt);
		}

		std::string StorageStrategy_Sqlite::getString(const std::shared_ptr<ObjectStoreAssembly> source) {
			sqlite3* db;
			std::string dbName = ":memory:"; // 使用内存数据库
			if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
				throw std::runtime_error("Failed to open database");
			}

			const char* createTableSQL = R"(
                CREATE TABLE IF NOT EXISTS ObjectStore (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    parentId INTEGER,
                    name TEXT,
                    type TEXT,
                    value TEXT
                );
            )";
			if (sqlite3_exec(db, createTableSQL, nullptr, nullptr, nullptr) != SQLITE_OK) {
				sqlite3_close(db);
				throw std::runtime_error("Failed to create table");
			}

			saveAssembly(db, source, 0);

			std::ostringstream oss;
			const char* selectSQL = R"(
                SELECT id, parentId, name, type, value FROM ObjectStore;
            )";
			sqlite3_stmt* stmt;
			if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr) != SQLITE_OK) {
				sqlite3_close(db);
				throw std::runtime_error("Failed to prepare statement");
			}

			while (sqlite3_step(stmt) == SQLITE_ROW) {
				int id = sqlite3_column_int(stmt, 0);
				int parentId = sqlite3_column_int(stmt, 1);
				const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
				const char* type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
				const char* value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

				oss << "id: " << id << ", parentId: " << parentId << ", name: " << name
					<< ", type: " << type << ", value: " << (value ? value : "null") << "\n";
			}

			sqlite3_finalize(stmt);
			sqlite3_close(db);

			return oss.str();
		}
	} // namespace oso
} // namespace rw