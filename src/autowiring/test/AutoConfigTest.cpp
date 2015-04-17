// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/AutoConfig.h>

class AutoConfigTest:
  public testing::Test
{};

struct Namespace1 {};
struct Namespace2 {};
struct XYZ {};

TEST_F(AutoConfigTest, VerifyBasicAssignment) {
  AutoConfig<int, Namespace1, XYZ> cfg1;
  *cfg1 = 1;
  ASSERT_EQ(*cfg1, 1) << "Operator = failed";

  AutoConfig<int, Namespace2, XYZ> cfg2(2);
  ASSERT_EQ(*cfg2, 2) << "Default construction failed";

  AutoConfig<int, Namespace1, XYZ> cfg1a;
  ASSERT_EQ(*cfg1a, 1) << "Failed to autowire AutoConfig value";

  *cfg1a = 3;
  ASSERT_EQ(*cfg1, 3) << "Failed to autowire AutoConfig value";

  AutoConfig<int, Namespace2, XYZ> cfg2a(5);
  ASSERT_EQ(*cfg2a, 2) << "Constructor overwrote value when it wasn't supposed to!";
}

//Ideally, this shouldn't even compile. As it stands, it should be at the very least an exception.
TEST_F(AutoConfigTest, DISABLED_VerifySingleUnderlyingType){
  AutoConfig<int, Namespace1, XYZ> cfg1;
  bool threw = false;
  try {
    AutoConfig<float, Namespace1, XYZ> cfg2;
  }
  catch(...){
    threw = true;
  }
  ASSERT_TRUE(threw) << "Constructing an AutoConfig with the same name and a different type failed to cause an exception";
}

TEST_F(AutoConfigTest, VerifyBasicSignals) {
  AutoCurrentContext ctxt;

  int handler_called = 0;
  int handler_value_read = 0;
  Autowired<AutoConfigVar<int, Namespace1, XYZ>> autoCfg;
  autoCfg(&AutoConfigVarBase::onChangedSignal) += [&](const AutoConfigVarBase& var) {
    ++handler_called;
    var.Get(&handler_value_read);
  };

  AutoConfig<int, Namespace1, XYZ> cfg1(1);
  ASSERT_EQ(handler_called, 1) << "OnChanged not triggered by construction";
  ASSERT_EQ(handler_value_read, 1) << "Bad value read in OnChanged";

  *cfg1 = 20;
  ASSERT_EQ(handler_called, 2) << "OnChanged not triggered by assignement";
  ASSERT_EQ(handler_value_read, 20) << "Bad value read in OnChanged";

  AutoConfig<int, Namespace1, XYZ> cfg2;
  *cfg2 = 2;

  ASSERT_EQ(handler_called, 3) << "OnChanged not triggred by assignement from alternate autowired instance";
  ASSERT_EQ(handler_value_read, 2) << "Bad value read in OnChanged";

  int handler_direct_called = 0;
  int handler_direct_value = 0;
  auto* registration = *cfg2 += [&](const int& var) {
    ++handler_direct_called;
    handler_direct_value = var;
  };

  *cfg1 = 30;
  ASSERT_EQ(handler_direct_called, 1) << "OnChanged not triggred by indirect +=";
  ASSERT_EQ(handler_direct_value, 30) << "Bad value read in OnChanged";

  *cfg1 -= registration;
  
  *cfg1 = 123;
  ASSERT_EQ(handler_direct_called, 1) << "OnChanged not unsubscribed by indirect -=";
  ASSERT_EQ(handler_direct_value, 30) << "Bad value read in OnChanged";
}

TEST_F(AutoConfigTest, VerifyBasicInheritance) {
  AutoConfig<int, Namespace1, XYZ> cfg_outer;
  *cfg_outer = 1001;

  {
    AutoCreateContext ctxt_middle;
    CurrentContextPusher pshr(ctxt_middle);
    AutoConfig<int, Namespace1, XYZ> cfg_middle;
    ASSERT_EQ(*cfg_middle, 1001) << "Configurable value obtained from parent context did not have the correct value";

    {
      AutoCreateContext ctxt_inner;
      CurrentContextPusher pshr(ctxt_inner);

      // Now inject an element here, and verify that it was wired up as expected:
      AutoConfig<int, Namespace1, XYZ> cfg_inner;
      ASSERT_EQ(1001, *cfg_inner) << "Configurable value obtained from a parent context did not have the correct value";

      // Now change the parent value....
      *cfg_outer = 1002;
      ASSERT_EQ(*cfg_inner, 1002) << "Configuration value did not update when value in a parent was updated";

      // Now change the value in the intervening context...
      *cfg_middle = 1003;
      ASSERT_EQ(*cfg_inner, 1003) << "Configuration value did not update when value in a parent was updated";

      // This must work as expected--a local context override will rewrite configuration values in the local scope
      *cfg_inner = 1004;
      ASSERT_EQ(*cfg_inner, 1004) << "Override of a configurable value in a derived class did not take place as expected";
    }
    ASSERT_EQ(*cfg_middle, 1003) << "A configuration value was not updated when accessed directly.";
  }

  //Modification of the value in the enclosed context should not effect us.
  ASSERT_EQ(1002, *cfg_outer) << "Override of a configurable value in a child scope affected parent";
}

struct Unparseable {
  int v;
};

TEST_F(AutoConfigTest, VerifyParsedAssignment) {
  // We must also be able to support implicit string-to-type conversion via the shift operator for this type
  AutoConfig<int, Namespace1, XYZ> cfg;

  // Direct assignment to a string should not work, the type isn't a string it's an int
  ASSERT_ANY_THROW(cfg->SetParsed("badvalue")) << "An attempt to assign a value to an unparsable string did not throw an exception";
  ASSERT_FALSE(cfg->IsConfigured()) << "A failed parse incorrectly marked the value as configured";

  // Assignment to a string type should result in an appropriate coercion to the right value
  cfg->SetParsed("324");
  ASSERT_EQ(*cfg, 324);
  ASSERT_TRUE(cfg->IsConfigured()) << "Assignement from a parsed value falied to mark the value as configured";

  AutoConfig<Unparseable, Namespace2, Unparseable> noparse;
  ASSERT_ANY_THROW(cfg->SetParsed("anyvalue")) << "An attempt to parse a value into an unparsable structure did not throw an exception";
  *noparse = Unparseable{ 20 };
  ASSERT_EQ((*noparse)->v, 20);
}

struct MyConfigurableClass {
  AutoConfig<int, Namespace1, XYZ> cfg;
};

TEST_F(AutoConfigTest, NestedContexts) {
  // Set up contexts and members
  AutoCurrentContext ctxt_outer;
  std::shared_ptr<CoreContext> ctxt_middle = ctxt_outer->Create<void>();
  std::shared_ptr<CoreContext> ctxt_sibling = ctxt_outer->Create<void>();
  std::shared_ptr<CoreContext> ctxt_inner = ctxt_middle->Create<void>();
  std::shared_ptr<CoreContext> ctxt_no_manager = ctxt_inner->Create<void>();
  std::shared_ptr<CoreContext> ctxt_leaf = ctxt_no_manager->Create<void>();
  
  AutoRequired<MyConfigurableClass> mcc_outer(ctxt_outer);
  AutoRequired<MyConfigurableClass> mcc_middle(ctxt_middle);
  AutoRequired<MyConfigurableClass> mcc_sibling(ctxt_sibling);
  AutoRequired<MyConfigurableClass> mcc_inner(ctxt_inner);
  AutoRequired<MyConfigurableClass> mcc_leaf(ctxt_leaf);
  
  // Set initial value
  *mcc_outer->cfg = 42;
  ASSERT_EQ(42, *mcc_outer->cfg) << "Config value not set";
  ASSERT_EQ(42, *mcc_middle->cfg) << "Config value not set in descendant context";
  ASSERT_EQ(42, *mcc_sibling->cfg) << "Config value not set in descendant context";
  ASSERT_EQ(42, *mcc_inner->cfg) << "Config value not set in descendant context";
  ASSERT_EQ(42, *mcc_leaf->cfg) << "Config value not set in desendant context";
  EXPECT_TRUE(mcc_middle->cfg->IsInherited()) << "Inherited key not marked as such";
  EXPECT_TRUE(mcc_leaf->cfg->IsInherited()) << "Inherited key not marked as such";
  
  // Set middle, inner shouldn't be able to be set from outer after this
  bool callback_hit1 = false;
  *mcc_inner->cfg += [&callback_hit1](int) {
    callback_hit1 = true;
  };
  *mcc_middle->cfg = 1337;
  ASSERT_EQ(42, *mcc_outer->cfg) << "Config value changed in outer context";
  ASSERT_EQ(42, *mcc_sibling->cfg) << "Config value set from sibling context";
  ASSERT_EQ(1337, *mcc_middle->cfg) << "Config value not set";
  ASSERT_EQ(1337, *mcc_inner->cfg) << "Config value not set in child context";
  ASSERT_EQ(1337, *mcc_leaf->cfg) << "Config value not set in leaf context";
  ASSERT_TRUE(callback_hit1) << "Callback not hit in inner context";
  
  // Set from outter, inner should be shielded by middle context
  *mcc_inner->cfg += [](int) {
    FAIL() << "This callback should never be hit";
  };
  
  // Make sure sibling context is not shielded
  bool callback_hit2 = false;
  *mcc_sibling->cfg += [&callback_hit2](int) {
    callback_hit2 = true;
  };
  
  // Set from outer, shouldn't effect middle or inner contexts
  *mcc_outer->cfg = 999;
  ASSERT_EQ(999, *mcc_outer->cfg) << "Config value not set";
  ASSERT_EQ(1337, *mcc_middle->cfg) << "Config value overwritten when value was set in this context";
  ASSERT_EQ(1337, *mcc_inner->cfg) << "Config value overwritten when value was set in parent context";
  
  // Make sure sibling hit
  ASSERT_EQ(999, *mcc_sibling->cfg) << "Value not set on sibling of context where value was previously set";
  ASSERT_TRUE(callback_hit2) << "Callback not called on sibling of context where value was previously set";
}

struct ValidatedKey{
  static bool Validate(const int& value) {
    return value > 5;
  }
};

struct MyValidatedClass{
  AutoConfig<int, ValidatedKey> m_config;
};

TEST_F(AutoConfigTest, Validators) {
  AutoRequired<MyValidatedClass> valid;
  
  *valid->m_config = 42;
  ASSERT_EQ(42, *valid->m_config) << "Value not set for key";
  
  ASSERT_ANY_THROW(*valid->m_config = 1) << "AutoConfig validator didn't reject invalid value";
  ASSERT_EQ(42, *valid->m_config) << "Value not set for key";
  
  *valid->m_config = 1337;
  ASSERT_EQ(1337, *valid->m_config) << "Value not set for key";
}

struct ComplexValue {
  int a;
  int b;
  int c;

  ComplexValue(int nA, int nB, int nC) : a(nA), b(nB), c(nC) {}
  ComplexValue(int repeated) : a(repeated), b(repeated), c(repeated) {}
  ComplexValue() {}
};

struct MyComplexValueClass {
  AutoConfig<ComplexValue, struct Namespace1, struct MyCxValue> m_cfg;
  AutoConfig<ComplexValue, struct Namespace1, struct MyCxValue2> m_cfg2 = ComplexValue{ 10, 15, 30 };

  MyComplexValueClass() : m_cfg(ComplexValue{ 2, 20, 20 }) {}
};

TEST_F(AutoConfigTest, ComplexConstruction){
  //Default constuction is not allowed if the underlying type does not support it
  AutoConfig<ComplexValue, Namespace1, MyCxValue> defaultConstructed;
  ASSERT_FALSE(defaultConstructed->IsConfigured()) << "non-initalizing constructor should not configure a value!";

  AutoConfig<ComplexValue, Namespace1, MyCxValue> fancyConstructed(1, 2, 3);
  
  ASSERT_TRUE(fancyConstructed->IsConfigured()) << "Initializing constructor did not set config value";
  ASSERT_EQ((*fancyConstructed)->a, 1) << "Initializing constructor did not set config value";
  ASSERT_EQ((*fancyConstructed)->b, 2) << "Initializing constructor did not set config value";
  ASSERT_EQ((*fancyConstructed)->c, 3) << "Initializing constructor did not set config value";

  AutoRequired<MyComplexValueClass> complex;
  ASSERT_EQ((*complex->m_cfg)->a, 1) << "Second Initalizing constructor overrode the first!";
  ASSERT_EQ((*complex->m_cfg)->b, 2) << "Second Initalizing constructor overrode the first!";
  ASSERT_EQ((*complex->m_cfg)->c, 3) << "Second Initalizing constructor overrode the first!";

  ASSERT_EQ((*complex->m_cfg2)->a, 10) << "Initializing constructor was not called";
  ASSERT_EQ((*complex->m_cfg2)->b, 15) << "Initializing constructor was not called";
  ASSERT_EQ((*complex->m_cfg2)->c, 30) << "Initializing constructor was not called";
}