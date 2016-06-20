//===-- FirehoseTest.cpp --------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <string>
#include "gtest/gtest.h"

#include "klee/Firehose.h"

using namespace klee::firehose;

namespace {

  class XMLMagic: public XML {
  public:
    XMLMagic() {}
    const std::string toXML() const {
      std::vector<std::string> v;
      v.push_back("abra");
      v.push_back("");
      v.push_back("cadabra");
      v.push_back("");
      return mkString(v, " ");
    }
  };

  TEST(XMLMagic, mkString) {
    XMLMagic o;
    EXPECT_EQ("abra cadabra", o.toXML());
  }

  
  class PointTest: public ::testing::Test {
  protected:
    Point p1, p2;
  public:
    PointTest(): p1(1, 2), p2(10, 4) {}
  };
  
  TEST_F(PointTest, constructor) {
    EXPECT_EQ(1U, p1.getColumn());
    EXPECT_EQ(2U, p1.getLine());
  }

  TEST_F(PointTest, copyConstructor) {
    Point p(p1);
    EXPECT_EQ(p, p1);
  }
  
  TEST_F(PointTest, toXML) {
    std::string xml = p2.toXML();
    EXPECT_EQ("<point column=\"10\" line=\"4\"/>", xml);
  }

  
  class RangeTest: public ::testing::Test {
  private:
    Point p1, p2;
  protected:
    Range r;
  public:
    RangeTest(): p1(5, 6), p2(10, 12), r(p1, p2) {}
  };

  TEST_F(RangeTest, constructor) {
    EXPECT_EQ(r.getP1(), Point(5, 6));
    EXPECT_EQ(r.getP2(), Point(10, 12));
  }

  TEST_F(RangeTest, copyConstructor) {
    Range rc(r);
    EXPECT_EQ(r, rc);
  }

  TEST_F(RangeTest, toXML) {
    std::string xml = r.toXML();
    EXPECT_EQ("<range>\n" +
	      r.getP1().toXML() + "\n" +
	      r.getP2().toXML() + "\n" +
	      "</range>",
	      xml);
  }


  class FileTest: public ::testing::Test {
  protected:
    File f1, f2;
  public:
    FileTest():
      f1("lib/Support/Firehose.cpp"), f2("include/klee/Firehose.h") {}
  };

  TEST_F(FileTest, constructor) {
    EXPECT_EQ("lib/Support/Firehose.cpp", f1.getPath());
    EXPECT_EQ("include/klee/Firehose.h", f2.getPath());
  }

  TEST_F(FileTest, copyConstructor) {
    File f(f1);
    EXPECT_EQ(f, f1);
  }

  TEST_F(FileTest, toXML) {
    std::string xml = f1.toXML();
    EXPECT_EQ("<file given-path=\"lib/Support/Firehose.cpp\"/>", xml);
  }
}
