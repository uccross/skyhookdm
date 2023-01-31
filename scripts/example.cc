#include <iostream>

#include "skyhook/client/file_skyhook.h"
#include "arrow/compute/exec/expression.h"
#include "arrow/dataset/dataset.h"
#include "arrow/dataset/file_base.h"
#include "arrow/filesystem/api.h"
#include "arrow/api.h"
#include "arrow/io/api.h"
#include "arrow/util/checked_cast.h"
#include "arrow/util/iterator.h"
#include "parquet/arrow/reader.h"
#include "parquet/arrow/writer.h"

std::shared_ptr<skyhook::SkyhookFileFormat> GetSkyhookFormat() {
  std::string ceph_config_path = "/etc/ceph/ceph.conf";
  std::string ceph_data_pool = "cephfs_data";
  std::string ceph_user_name = "client.admin";
  std::string ceph_cluster_name = "ceph";
  std::string ceph_cls_name = "skyhook";
  std::shared_ptr<skyhook::RadosConnCtx> rados_ctx =
      std::make_shared<skyhook::RadosConnCtx>(ceph_config_path, ceph_data_pool,
                                              ceph_user_name, ceph_cluster_name,
                                              ceph_cls_name);
  auto format = skyhook::SkyhookFileFormat::Make(rados_ctx, "parquet").ValueOrDie();
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
  auto factory = arrow::dataset::FileSystemDatasetFactory::Make(
                                         std::move(fs), s, std::move(format), options).ValueOrDie();

  arrow::dataset::InspectOptions inspect_options;
  arrow::dataset::FinishOptions finish_options;
  auto schema = factory->Inspect(inspect_options).ValueOrDie();
  auto dataset = factory->Finish(finish_options).ValueOrDie();
  return dataset;
}

std::shared_ptr<arrow::fs::FileSystem> GetFileSystemFromUri(const std::string& uri,
                                                            std::string* path) {
  auto fs = arrow::fs::FileSystemFromUri(uri, path).ValueOrDie();
  return fs;
}

std::shared_ptr<arrow::dataset::Dataset> GetDatasetFromPath(
    std::shared_ptr<arrow::fs::FileSystem> fs,
    std::shared_ptr<arrow::dataset::FileFormat> format, std::string path) {
  auto info = fs->GetFileInfo(path).ValueOrDie();
  return GetDatasetFromDirectory(std::move(fs), std::move(format), std::move(path));
}

std::shared_ptr<arrow::dataset::Scanner> GetScannerFromDataset(
    const std::shared_ptr<arrow::dataset::Dataset>& dataset,
    std::vector<std::string> columns, arrow::compute::Expression filter,
    bool use_threads) {
  auto scanner_builder = dataset->NewScan().ValueOrDie();

  if (!columns.empty()) {
    scanner_builder->Project(std::move(columns));
  }

  scanner_builder->Filter(std::move(filter));
  scanner_builder->UseThreads(use_threads);
  auto scanner = scanner_builder->Finish().ValueOrDie();
  return scanner;
}

int main() {
  std::string path;
  auto fs = GetFileSystemFromUri("file:///mnt/cephfs/dataset", &path);
  std::vector<std::string> columns;

  auto skyhook_format = GetSkyhookFormat();
  auto dataset = GetDatasetFromPath(fs, skyhook_format, path);
  auto scanner =
      GetScannerFromDataset(dataset, columns, arrow::compute::literal(true), true);
  auto table = scanner->ToTable().ValueOrDie();
  std::cout << table->ToString() << "\n";
}
