#include "skyhook/client/file_skyhook.h"
#include "arrow/compute/exec/expression.h"
#include "arrow/dataset/dataset.h"
#include "arrow/dataset/file_base.h"
#include "arrow/filesystem/api.h"
#include "arrow/io/api.h"
#include "arrow/testing/gtest_util.h"
#include "arrow/util/checked_cast.h"
#include "arrow/util/iterator.h"
#include "parquet/arrow/reader.h"
#include "parquet/arrow/writer.h"

std::shared_ptr<skyhook::SkyhookFileFormat> GetSkyhookFormat() {
  // The constants below should match the parameters with
  // which the Ceph cluster is configured in integration_skyhook.sh.
  // Currently, all the default values have been used.
  std::string ceph_config_path = "/etc/ceph/ceph.conf";
  std::string ceph_data_pool = "cephfs_data";
  std::string ceph_user_name = "client.admin";
  std::string ceph_cluster_name = "ceph";
  std::string ceph_cls_name = "skyhook";
  std::shared_ptr<skyhook::RadosConnCtx> rados_ctx =
      std::make_shared<skyhook::RadosConnCtx>(ceph_config_path, ceph_data_pool,
                                              ceph_user_name, ceph_cluster_name,
                                              ceph_cls_name);
  EXPECT_OK_AND_ASSIGN(auto format,
                       skyhook::SkyhookFileFormat::Make(rados_ctx, "parquet"));
  return format;
}

std::shared_ptr<arrow::dataset::ParquetFileFormat> GetParquetFormat() {
  return std::make_shared<arrow::dataset::ParquetFileFormat>();
}

std::shared_ptr<arrow::dataset::Dataset> GetDatasetFromDirectory(
    std::shared_ptr<arrow::fs::FileSystem> fs,
    std::shared_ptr<arrow::dataset::FileFormat> format, std::string dir) {
  arrow::fs::FileSelector s;
  s.base_dir = std::move(dir);
  s.recursive = true;

  arrow::dataset::FileSystemFactoryOptions options;
  options.partitioning = std::make_shared<arrow::dataset::HivePartitioning>(
      arrow::schema({arrow::field("payment_type", arrow::int32()),
                     arrow::field("VendorID", arrow::int32())}));
  EXPECT_OK_AND_ASSIGN(auto factory, arrow::dataset::FileSystemDatasetFactory::Make(
                                         std::move(fs), s, std::move(format), options));

  arrow::dataset::InspectOptions inspect_options;
  arrow::dataset::FinishOptions finish_options;
  EXPECT_OK_AND_ASSIGN(auto schema, factory->Inspect(inspect_options));
  EXPECT_OK_AND_ASSIGN(auto dataset, factory->Finish(finish_options));
  return dataset;
}

std::shared_ptr<arrow::fs::FileSystem> GetFileSystemFromUri(const std::string& uri,
                                                            std::string* path) {
  EXPECT_OK_AND_ASSIGN(auto fs, arrow::fs::FileSystemFromUri(uri, path));
  return fs;
}

std::shared_ptr<arrow::dataset::Dataset> GetDatasetFromPath(
    std::shared_ptr<arrow::fs::FileSystem> fs,
    std::shared_ptr<arrow::dataset::FileFormat> format, std::string path) {
  EXPECT_OK_AND_ASSIGN(auto info, fs->GetFileInfo(path));
  return GetDatasetFromDirectory(std::move(fs), std::move(format), std::move(path));
}

std::shared_ptr<arrow::dataset::Scanner> GetScannerFromDataset(
    const std::shared_ptr<arrow::dataset::Dataset>& dataset,
    std::vector<std::string> columns, arrow::compute::Expression filter,
    bool use_threads) {
  EXPECT_OK_AND_ASSIGN(auto scanner_builder, dataset->NewScan());

  if (!columns.empty()) {
    ARROW_EXPECT_OK(scanner_builder->Project(std::move(columns)));
  }

  ARROW_EXPECT_OK(scanner_builder->Filter(std::move(filter)));
  ARROW_EXPECT_OK(scanner_builder->UseThreads(use_threads));
  EXPECT_OK_AND_ASSIGN(auto scanner, scanner_builder->Finish());
  return scanner;
}

int main() {
  std::string path;
  auto fs = GetFileSystemFromUri("file:///mnt/cephfs/dataset_128MB", &path);
  std::vector<std::string> columns;

  auto skyhook_format = GetSkyhookFormat();
  auto dataset = GetDatasetFromPath(fs, skyhook_format, path);
  auto scanner =
      GetScannerFromDataset(dataset, columns, arrow::compute::literal(true), true);
  EXPECT_OK_AND_ASSIGN(auto table, scanner->ToTable());
  std::cout << table->ToString() << "\n";
}
