#include "exotic/cognition/storage_schema.hpp"
#include <sqlite3.h>
#include <cassert>
#include <cstdio>
using namespace exotic::cognition;
int main(){const char*p="storage_schema_test.db";std::remove(p);sqlite3*d=nullptr;assert(sqlite3_open(p,&d)==SQLITE_OK);apply_storage_migrations(d);assert(storage_schema_version(d)==kCurrentSchemaVersion);sqlite3_stmt*s=nullptr;assert(sqlite3_prepare_v2(d,"SELECT COUNT(*) FROM schema_migrations",-1,&s,nullptr)==SQLITE_OK);assert(sqlite3_step(s)==SQLITE_ROW);assert(sqlite3_column_int(s,0)==kCurrentSchemaVersion);sqlite3_finalize(s);apply_storage_migrations(d);assert(storage_schema_version(d)==kCurrentSchemaVersion);assert(sqlite3_prepare_v2(d,"SELECT name FROM sqlite_master WHERE type='table' AND name IN ('attention_state','belief_revision_history','adaptation_history','intentions')",-1,&s,nullptr)==SQLITE_OK);int count=0;while(sqlite3_step(s)==SQLITE_ROW)++count;assert(count==4);sqlite3_finalize(s);sqlite3_close(d);std::remove(p);std::remove("storage_schema_test.db-wal");std::remove("storage_schema_test.db-shm");return 0;}
