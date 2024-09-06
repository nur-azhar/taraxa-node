#pragma once
#include "storage/migration/migration_base.hpp"

namespace taraxa::storage::migration {
class PeriodDataDagBlockMigration : public migration::Base {
 public:
  PeriodDataDagBlockMigration(std::shared_ptr<DbStorage> db);
  std::string id() override;
  uint32_t dbVersion() override;
  void migrate(logger::Logger& log) override;
};
}  // namespace taraxa::storage::migration