#include "oneflow/core/kernel/normalization_kernel.h"
#include "oneflow/core/device/cpu_device_context.h"
#include "oneflow/core/device/cuda_device_context.h"
#include "oneflow/core/job/job_conf.pb.h"
#include "oneflow/core/job/job_desc.h"
#include "oneflow/core/kernel/opkernel_test_common.h"
#include "oneflow/core/kernel/opkernel_test_case.h"

namespace oneflow {

namespace test {

template<DeviceType device_type, typename T>
OpKernelTestCase* NormalizationTestCase_single_number_train(bool is_train,
                                                            bool is_forward) {
  OpKernelTestCase* norm_test_case = new OpKernelTestCase();
  norm_test_case->set_is_train(is_train);
  norm_test_case->set_is_forward(is_forward);
  norm_test_case->set_device_type(device_type);
  auto* conf = norm_test_case->mut_op_conf()->mutable_normalization_conf();
  bool scale = true;
  bool center = true;
  conf->set_scale(scale);
  conf->set_center(center);

  using KTC = KTCommon<device_type, T>;
  BlobDesc* one_blob_desc =
      new BlobDesc(Shape({1}), GetDataType<T>::val, false, false, 1);
  norm_test_case->InitBlob("inputs",
                           KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {1}));
  norm_test_case->InitBlob("moving_mean",
                           KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0}));
  norm_test_case->InitBlob("moving_variance",
                           KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {1}));
  if (center)
    norm_test_case->InitBlob(
        "beta", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0}));
  if (scale)
    norm_test_case->InitBlob(
        "gamma", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {1}));
  norm_test_case->InitBlob(GenDiffBn("outputs"),
                           KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {1}));
  int64_t* piece_id = new int64_t;
  *piece_id = 1;
  norm_test_case->mut_kernel_ctx()->other = piece_id;

  if (is_train)
    norm_test_case->ForwardCheckBlob(
        "new_mean", device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {1}));
  if (is_train)
    norm_test_case->ForwardCheckBlob(
        "new_variance", device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0}));
  norm_test_case->ForwardCheckBlob(
      "inv_var", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {31.622776}));
  norm_test_case->ForwardCheckBlob(
      "outputs", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0}));
  norm_test_case->ForwardCheckBlob(
      "moving_mean", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0.01}), false);
  norm_test_case->ForwardCheckBlob(
      "moving_variance", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0.99}), false);

  norm_test_case->BackwardCheckBlob(
      GenDiffBn("inputs"), device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {31.622776}));
  if (center)
    norm_test_case->BackwardCheckBlob(
        GenDiffBn("beta"), device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {1}));
  if (scale)
    norm_test_case->BackwardCheckBlob(
        GenDiffBn("gamma"), device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0}));

  return norm_test_case;
}

template<DeviceType device_type, typename T>
OpKernelTestCase* NormalizationTestCase_single_number_predict(bool is_train,
                                                              bool is_forward) {
  OpKernelTestCase* norm_test_case = new OpKernelTestCase();
  norm_test_case->set_is_train(is_train);
  norm_test_case->set_is_forward(is_forward);
  norm_test_case->set_device_type(device_type);
  auto* conf = norm_test_case->mut_op_conf()->mutable_normalization_conf();
  bool scale = true;
  bool center = true;
  conf->set_scale(scale);
  conf->set_center(center);

  using KTC = KTCommon<device_type, T>;
  BlobDesc* one_blob_desc =
      new BlobDesc(Shape({1}), GetDataType<T>::val, false, false, 1);
  norm_test_case->InitBlob("inputs",
                           KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {1}));
  norm_test_case->InitBlob("moving_mean",
                           KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0}));
  norm_test_case->InitBlob("moving_variance",
                           KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {1}));
  if (center)
    norm_test_case->InitBlob(
        "beta", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0}));
  if (scale)
    norm_test_case->InitBlob(
        "gamma", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {1}));
  norm_test_case->InitBlob(GenDiffBn("outputs"),
                           KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {1}));
  int64_t* piece_id = new int64_t;
  *piece_id = 1;
  norm_test_case->mut_kernel_ctx()->other = piece_id;

  if (is_train)
    norm_test_case->ForwardCheckBlob(
        "new_mean", device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {1}));
  if (is_train)
    norm_test_case->ForwardCheckBlob(
        "new_variance", device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0}));
  norm_test_case->ForwardCheckBlob(
      "inv_var", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0.9995003}));
  norm_test_case->ForwardCheckBlob(
      "outputs", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0.9995003}));
  norm_test_case->ForwardCheckBlob(
      "moving_mean", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0}), false);
  norm_test_case->ForwardCheckBlob(
      "moving_variance", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {1}), false);

  norm_test_case->BackwardCheckBlob(
      GenDiffBn("inputs"), device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {31.622776}));
  if (center)
    norm_test_case->BackwardCheckBlob(
        GenDiffBn("beta"), device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {1}));
  if (scale)
    norm_test_case->BackwardCheckBlob(
        GenDiffBn("gamma"), device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0}));

  return norm_test_case;
}

template<DeviceType device_type, typename T>
OpKernelTestCase* NormalizationTestCase_first_piece_train(bool is_train,
                                                          bool is_forward) {
  OpKernelTestCase* norm_test_case = new OpKernelTestCase();
  norm_test_case->set_is_train(is_train);
  norm_test_case->set_is_forward(is_forward);
  norm_test_case->set_device_type(device_type);
  auto* conf = norm_test_case->mut_op_conf()->mutable_normalization_conf();
  bool scale = true;
  bool center = true;
  conf->set_scale(scale);
  conf->set_center(center);

  using KTC = KTCommon<device_type, T>;
  BlobDesc* blob_desc =
      new BlobDesc(Shape({1, 5}), GetDataType<T>::val, false, false, 1);
  BlobDesc* one_blob_desc =
      new BlobDesc(Shape({1}), GetDataType<T>::val, false, false, 1);
  norm_test_case->InitBlob(
      "inputs", KTC::CreateBlobWithSpecifiedVal(blob_desc, {1, 2, 3, 4, 5}));
  norm_test_case->InitBlob("moving_mean",
                           KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0}));
  norm_test_case->InitBlob("moving_variance",
                           KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {1}));
  if (center)
    norm_test_case->InitBlob(
        "beta", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0}));
  if (scale)
    norm_test_case->InitBlob(
        "gamma", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {10}));
  norm_test_case->InitBlob(
      GenDiffBn("outputs"),
      KTC::CreateBlobWithSpecifiedVal(blob_desc, {1, 2, 2, 6, 3}));
  int64_t* piece_id = new int64_t;
  *piece_id = 0;
  norm_test_case->mut_kernel_ctx()->other = piece_id;

  if (is_train)
    norm_test_case->ForwardCheckBlob(
        "new_mean", device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {3.0}));
  if (is_train)
    norm_test_case->ForwardCheckBlob(
        "new_variance", device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {2.0}));
  norm_test_case->ForwardCheckBlob(
      "inv_var", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0.706930}));
  norm_test_case->ForwardCheckBlob(
      "outputs", device_type,
      KTC::CreateBlobWithSpecifiedVal(
          blob_desc, {-14.13860, -7.06930, 0, 7.06930, 14.13860}));
  norm_test_case->ForwardCheckBlob(
      "moving_mean", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {3.0}), false);
  norm_test_case->ForwardCheckBlob(
      "moving_variance", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {2.0}), false);

  norm_test_case->BackwardCheckBlob(
      GenDiffBn("inputs"), device_type,
      KTC::CreateBlobWithSpecifiedVal(
          blob_desc, {7.06930, 14.13860, 14.13860, 42.41580, 21.20790}));
  if (center)
    norm_test_case->BackwardCheckBlob(
        GenDiffBn("beta"), device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {14}));
  if (scale)
    norm_test_case->BackwardCheckBlob(
        GenDiffBn("gamma"), device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {5.655440}));

  return norm_test_case;
}

template<DeviceType device_type, typename T>
OpKernelTestCase* NormalizationTestCase_first_piece_predict(bool is_train,
                                                            bool is_forward) {
  OpKernelTestCase* norm_test_case = new OpKernelTestCase();
  norm_test_case->set_is_train(is_train);
  norm_test_case->set_is_forward(is_forward);
  norm_test_case->set_device_type(device_type);
  auto* conf = norm_test_case->mut_op_conf()->mutable_normalization_conf();
  bool scale = true;
  bool center = true;
  conf->set_scale(scale);
  conf->set_center(center);

  using KTC = KTCommon<device_type, T>;
  BlobDesc* blob_desc =
      new BlobDesc(Shape({1, 5}), GetDataType<T>::val, false, false, 1);
  BlobDesc* one_blob_desc =
      new BlobDesc(Shape({1}), GetDataType<T>::val, false, false, 1);
  norm_test_case->InitBlob(
      "inputs", KTC::CreateBlobWithSpecifiedVal(blob_desc, {1, 2, 3, 4, 5}));
  norm_test_case->InitBlob(
      "moving_mean", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {3.0}));
  norm_test_case->InitBlob(
      "moving_variance", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {2.0}));
  if (center)
    norm_test_case->InitBlob(
        "beta", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0}));
  if (scale)
    norm_test_case->InitBlob(
        "gamma", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {10}));
  norm_test_case->InitBlob(
      GenDiffBn("outputs"),
      KTC::CreateBlobWithSpecifiedVal(blob_desc, {1, 2, 2, 6, 3}));
  int64_t* piece_id = new int64_t;
  *piece_id = 0;
  norm_test_case->mut_kernel_ctx()->other = piece_id;

  if (is_train)
    norm_test_case->ForwardCheckBlob(
        "new_mean", device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {3.0}));
  if (is_train)
    norm_test_case->ForwardCheckBlob(
        "new_variance", device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {2.0}));
  norm_test_case->ForwardCheckBlob(
      "inv_var", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0.706930}));
  norm_test_case->ForwardCheckBlob(
      "outputs", device_type,
      KTC::CreateBlobWithSpecifiedVal(
          blob_desc, {-14.13860, -7.06930, 0, 7.06930, 14.13860}));
  norm_test_case->ForwardCheckBlob(
      "moving_mean", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {3.0}), false);
  norm_test_case->ForwardCheckBlob(
      "moving_variance", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {2.0}), false);

  norm_test_case->BackwardCheckBlob(
      GenDiffBn("inputs"), device_type,
      KTC::CreateBlobWithSpecifiedVal(
          blob_desc, {7.06930, 14.13860, 14.13860, 42.41580, 21.20790}));
  if (center)
    norm_test_case->BackwardCheckBlob(
        GenDiffBn("beta"), device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {14}));
  if (scale)
    norm_test_case->BackwardCheckBlob(
        GenDiffBn("gamma"), device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {5.655440}));

  return norm_test_case;
}

template<DeviceType device_type, typename T>
OpKernelTestCase* NormalizationTestCase_second_piece_without_gamma(
    bool is_train, bool is_forward) {
  OpKernelTestCase* norm_test_case = new OpKernelTestCase();
  norm_test_case->set_is_train(is_train);
  norm_test_case->set_is_forward(is_forward);
  norm_test_case->set_device_type(device_type);
  auto* conf = norm_test_case->mut_op_conf()->mutable_normalization_conf();
  bool scale = false;
  bool center = true;
  conf->set_scale(scale);
  conf->set_center(center);

  using KTC = KTCommon<device_type, T>;
  BlobDesc* blob_desc =
      new BlobDesc(Shape({1, 5}), GetDataType<T>::val, false, false, 1);
  BlobDesc* one_blob_desc =
      new BlobDesc(Shape({1}), GetDataType<T>::val, false, false, 1);
  norm_test_case->InitBlob(
      "inputs", KTC::CreateBlobWithSpecifiedVal(blob_desc, {1, 2, 3, 4, 5}));
  norm_test_case->InitBlob(
      "moving_mean", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {3.0}));
  norm_test_case->InitBlob(
      "moving_variance", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {2.0}));
  if (center)
    norm_test_case->InitBlob(
        "beta", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {1}));
  if (scale)
    norm_test_case->InitBlob(
        "gamma", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {10}));
  norm_test_case->InitBlob(
      GenDiffBn("outputs"),
      KTC::CreateBlobWithSpecifiedVal(blob_desc, {1, 2, 2, 6, 3}));
  int64_t* piece_id = new int64_t;
  *piece_id = 1;
  norm_test_case->mut_kernel_ctx()->other = piece_id;

  if (is_train)
    norm_test_case->ForwardCheckBlob(
        "new_mean", device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {3.0}));
  if (is_train)
    norm_test_case->ForwardCheckBlob(
        "new_variance", device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {2.0}));
  norm_test_case->ForwardCheckBlob(
      "inv_var", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0.706930}));
  norm_test_case->ForwardCheckBlob(
      "outputs", device_type,
      KTC::CreateBlobWithSpecifiedVal(
          blob_desc, {1 - 1.413860, 1 - 0.706930, 1, 1.706930, 2.413860}));
  norm_test_case->ForwardCheckBlob(
      "moving_mean", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {3.0}), false);
  norm_test_case->ForwardCheckBlob(
      "moving_variance", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {2.0}), false);

  norm_test_case->BackwardCheckBlob(
      GenDiffBn("inputs"), device_type,
      KTC::CreateBlobWithSpecifiedVal(
          blob_desc, {0.706930, 1.413860, 1.413860, 4.241580, 2.120790}));
  if (center)
    norm_test_case->BackwardCheckBlob(
        GenDiffBn("beta"), device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {14}));
  if (scale)
    norm_test_case->BackwardCheckBlob(
        GenDiffBn("gamma"), device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {5.655440}));

  return norm_test_case;
}

template<DeviceType device_type, typename T>
OpKernelTestCase* NormalizationTestCase_second_piece_without_beta(
    bool is_train, bool is_forward) {
  OpKernelTestCase* norm_test_case = new OpKernelTestCase();
  norm_test_case->set_is_train(is_train);
  norm_test_case->set_is_forward(is_forward);
  norm_test_case->set_device_type(device_type);
  auto* conf = norm_test_case->mut_op_conf()->mutable_normalization_conf();
  bool scale = true;
  bool center = false;
  conf->set_scale(scale);
  conf->set_center(center);

  using KTC = KTCommon<device_type, T>;
  BlobDesc* blob_desc =
      new BlobDesc(Shape({1, 5}), GetDataType<T>::val, false, false, 1);
  BlobDesc* one_blob_desc =
      new BlobDesc(Shape({1}), GetDataType<T>::val, false, false, 1);
  norm_test_case->InitBlob(
      "inputs", KTC::CreateBlobWithSpecifiedVal(blob_desc, {1, 2, 3, 4, 5}));
  norm_test_case->InitBlob(
      "moving_mean", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {3.0}));
  norm_test_case->InitBlob(
      "moving_variance", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {2.0}));
  if (center)
    norm_test_case->InitBlob(
        "beta", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {1}));
  if (scale)
    norm_test_case->InitBlob(
        "gamma", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {10}));
  norm_test_case->InitBlob(
      GenDiffBn("outputs"),
      KTC::CreateBlobWithSpecifiedVal(blob_desc, {1, 2, 2, 6, 3}));
  int64_t* piece_id = new int64_t;
  *piece_id = 1;
  norm_test_case->mut_kernel_ctx()->other = piece_id;

  if (is_train)
    norm_test_case->ForwardCheckBlob(
        "new_mean", device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {3.0}));
  if (is_train)
    norm_test_case->ForwardCheckBlob(
        "new_variance", device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {2.0}));
  norm_test_case->ForwardCheckBlob(
      "inv_var", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0.706930}));
  norm_test_case->ForwardCheckBlob(
      "outputs", device_type,
      KTC::CreateBlobWithSpecifiedVal(
          blob_desc, {-14.13860, 7.06930, 0, 7.06930, 14.13860}));
  norm_test_case->ForwardCheckBlob(
      "moving_mean", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {3.0}), false);
  norm_test_case->ForwardCheckBlob(
      "moving_variance", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {2.0}), false);

  norm_test_case->BackwardCheckBlob(
      GenDiffBn("inputs"), device_type,
      KTC::CreateBlobWithSpecifiedVal(
          blob_desc, {7.06930, 14.13860, 14.13860, 42.41580, 21.20790}));
  if (center)
    norm_test_case->BackwardCheckBlob(
        GenDiffBn("beta"), device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {14}));
  if (scale)
    norm_test_case->BackwardCheckBlob(
        GenDiffBn("gamma"), device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {5.655440}));

  return norm_test_case;
}

template<DeviceType device_type, typename T>
OpKernelTestCase* NormalizationTestCase_second_piece_without_beta_and_gamma(
    bool is_train, bool is_forward) {
  OpKernelTestCase* norm_test_case = new OpKernelTestCase();
  norm_test_case->set_is_train(is_train);
  norm_test_case->set_is_forward(is_forward);
  norm_test_case->set_device_type(device_type);
  auto* conf = norm_test_case->mut_op_conf()->mutable_normalization_conf();
  bool scale = false;
  bool center = false;
  conf->set_scale(scale);
  conf->set_center(center);

  using KTC = KTCommon<device_type, T>;
  BlobDesc* blob_desc =
      new BlobDesc(Shape({1, 5}), GetDataType<T>::val, false, false, 1);
  BlobDesc* one_blob_desc =
      new BlobDesc(Shape({1}), GetDataType<T>::val, false, false, 1);
  norm_test_case->InitBlob(
      "inputs", KTC::CreateBlobWithSpecifiedVal(blob_desc, {1, 2, 3, 4, 5}));
  norm_test_case->InitBlob(
      "moving_mean", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {3.0}));
  norm_test_case->InitBlob(
      "moving_variance", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {2.0}));
  if (center)
    norm_test_case->InitBlob(
        "beta", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {1}));
  if (scale)
    norm_test_case->InitBlob(
        "gamma", KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {10}));
  norm_test_case->InitBlob(
      GenDiffBn("outputs"),
      KTC::CreateBlobWithSpecifiedVal(blob_desc, {1, 2, 2, 6, 3}));
  int64_t* piece_id = new int64_t;
  *piece_id = 1;
  norm_test_case->mut_kernel_ctx()->other = piece_id;

  if (is_train)
    norm_test_case->ForwardCheckBlob(
        "new_mean", device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {3.0}));
  if (is_train)
    norm_test_case->ForwardCheckBlob(
        "new_variance", device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {2.0}));
  norm_test_case->ForwardCheckBlob(
      "inv_var", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {0.706930}));
  norm_test_case->ForwardCheckBlob(
      "outputs", device_type,
      KTC::CreateBlobWithSpecifiedVal(
          blob_desc, {-1.413860, 0.706930, 0, 0.706930, 1.413860}));
  norm_test_case->ForwardCheckBlob(
      "moving_mean", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {3.0}), false);
  norm_test_case->ForwardCheckBlob(
      "moving_variance", device_type,
      KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {2.0}), false);

  norm_test_case->BackwardCheckBlob(
      GenDiffBn("inputs"), device_type,
      KTC::CreateBlobWithSpecifiedVal(
          blob_desc, {0.706930, 1.413860, 1.413860, 4.241580, 2.120790}));
  if (center)
    norm_test_case->BackwardCheckBlob(
        GenDiffBn("beta"), device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {14}));
  if (scale)
    norm_test_case->BackwardCheckBlob(
        GenDiffBn("gamma"), device_type,
        KTC::CreateBlobWithSpecifiedVal(one_blob_desc, {5.655440}));

  return norm_test_case;
}

TEST_CPU_AND_GPU_OPKERNEL(NormalizationTestCase_single_number_train,
                          OF_PP_MAKE_TUPLE_SEQ(float, DataType::kFloat),
                          (train), (forward)(backward));
TEST_CPU_AND_GPU_OPKERNEL(NormalizationTestCase_single_number_predict,
                          OF_PP_MAKE_TUPLE_SEQ(float, DataType::kFloat),
                          (predict), (forward)(backward));
TEST_CPU_AND_GPU_OPKERNEL(NormalizationTestCase_first_piece_train,
                          OF_PP_MAKE_TUPLE_SEQ(float, DataType::kFloat),
                          (train), (forward)(backward));
TEST_CPU_AND_GPU_OPKERNEL(NormalizationTestCase_first_piece_predict,
                          OF_PP_MAKE_TUPLE_SEQ(float, DataType::kFloat),
                          (predict), (forward)(backward));
TEST_CPU_AND_GPU_OPKERNEL(NormalizationTestCase_second_piece_without_gamma,
                          OF_PP_MAKE_TUPLE_SEQ(float, DataType::kFloat),
                          (train)(predict), (forward)(backward));
TEST_CPU_AND_GPU_OPKERNEL(NormalizationTestCase_second_piece_without_beta,
                          OF_PP_MAKE_TUPLE_SEQ(float, DataType::kFloat),
                          (train)(predict), (forward)(backward));
TEST_CPU_AND_GPU_OPKERNEL(
    NormalizationTestCase_second_piece_without_beta_and_gamma,
    OF_PP_MAKE_TUPLE_SEQ(float, DataType::kFloat), (train)(predict),
    (forward)(backward));

}  // namespace test

}  // namespace oneflow
