#include "oneflow/core/framework/framework.h"
#include "oneflow/customized/kernels/softmax_kernel_util.h"

namespace oneflow {
namespace user_op {

template<DeviceType device_type, typename T>
struct CrossEntropyKernelUtil {
  static void ComputeEntropy(DeviceCtx* ctx, const int64_t num_instances, const int64_t num_classes,
                             const T* x, const T* labels, T* y);
  static void ComputeDiffWithSoftmax(DeviceCtx* ctx, const int64_t elem_cnt,
                                     const int64_t num_classes, const T* prob, const T* labels,
                                     const T* dy, T* dx);
};

template<DeviceType device_type, typename T>
class SoftmaxCrossEntropyKernel final : public user_op::OpKernel {
 public:
  SoftmaxCrossEntropyKernel() = default;
  ~SoftmaxCrossEntropyKernel() = default;

 private:
  void Compute(user_op::KernelComputeContext* ctx) const override {
    const user_op::Tensor* prediction = ctx->Tensor4ArgNameAndIndex("prediction", 0);
    const user_op::Tensor* label = ctx->Tensor4ArgNameAndIndex("label", 0);
    user_op::Tensor* prob = ctx->Tensor4ArgNameAndIndex("prob", 0);
    user_op::Tensor* tmp_buffer = ctx->Tensor4ArgNameAndIndex("tmp_buffer", 0);
    user_op::Tensor* out = ctx->Tensor4ArgNameAndIndex("out", 0);
    const auto num_axes = label->shape().NumAxes();
    const int64_t num_instances = label->shape().Count(0, num_axes - 1);
    const int64_t num_classes = label->shape().At(num_axes - 1);
    SoftmaxKernelUtil<device_type, T>::ComputeProb(
        ctx->device_ctx(), num_instances, num_classes, prediction->dptr<T>(), out->mut_dptr<T>(),
        prob->mut_dptr<T>(), tmp_buffer->mut_dptr(), tmp_buffer->shape().elem_cnt() * sizeof(T));
    CrossEntropyKernelUtil<device_type, T>::ComputeEntropy(ctx->device_ctx(), num_instances,
                                                           num_classes, prob->dptr<T>(),
                                                           label->dptr<T>(), out->mut_dptr<T>());
  }
  bool AlwaysComputeWhenAllOutputsEmpty() const override { return false; }
};

#define REGISTER_SOFTMAX_CROSS_ENTROPY_KERNEL(device_type_v, dtype_pair)                     \
  REGISTER_USER_KERNEL("softmax_cross_entropy")                                              \
      .SetCreateFn<SoftmaxCrossEntropyKernel<device_type_v, OF_PP_PAIR_FIRST(dtype_pair)>>() \
      .SetIsMatchedPred([](const user_op::KernelRegContext& ctx) {                           \
        const user_op::TensorDesc* label_desc = ctx.TensorDesc4ArgNameAndIndex("label", 0);  \
        const user_op::TensorDesc* out_desc = ctx.TensorDesc4ArgNameAndIndex("out", 0);      \
        return ctx.device_type() == device_type_v                                            \
               && label_desc->data_type() == OF_PP_PAIR_SECOND(dtype_pair)                   \
               && out_desc->data_type() == OF_PP_PAIR_SECOND(dtype_pair);                    \
      })                                                                                     \
      .SetInferTmpSizeFn([](user_op::InferContext* ctx) {                                    \
        const Shape* prediction_shape = ctx->Shape4ArgNameAndIndex("prediction", 0);         \
        return prediction_shape->elem_cnt() * sizeof(OF_PP_PAIR_FIRST(dtype_pair));          \
      });

template<DeviceType device_type, typename T>
class SoftmaxCrossEntropyGradKernel final : public user_op::OpKernel {
 public:
  SoftmaxCrossEntropyGradKernel() = default;
  ~SoftmaxCrossEntropyGradKernel() = default;

 private:
  void Compute(user_op::KernelComputeContext* ctx) const override {
    const user_op::Tensor* label = ctx->Tensor4ArgNameAndIndex("label", 0);
    const user_op::Tensor* dy = ctx->Tensor4ArgNameAndIndex("dy", 0);
    const user_op::Tensor* prob = ctx->Tensor4ArgNameAndIndex("prob", 0);
    user_op::Tensor* prediction_diff = ctx->Tensor4ArgNameAndIndex("prediction_diff", 0);
    const int64_t num_instances = dy->shape().elem_cnt();
    CHECK_EQ(prob->shape().elem_cnt() % num_instances, 0);
    const int64_t num_classes = prob->shape().elem_cnt() / num_instances;

    CrossEntropyKernelUtil<device_type, T>::ComputeDiffWithSoftmax(
        ctx->device_ctx(), prediction_diff->shape().elem_cnt(), num_classes, prob->dptr<T>(),
        label->dptr<T>(), dy->dptr<T>(), prediction_diff->mut_dptr<T>());
  }
  bool AlwaysComputeWhenAllOutputsEmpty() const override { return false; }
};

#define REGISTER_SOFTMAX_CROSS_ENTROPY_GRAD_KERNEL(device_type_v, dtype_pair)                    \
  REGISTER_USER_KERNEL("softmax_cross_entropy_grad")                                             \
      .SetCreateFn<SoftmaxCrossEntropyGradKernel<device_type_v, OF_PP_PAIR_FIRST(dtype_pair)>>() \
      .SetIsMatchedPred([](const user_op::KernelRegContext& ctx) {                               \
        const user_op::TensorDesc* label_desc = ctx.TensorDesc4ArgNameAndIndex("label", 0);      \
        const user_op::TensorDesc* prediction_diff_desc =                                        \
            ctx.TensorDesc4ArgNameAndIndex("prediction_diff", 0);                                \
        return ctx.device_type() == device_type_v                                                \
               && label_desc->data_type() == OF_PP_PAIR_SECOND(dtype_pair)                       \
               && prediction_diff_desc->data_type() == OF_PP_PAIR_SECOND(dtype_pair);            \
      })                                                                                         \
      .SetInplaceProposalFn([](const user_op::InferContext&,                                     \
                               user_op::AddInplaceArgPair AddInplaceArgPairFn) -> Maybe<void> {  \
        OF_RETURN_IF_ERROR(AddInplaceArgPairFn("prediction_diff", 0, "prob", 0, true));          \
        return Maybe<void>::Ok();                                                                \
      });

}  // namespace user_op
}  // namespace oneflow