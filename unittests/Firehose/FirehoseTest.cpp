/* -*- mode: c++; c-basic-offset: 2; -*- */

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

  Point p1(1, 2);
  Point p2(10, 4);

  Range r(Point(5, 6), Point(10, 12));
  
  File f1("lib/Support/Firehose.cpp");
  File f2("include/klee/Firehose.h");

  Function fun1("function1");
  Function fun2("another_function");

  Location loc1(File("a/b/c"), Function("f1"), Range(Point(120, 0), Point(150, 0)));
  Location loc2(File("t.c"), Function("error"), Point(42, 3));
  Location loc3(File("Test.c"), Function("Test1"));

  Message msg1("Out of memory");
  Message msg2(std::string("Invalid pointer"));

  Notes notes1("Function call: f(a=3, b=7)");
  Notes notes2(std::string("Function call: ") +
	       loc3.getFunction().getName() +
	       std::string("(name=22974400)"));

  State state1(loc1);
  State state2(loc2);
  State state3(loc3, notes2);

  // The ugliness of C++ < 11 - No easy way to construct a complex
  // persistent data structure
  const std::vector<State> constructTrace3Vec() {
    std::vector<State> v;
    v.push_back(state1);
    v.push_back(state2);
    v.push_back(state3);
    return v;
  }

  std::vector<State> trace1Vec;
  std::vector<State> trace2Vec(1, state1);
  std::vector<State> trace3Vec(constructTrace3Vec());
  Trace trace1(trace1Vec);
  Trace trace2(trace2Vec);
  Trace trace3(trace3Vec);
  // if KLEE was compiled with the -std=c++11 option, we could
  // construct trace trace3 this way instead:
  // Trace trace3(std::vector<State>{state1, state2, state3});

  Issue issue1(msg1, loc1);
  Issue issue2(msg1, loc1, trace1);
  Issue issue3(msg2, loc2, trace3);

  Message failure1Msg(std::string("unable to load symbol(") +
  		      std::string("_ZN4dcpp4Text13systemCharsetE) while ") +
  		      std::string("initializing globals."));
  Message failure2Msg("failed external call: ajStrNew");
  Location failure2Loc(File(std::string("/tmp/buildd/embassy-domsearch-") +
  			    std::string("0.1.650/src/seqfraggle.c")),
  		       dummyFunction(),
  		       Point(0, 119));
  Failure failure1(std::string("symbol-loading"), failure1Msg, dummyLocation());
  Failure failure2(std::string("external-call"), failure2Msg, failure2Loc);

  Message info1Msg("WARNING: undefined reference to function: _ZN4QUrlD1Ev");
  Message info2Msg("WARNING ONCE: function \"socket\" has inline asm");
  Message info3Msg("calling external: ev_default_loop(0)");
  Message info4Msg("undefined reference to variable: acs_map");
  Message info5Msg("calling __user_main with extra arguments.");
  Message info6Msg("Large alloc: 13113808 bytes.  KLEE may run out of memory.");
  Message info7Msg("__syscall_rt_sigaction: silently ignoring");
  Message info8Msg("execve: ignoring (EACCES)");
  Message info9Msg("executable has module level assembly (ignoring)");
  Message info10Msg("unable to write output test case, losing it");
  Info info1(std::string("undefined-function-reference"), info1Msg);
  Info info2(std::string("inline-asm"), info2Msg);
  Info info3(std::string("calling-external"), info3Msg);
  Info info4(std::string("undefined-variable-reference"), info4Msg);
  Info info5(std::string("calling-user-main"), info5Msg);
  Info info6(std::string("large-alloc"), info6Msg);
  Info info7(std::string("silently-ignoring"), info7Msg);
  Info info8(std::string("execve"), info8Msg);
  Info info9(std::string("module-level-assembly"), info9Msg);
  Info info10(std::string("other-info"), info10Msg);
  
  // The ugliness of C++ < 11 - No easy way to construct a complex
  // persistent data structure
  const std::vector<Issue> constructResults3Vec() {
    std::vector<Issue> v;
    v.push_back(issue1);
    v.push_back(issue2);
    v.push_back(issue3);
    return v;
  }

  std::vector<Issue> results1Vec;
  std::vector<Issue> results2Vec(1, issue1);
  std::vector<Issue> results3Vec(constructResults3Vec());
  Results results1(results1Vec);
  Results results2(results2Vec);
  Results results3(results3Vec);
  
  Generator gen1("klee", "1.2.0");
  Generator gen2("clanganalyzer", "n/a");

  Metadata metadata1(gen1);
  Metadata metadata2(gen2);

  Analysis analysis1(metadata1, results1);
  Analysis analysis2(metadata1, results2);
  Analysis analysis3(metadata2, results3);
  
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


  // Point
  TEST(PointTest, constructor) {
    EXPECT_EQ(1U, p1.getColumn());
    EXPECT_EQ(2U, p1.getLine());
    // We could have used EXPECT_NE(p1, p2) here if the != operator
    // was defined on Point. Nevertheless, all tests have to pass so
    // we make no distinction between fatal and non-fatal assertions.
    ASSERT_FALSE(p1 == p2);
  }

  TEST(PointTest, copyConstructor1) {
    Point a(p1);
    EXPECT_EQ(a, p1);
  }

  TEST(PointTest, copyConstructor2) {
    Point b(p2);
    EXPECT_EQ(b, p2);
  }

  TEST(PointTest, copyConstructor3) {
    Point d(dummyPoint());
    EXPECT_EQ(d, dummyPoint());
  }

  TEST(PointTest, toXML) {
    std::string xml = p2.toXML();
    EXPECT_EQ("<point column=\"10\" line=\"4\"/>", xml);
  }

  TEST(PointTestDummy, toXML) {
    std::string xml = dummyPoint().toXML();
    EXPECT_EQ("", xml);
  }

  // Range
  TEST(RangeTest, constructor) {
    EXPECT_EQ(r.getP1(), Point(5, 6));
    EXPECT_EQ(r.getP2(), Point(10, 12));
    ASSERT_FALSE(r == dummyRange());
  }

  TEST(RangeTest, copyConstructor1) {
    Range rc(r);
    EXPECT_EQ(r, rc);
  }

  TEST(RangeTest, copyConstructor2) {
    Range dr(dummyRange());
    EXPECT_EQ(dr, dummyRange());
  }

  TEST(RangeTest, toXML) {
    std::string xml = r.toXML();
    EXPECT_EQ("<range>\n" +
	      r.getP1().toXML() + "\n" +
	      r.getP2().toXML() + "\n" +
	      "</range>",
	      xml);
  }

  TEST(RangeTestDummy, toXML) {
    std::string xml = dummyRange().toXML();
    EXPECT_EQ("", xml);
  }


  // File
  TEST(FileTest, constructor) {
    EXPECT_EQ("lib/Support/Firehose.cpp", f1.getPath());
    EXPECT_EQ("include/klee/Firehose.h", f2.getPath());
    ASSERT_FALSE(f1 == f2);
  }

  TEST(FileTest, copyConstructor1) {
    File a(f1);
    EXPECT_EQ(a, f1);
  }

  TEST(FileTest, copyConstructor2) {
    File b(f2);
    EXPECT_EQ(b, f2);
  }

  TEST(FileTest, copyConstructor3) {
    File df(dummyFile());
    EXPECT_EQ(df, dummyFile());
  }

  TEST(FileTest, toXML) {
    std::string xml = f1.toXML();
    EXPECT_EQ("<file given-path=\"lib/Support/Firehose.cpp\"/>", xml);
  }

  TEST(FileTestDummy, toXML) {
    std::string xml = dummyFile().toXML();
    EXPECT_EQ("", xml);
  }

  
  // Function
  TEST(FunctionTest, constructor) {
    EXPECT_EQ("function1", fun1.getName());
    EXPECT_EQ("another_function", fun2.getName());
    
    std::string s("fun_name");
    Function fun(s);
    EXPECT_EQ("fun_name", fun.getName());

    ASSERT_FALSE(fun1 == fun2);
  }

  TEST(FunctionTest, copyConstructor1) {
    Function a(fun1);
    EXPECT_EQ(a, fun1);
  }

  TEST(FunctionTest, copyConstructor2) {
    Function b(fun2);
    EXPECT_EQ(b, fun2);
  }

  TEST(FunctionTest, copyConstructor3) {
    Function df(dummyFunction());
    EXPECT_EQ(df, dummyFunction());
  }

  TEST(FunctionTest, toXML) {
    std::string xml = fun2.toXML();
    EXPECT_EQ("<function name=\"another_function\"/>", xml);
  }

  TEST(FunctionTestDummy, toXML) {
    std::string xml = dummyFunction().toXML();
    EXPECT_EQ("", xml);
  }


  // Location
  TEST(LocationTest, constructor1) {
    EXPECT_EQ(File("a/b/c"), loc1.getFile());
    EXPECT_EQ(Function("f1"), loc1.getFunction());
    EXPECT_EQ(Range(Point(120, 0), Point(150, 0)), loc1.getRange());
    EXPECT_EQ(dummyPoint(), loc1.getPoint());
    ASSERT_FALSE(loc1 == loc2);
  }

  TEST(LocationTest, constructor2) {
    EXPECT_EQ(File("t.c"), loc2.getFile());
    EXPECT_EQ(Function("error"), loc2.getFunction());
    EXPECT_EQ(dummyRange(), loc2.getRange());
    EXPECT_EQ(Point(42, 3), loc2.getPoint());
    ASSERT_FALSE(loc3 == loc2);
  }

  TEST(LocationTest, constructor3) {
    EXPECT_EQ(File("Test.c"), loc3.getFile());
    EXPECT_EQ(Function("Test1"), loc3.getFunction());
    EXPECT_EQ(dummyRange(), loc3.getRange());
    EXPECT_EQ(dummyPoint(), loc3.getPoint());
    ASSERT_FALSE(loc1 == loc3);
  }
  
  TEST(LocationTest, copyConstructor1) {
    Location a(loc1);
    EXPECT_EQ(a, loc1);
  }

  TEST(LocationTest, copyConstructor2) {
    Location b(loc2);
    EXPECT_EQ(b, loc2);
  }

  TEST(LocationTest, copyConstructor3) {
    Location c(loc3);
    EXPECT_EQ(c, loc3);
  }

  TEST(LocationTest, copyConstructor4) {
    Location dl(dummyLocation());
    EXPECT_EQ(dl, dummyLocation());
  }

  TEST(LocationTest, toXML1) {
    std::string xml = loc1.toXML();
    EXPECT_EQ("<location>\n" +
	      loc1.getFile().toXML() + "\n" +
	      loc1.getFunction().toXML() + "\n" +
	      loc1.getRange().toXML() + "\n" +
	      "</location>",
	      xml);
  }

  TEST(LocationTest, toXML2) {
    std::string xml = loc2.toXML();
    EXPECT_EQ("<location>\n" +
	      loc2.getFile().toXML() + "\n" +
	      loc2.getFunction().toXML() + "\n" +
	      loc2.getPoint().toXML() + "\n" +
	      "</location>",
	      xml);
  }

  TEST(LocationTest, toXML3) {
    std::string xml = loc3.toXML();
    EXPECT_EQ("<location>\n" +
	      loc3.getFile().toXML() + "\n" +
	      loc3.getFunction().toXML() + "\n" +
	      "</location>",
	      xml);
  }

  TEST(LocationTestDummy, toXML) {
    std::string xml = dummyLocation().toXML();
    EXPECT_EQ("", xml);
  }


  // Message
  TEST(MessageTest, constructor) {
    EXPECT_EQ("Out of memory", msg1.get());
    EXPECT_EQ("Invalid pointer", msg2.get());
  }

  TEST(MessageTest, copyConstructor1) {
    Message a(msg1);
    EXPECT_EQ(a, msg1);
  }

  TEST(MessageTest, copyConstructor2) {
    Message b(msg2);
    EXPECT_EQ(b, msg2);
  }

  TEST(MessageTest, copyConstructor3) {
    Message dm(dummyMessage());
    EXPECT_EQ(dm, dummyMessage());
  }

  const std::string messageToXML(const Message& msg) {
    return std::string("<message>" + msg.get() + "</message>");
  }
  
  TEST(MessageTest, toXML1) {
    std::string xml1 = msg1.toXML();
    std::string xml2 = msg2.toXML();
    EXPECT_EQ(xml1, messageToXML(msg1));
    EXPECT_EQ(xml2, messageToXML(msg2));
  }

  TEST(MessageTestDummy, toXML) {
    EXPECT_EQ("", dummyMessage().toXML());
  }

  
  // Notes
  TEST(NotesTest, constructor) {
    EXPECT_EQ("Function call: f(a=3, b=7)", notes1.get());
    EXPECT_EQ(std::string("Function call: ") +
	      loc3.getFunction().getName() +
	      std::string("(name=22974400)"),
	      notes2.get());
  }

  TEST(NotesTest, copyConstructor1) {
    Notes a(notes1);
    EXPECT_EQ(a, notes1);
  }

  TEST(NotesTest, copyConstructor2) {
    Notes b(notes2);
    EXPECT_EQ(b, notes2);
  }

  TEST(NotesTest, copyConstructor3) {
    Notes dm(dummyNotes());
    EXPECT_EQ(dm, dummyNotes());
  }

  const std::string notesToXML(const Notes& notes) {
    return std::string("<notes>" + notes.get() + "</notes>");
  }
  
  TEST(NotesTest, toXML1) {
    std::string xml1 = notes1.toXML();
    std::string xml2 = notes2.toXML();
    EXPECT_EQ(xml1, notesToXML(notes1));
    EXPECT_EQ(xml2, notesToXML(notes2));
  }

  TEST(NotesTestDummy, toXML) {
    EXPECT_EQ("", dummyNotes().toXML());
  }

  
  // State
  TEST(StateTest, constructor1) {
    EXPECT_EQ(loc1, state1.getLocation());
    EXPECT_EQ(dummyNotes(), state1.getNotes());
    ASSERT_FALSE(state1 == state2);
  }

  TEST(StateTest, constructor2) {
    EXPECT_EQ(loc2, state2.getLocation());
    EXPECT_EQ(dummyNotes(), state2.getNotes());
    ASSERT_FALSE(state3 == state2);
  }

  TEST(StateTest, constructor3) {
    EXPECT_EQ(loc3, state3.getLocation());
    EXPECT_EQ(notes2, state3.getNotes());
    ASSERT_FALSE(state1 == state3);
  }

  TEST(StateTest, copyConstructor1) {
    State a(state1);
    EXPECT_EQ(a, state1);
  }

  TEST(StateTest, copyConstructor2) {
    State b(state2);
    EXPECT_EQ(b, state2);
  }

  TEST(StateTest, copyConstructor3) {
    State c(state3);
    EXPECT_EQ(c, state3);
  }

  TEST(StateTest, copyConstructor4) {
    State ds(dummyState());
    EXPECT_EQ(ds, dummyState());
  }

  const std::string stateToXML(const State& s) {
    std::ostringstream os;

    os << "<state>\n" << s.getLocation().toXML() + "\n";
    if (!(s.getNotes() == dummyNotes()))
      os << s.getNotes().toXML() + "\n";
    os << "</state>";

    return os.str();
  }
  
  TEST(StateTest, toXML) {
    std::string xml1 = state1.toXML();
    std::string xml2 = state2.toXML();
    std::string xml3 = state3.toXML();
    EXPECT_EQ(xml1, stateToXML(state1));
    EXPECT_EQ(xml2, stateToXML(state2));
    EXPECT_EQ(xml3, stateToXML(state3));
  }

  TEST(StateTestDummy, toXML) {
    std::string xml = dummyState().toXML();
    EXPECT_EQ("", xml);
  }


  // Trace
  TEST(TraceTest, constructor1) {
    EXPECT_EQ(trace1Vec, trace1.getStates());
    ASSERT_FALSE(trace1 == trace2);
    ASSERT_TRUE(trace1 == trace1);
  }

  TEST(TraceTest, constructor2) {
    EXPECT_EQ(trace2Vec, trace2.getStates());
    ASSERT_FALSE(trace3 == trace2);
  }

  TEST(TraceTest, constructor3) {
    EXPECT_EQ(trace3Vec, trace3.getStates());
    ASSERT_FALSE(trace1 == trace3);
  }

  TEST(TraceTest, copyConstructor1) {
    Trace a(trace1);
    EXPECT_EQ(a, trace1);
  }

  TEST(TraceTest, copyConstructor2) {
    Trace b(trace2);
    EXPECT_EQ(b, trace2);
  }

  TEST(TraceTest, copyConstructor3) {
    Trace c(trace3);
    EXPECT_EQ(c, trace3);
  }

  TEST(TraceTest, copyConstructor4) {
    Trace dt(dummyTrace());
    EXPECT_EQ(dt, dummyTrace());
  }

  const std::string traceToXML(const Trace& t) {
    std::ostringstream os;
    std::vector<State> v = t.getStates();

    os << "<trace>\n";
    for (std::vector<State>::iterator iter = v.begin();
	 iter != v.end();
	 ++iter)
      os << iter->toXML() << "\n";
    os << "</trace>";
    return os.str();
  }
  
  TEST(TraceTest, toXML) {
    std::string xml1("<trace>\n</trace>");
    std::string xml2 = trace2.toXML();
    std::string xml3 = trace3.toXML();
    ASSERT_EQ(xml1, traceToXML(trace1));
    ASSERT_EQ(xml2, traceToXML(trace2));
    ASSERT_EQ(xml3, traceToXML(trace3));
  }

  TEST(TraceTestDummy, states) {
    std::vector<State> vStates(dummyTrace().getStates());
    EXPECT_EQ(1, vStates.size());
    EXPECT_EQ(dummyState(), vStates[0]);
  }

  TEST(TraceTestDummy, identity) {
    ASSERT_TRUE(dummyTrace() == dummyTrace());
  }
  
  TEST(TraceTestDummy, toXML) {
    ASSERT_EQ("", dummyTrace().toXML());
  }


  // Issue
  TEST(IssueTest, constructor1) {
    EXPECT_EQ(msg1, issue1.getMessage());
    EXPECT_EQ(loc1, issue1.getLocation());
    EXPECT_EQ(dummyTrace(), issue1.getTrace());
  }

  TEST(IssueTest, constructor2) {
    EXPECT_EQ(msg1, issue2.getMessage());
    EXPECT_EQ(loc1, issue2.getLocation());
    EXPECT_EQ(trace1, issue2.getTrace());
  }

  TEST(IssueTest, constructor3) {
    EXPECT_EQ(msg2, issue3.getMessage());
    EXPECT_EQ(loc2, issue3.getLocation());
    EXPECT_EQ(trace3, issue3.getTrace());
  }

  TEST(IssueTest, copyConstructor1) {
    Issue a(issue1);
    EXPECT_EQ(a, issue1);
  }

  TEST(IssueTest, copyConstructor2) {
    Issue b(issue2);
    EXPECT_EQ(b, issue2);
  }

  TEST(IssueTest, copyConstructor3) {
    Issue c(issue3);
    EXPECT_EQ(c, issue3);
  }

  TEST(IssueTest, copyConstructor4) {
    Issue di(dummyIssue());
    EXPECT_EQ(di, dummyIssue());
  }

  const std::string issueToXML(const Issue& issue) {
    std::ostringstream os;
    os << "<issue>\n";
    os << issue.getMessage().toXML() + "\n";
    os << issue.getLocation().toXML() + "\n";
    if (!(issue.getTrace() == dummyTrace()))
      os << issue.getTrace().toXML() + "\n";
    os << "</issue>";

    return os.str();
  }

  TEST(IssueTest, toXML) {
    std::string xml1 = issue1.toXML();
    std::string xml2 = issue2.toXML();
    std::string xml3 = issue3.toXML();
    EXPECT_EQ(xml1, issueToXML(issue1));
    EXPECT_EQ(xml2, issueToXML(issue2));
    EXPECT_EQ(xml3, issueToXML(issue3));
  }

  TEST(IssueTestDummy, toXML) {
    EXPECT_EQ("", dummyIssue().toXML());
  }


  // Failure
  TEST(FailureTest, constructor1) {
    EXPECT_EQ(std::string("symbol-loading"), failure1.getId());
    EXPECT_EQ(failure1Msg, failure1.getMessage());
    EXPECT_EQ(dummyLocation(), failure1.getLocation());
    ASSERT_FALSE(failure1 == failure2);
  }

  TEST(FailureTest, constructor2) {
    EXPECT_EQ(std::string("external-call"), failure2.getId());
    EXPECT_EQ(failure2Msg, failure2.getMessage());
    EXPECT_EQ(failure2Loc, failure2.getLocation());
  }

  TEST(FailureTest, copyConstructor1) {
    Failure a(failure1);
    EXPECT_EQ(a, failure1);
  }
  
  TEST(FailureTest, copyConstructor2) {
    Failure b(failure2);
    EXPECT_EQ(b, failure2);
  }
  
  TEST(FailureTest, copyConstructor3) {
    Failure df(dummyFailure());
    EXPECT_EQ(df, dummyFailure());
  }
  
  const std::string failureToXML(const Failure& failure) {
    std::stringstream ss;

    ss << "<failure failure-id=\"" + failure.getId() + "\">\n";
    if (!(failure.getLocation() == dummyLocation()))
      ss << failure.getLocation().toXML() + "\n";
    ss << failure.getMessage().toXML() + "\n";
    ss << "</failure>";

    return ss.str();
  }

  TEST(FailureTest, toXML) {
    std::string xml1 = failure1.toXML();
    std::string xml2 = failure2.toXML();
    EXPECT_EQ(xml1, failureToXML(failure1));
    EXPECT_EQ(xml2, failureToXML(failure2));
  }

  TEST(FailureTestDummy, toXML) {
    EXPECT_EQ("", dummyFailure().toXML());
  }
  

  // Info
  TEST(InfoTest, constructor) {
    EXPECT_EQ(info1Msg, info1.getMessage());
    EXPECT_EQ(info2Msg, info2.getMessage());
    EXPECT_EQ(info3Msg, info3.getMessage());
    EXPECT_EQ(info4Msg, info4.getMessage());
    EXPECT_EQ(info5Msg, info5.getMessage());
    EXPECT_EQ(info6Msg, info6.getMessage());
    EXPECT_EQ(info7Msg, info7.getMessage());
    EXPECT_EQ(info8Msg, info8.getMessage());
    EXPECT_EQ(info9Msg, info9.getMessage());
    EXPECT_EQ(info10Msg, info10.getMessage());
    EXPECT_EQ(std::string("undefined-function-reference"), info1.getId());
    EXPECT_EQ(std::string("inline-asm"), info2.getId());
    EXPECT_EQ(std::string("calling-external"), info3.getId());
    EXPECT_EQ(std::string("undefined-variable-reference"), info4.getId());
    EXPECT_EQ(std::string("calling-user-main"), info5.getId());
    EXPECT_EQ(std::string("large-alloc"), info6.getId());
    EXPECT_EQ(std::string("silently-ignoring"), info7.getId());
    EXPECT_EQ(std::string("execve"), info8.getId());
    EXPECT_EQ(std::string("module-level-assembly"), info9.getId());
    EXPECT_EQ(std::string("other-info"), info10.getId());
  }

  TEST(InfoTest, copyConstructor) {
    Info a(info1);
    Info b(info2);
    Info c(info8);
    Info d(dummyInfo());
    EXPECT_EQ(a, info1);
    EXPECT_EQ(b, info2);
    EXPECT_EQ(c, info8);
    EXPECT_EQ(d, dummyInfo());
  }
  

  // Results
  TEST(ResultsTest, constructor1) {
    EXPECT_EQ(results1Vec, results1.getIssues());
    EXPECT_EQ(0, results1.getIssues().size());
    ASSERT_FALSE(results1 == results2);
  }

  TEST(ResultsTest, constructor2) {
    EXPECT_EQ(results2Vec, results2.getIssues());
    EXPECT_EQ(1, results2.getIssues().size());
    EXPECT_EQ(issue1, results2.getIssues()[0]);
    ASSERT_FALSE(results3 == results2);
  }

  TEST(ResultsTest, constructor3) {
    EXPECT_EQ(results3Vec, results3.getIssues());
    EXPECT_EQ(3, results3.getIssues().size());
    EXPECT_EQ(issue1, results3.getIssues()[0]);
    EXPECT_EQ(issue2, results3.getIssues()[1]);
    EXPECT_EQ(issue3, results3.getIssues()[2]);
    ASSERT_FALSE(results1 == results3);
  }

  TEST(ResultsTest, copyConstructor1) {
    Results a(results1);
    EXPECT_EQ(a, results1);
  }
  
  TEST(ResultsTest, copyConstructor2) {
    Results b(results2);
    EXPECT_EQ(b, results2);
  }
  
  TEST(ResultsTest, copyConstructor3) {
    Results c(results3);
    EXPECT_EQ(c, results3);
  }
  
  TEST(ResultsTest, copyConstructor4) {
    Results dr(dummyResults());
    EXPECT_EQ(dr, dummyResults());
  }

  const std::string resultsToXML(const Results& results) {
    std::ostringstream os;
    std::vector<Issue> v = results.getIssues();
    
    os << "<results>\n";
    for (std::vector<Issue>::iterator iter = v.begin();
	 iter != v.end();
	 ++iter)
      os << iter->toXML() + "\n";
    os << "</results>";
    return os.str();
  }
  
  TEST(ResultsTest, toXML) {
    std::string xml1 = results1.toXML();
    std::string xml2 = results2.toXML();
    std::string xml3 = results3.toXML();
    EXPECT_EQ("<results>\n</results>", xml1);
    EXPECT_EQ(xml2, resultsToXML(results2));
    EXPECT_EQ(xml3, resultsToXML(results3));
  }
  
  TEST(ResultsTestDummy, states) {
    std::vector<Issue> vIssues(dummyResults().getIssues());
    EXPECT_EQ(1, vIssues.size());
    EXPECT_EQ(dummyIssue(), vIssues[0]);
  }

  TEST(ResultsTestDummy, identity) {
    ASSERT_TRUE(dummyResults() == dummyResults());
  }
  
  TEST(ResultsTestDummy, toXML) {
    ASSERT_EQ("", dummyResults().toXML());
  }


  // Generator
  TEST(GeneratorTest, constructor1) {
    EXPECT_EQ("klee", gen1.getName());
    EXPECT_EQ("1.2.0", gen1.getVersion());
    ASSERT_FALSE(gen1 == gen2);
  }

  TEST(GeneratorTest, constructor2) {
    EXPECT_EQ("clanganalyzer", gen2.getName());
    EXPECT_EQ("n/a", gen2.getVersion());
    ASSERT_FALSE(gen2 == dummyGenerator());
  }

  TEST(GeneratorTest, copyConstructor1) {
    Generator a(gen1);
    EXPECT_EQ(a, gen1);
  }

  TEST(GeneratorTest, copyConstructor2) {
    Generator b(gen2);
    EXPECT_EQ(b, gen2);
  }

  TEST(GeneratorTest, copyConstructor3) {
    Generator dg(dummyGenerator());
    EXPECT_EQ(dg, dummyGenerator());
  }

  TEST(GeneratorTest, toXML) {
    std::string xml1 = "<generator name=\"klee\" version=\"1.2.0\"/>";
    std::string xml2 = "<generator name=\"clanganalyzer\" version=\"n/a\"/>";
    EXPECT_EQ(xml1, gen1.toXML());
    EXPECT_EQ(xml2, gen2.toXML());
  }

  TEST(GeneratorTestDummy, toXML) {
    EXPECT_EQ("", dummyGenerator().toXML());
  }


  // Metadata
  TEST(MetadataTest, constructor) {
    EXPECT_EQ(gen1, metadata1.getGenerator());
    EXPECT_EQ(gen2, metadata2.getGenerator());
    // TODO: SUT tests missing here
    ASSERT_FALSE(metadata1 == metadata2);
  }

  TEST(MetadataTest, copyConstructor1) {
    Metadata a(metadata1);
    EXPECT_EQ(a, metadata1);
  }
  
  TEST(MetadataTest, copyConstructor2) {
    Metadata b(metadata2);
    EXPECT_EQ(b, metadata2);
  }
  
  TEST(MetadataTest, copyConstructor3) {
    Metadata dm(dummyMetadata());
    EXPECT_EQ(dm, dummyMetadata());
  }

  const std::string metadataToXML(const Metadata& metadata) {
    std::ostringstream os;

    os << "<metadata>\n";
    os << metadata.getGenerator().toXML() + "\n";
    // os << metadata.getSUT().toXML() + "\n";
    os << "</metadata>";

    return os.str();
  }
  
  TEST(MetadataTest, toXML) {
    std::string xml1 = metadata1.toXML();
    std::string xml2 = metadata2.toXML();
    EXPECT_EQ(xml1, metadataToXML(metadata1));
    EXPECT_EQ(xml2, metadataToXML(metadata2));
  }

  TEST(MetadataTestDummy, toXML) {
    EXPECT_EQ("", dummyMetadata().toXML());
  }


  // Analysis
  TEST(AnalysisTest, constructor1) {
    EXPECT_EQ(metadata1, analysis1.getMetadata());
    EXPECT_EQ(results1, analysis1.getResults());
    ASSERT_FALSE(analysis1 == analysis2);
  }
  
  TEST(AnalysisTest, constructor2) {
    EXPECT_EQ(metadata1, analysis2.getMetadata());
    EXPECT_EQ(results2, analysis2.getResults());
    ASSERT_FALSE(analysis3 == analysis2);
  }
  
  TEST(AnalysisTest, constructor3) {
    EXPECT_EQ(metadata2, analysis3.getMetadata());
    EXPECT_EQ(results3, analysis3.getResults());
    ASSERT_FALSE(analysis1 == analysis3);
  }
  
  TEST(AnalysisTest, copyConstructor1) {
    Analysis a(analysis1);
    EXPECT_EQ(a, analysis1);
  }
  
  TEST(AnalysisTest, copyConstructor2) {
    Analysis b(analysis2);
    EXPECT_EQ(b, analysis2);
  }
  
  TEST(AnalysisTest, copyConstructor3) {
    Analysis c(analysis3);
    EXPECT_EQ(c, analysis3);
  }
  
  TEST(AnalysisTest, copyConstructor4) {
    Analysis da(dummyAnalysis());
    EXPECT_EQ(da, dummyAnalysis());
  }

  const std::string analysisToXML(const Analysis& analysis) {
    std::ostringstream os;

    os << "<analysis>\n";
    os << analysis.getMetadata().toXML() + "\n";
    os << analysis.getResults().toXML() + "\n";
    os << "</analysis>";

    return os.str();
  }
  
  TEST(AnalysisTest, toXML) {
    std::string xml1 = analysis1.toXML();
    std::string xml2 = analysis2.toXML();
    std::string xml3 = analysis3.toXML();
    EXPECT_EQ(xml1, analysisToXML(analysis1));
    EXPECT_EQ(xml2, analysisToXML(analysis2));
    EXPECT_EQ(xml3, analysisToXML(analysis3));
  }

  TEST(AnalysisTestDummy, toXML) {
    EXPECT_EQ("", dummyAnalysis().toXML());
  }  
}
