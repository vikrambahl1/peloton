//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// rule.h
//
// Identification: src/include/optimizer/optimizer_task.h
//
// Copyright (c) 2015-17, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include "optimizer/optimizer_metadata.h"
#include "optimizer/memo.h"
#include "optimizer/group_expression.h"
#include "optimizer/group.h"
#include "optimizer/rule.h"
#include "optimizer/memo.h"

namespace peloton {
namespace optimizer {

class OptimizeContext;

enum class OptimizerTaskType {
  OPTIMIZE_GROUP,
  OPTIMIZE_EXPR,
  EXPLORE_GROUP,
  EXPLORE_EXPR,
  APPLY_RULE,
  OPTIMIZE_INPUTS
};

class OptimizerTask {
 public:
  OptimizerTask(std::shared_ptr<OptimizeContext> context,
                OptimizerTaskType type)
      : type_(type), context_(context) {}

  virtual void execute() = 0;

  void PushTask(OptimizerTask* task);

  inline Memo& GetMemo() const;

  inline RuleSet& GetRuleSet() const;

 protected:
  OptimizerTaskType type_;
  std::shared_ptr<OptimizeContext> context_;
};

class OptimizeGroup : public OptimizerTask {
 public:
  OptimizeGroup(Group* group, std::shared_ptr<OptimizeContext> context)
      : OptimizerTask(context, OptimizerTaskType::OPTIMIZE_GROUP),
        group_(group) {}
  virtual void execute() override;

 private:
  Group* group_;
};

class OptimizeExpression : public OptimizerTask {
 public:
  OptimizeExpression(GroupExpression* group_expr,
                     std::shared_ptr<OptimizeContext> context)
      : OptimizerTask(context, OptimizerTaskType::OPTIMIZE_EXPR),
        group_expr_(group_expr) {}
  virtual void execute() override;

 private:
  GroupExpression* group_expr_;
};

class ExploreGroup : public OptimizerTask {
 public:
  ExploreGroup(Group* group, std::shared_ptr<OptimizeContext> context)
      : OptimizerTask(context, OptimizerTaskType::EXPLORE_GROUP),
        group_(group) {}
  virtual void execute() override;

 private:
  Group* group_;
};

class ExploreExpression : public OptimizerTask {
 public:
  ExploreExpression(GroupExpression* group_expr,
                    std::shared_ptr<OptimizeContext> context)
      : OptimizerTask(context, OptimizerTaskType::EXPLORE_EXPR),
        group_expr_(group_expr) {}
  virtual void execute() override;

 private:
  GroupExpression* group_expr_;
};

class ApplyRule : public OptimizerTask {
 public:
  ApplyRule(GroupExpression* group_expr, Rule* rule,
            std::shared_ptr<OptimizeContext> context)
      : OptimizerTask(context, OptimizerTaskType::APPLY_RULE),
        group_expr_(group_expr),
        rule_(rule) {}
  virtual void execute() override;

 private:
  GroupExpression* group_expr_;
  Rule* rule_;
  bool explore_only;
};

class OptimizeInputs : public OptimizerTask {
 public:
  OptimizeInputs(GroupExpression* group_expr,
                 std::shared_ptr<OptimizeContext> context)
      : OptimizerTask(context, OptimizerTaskType::OPTIMIZE_INPUTS),
        group_expr_(group_expr) {}

  OptimizeInputs(OptimizeInputs* task)
      : output_input_properties_(std::move(task->output_input_properties_)),
        group_expr_(task->group_expr_), cur_total_cost_(task->cur_total_cost_),
        cur_child_idx_(task->cur_child_idx_),
        cur_prop_pair_idx_(task->cur_prop_pair_idx_) {}

  virtual void execute() override;

 private:
  std::vector<std::pair<std::shared_ptr<PropertySet>,
                        std::vector<std::shared_ptr<PropertySet>>>> output_input_properties_;
  GroupExpression* group_expr_;
  double cur_total_cost_;
  int cur_child_idx_ = -1;
  int pre_child_idx_ = -1;
  int cur_prop_pair_idx_ = 0;
};

}  // namespace optimizer
}  // namespace peloton
