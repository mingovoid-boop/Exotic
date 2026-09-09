#pragma once
struct sqlite3;
namespace exotic::cognition {
inline constexpr int kCurrentSchemaVersion=4;
void apply_storage_migrations(sqlite3* db);
int storage_schema_version(sqlite3* db);
} // namespace exotic::cognition
