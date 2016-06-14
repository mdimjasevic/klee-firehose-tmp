//===-- Firehose.h ----------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

/*
 * This file defines classes with data structures and functions for
 * outputting results in the Firehose format:
 *
 * https://github.com/fedora-static-analysis/firehose
 */

#ifndef KLEE_FIREHOSE_H
#define KLEE_FIREHOSE_H

#include <string>
#include <vector>

namespace klee {
  namespace firehose {

    template <typename T> std::string NumberToString(T Number);
    // Dummy string value used for default reference parameter values
    static const std::string dummyString("");

    class XML {
    protected:
      const std::string mkString(std::vector<std::string>& ss,
				 std::string sep = "\n") const;
    public:
      virtual const std::string toXML() const = 0;
      XML() {}
    };

    class Point: public XML {
    private:
      unsigned m_column;
      unsigned m_line;
    public:
      Point(unsigned column, unsigned line);
      Point(const Point& that);
      unsigned getColumn() const;
      unsigned getLine() const;
      const std::string toXML() const;
    };

    // Dummy point value
    static const Point dummyPoint = Point(0, 0);

    
    class Range: public XML {
    private:
      Point m_p1, m_p2;
    public:
      Range(const Point& p1, const Point& p2);
      Range(const Range& that);
      const Point& getP1() const;
      const Point& getP2() const;
      const std::string toXML() const;
    };

    // Dummy range value
    static const Range dummyRange = Range(dummyPoint, dummyPoint);

    
    class File: public XML {
    private:
      std::string m_path;
    public:
      File(const std::string& path);
      File(const char* path);
      File(const File& that);
      const std::string& getPath() const;
      const std::string toXML() const;
    };

    // Dummy file value
    static const File dummyFile(dummyString);

    
    class Function: public XML {
    private:
      std::string m_name;
    public: 
      Function(const std::string& name);
      Function(const char* name);
      Function(const Function& that);
      const std::string& getName() const;
      const std::string toXML() const;
    };

    // Dummy function value
    static const Function dummyFunction(dummyString);
    

    class Location: public XML {
    private:
      File m_file;
      Function m_function;
      Range m_range;
      Point m_point;
    public:
      Location(const File& file, const Function& function = dummyFunction,
	       const Range& range = dummyRange);
      Location(const File& file, const Function& function = dummyFunction,
	       const Point& point = dummyPoint);
      Location(const Location& that);
      const File& getFile() const;
      const Function& getFunction() const;
      const Range& getRange() const;
      const Point& getPoint() const;
      const std::string toXML() const;
    };

    //Dummy location value
    static const Location dummyLocation(dummyFile, dummyFunction, dummyRange);

    
    class State: public XML {
    private:
      Location m_location;
    public:
      State(const Location& location);
      State(const State& that);
      const Location& getLocation() const;
      const std::string toXML() const;
    };

    // Dummy location value
    static const State dummyState(dummyLocation);
    

    class Trace: public XML {
    private:
      std::vector<State> m_states;
    public:
      Trace(const std::vector<State>& states);
      Trace(const Trace& that);
      const std::vector<State>& getStates() const;
      const std::string toXML() const;
    };

    // Dummy trace value
    static const Trace dummyTrace(std::vector<State>(1, dummyState));

    
    class Message: public XML {
    private:
      std::string m_msg;
    public:
      Message(const std::string& msg);
      Message(const Message& that);
      const std::string& get() const;
      const std::string toXML() const;
    };

    // Dummy message value
    static const Message dummyMessage(dummyString);

    
    class Issue: public XML {
    private:
      Message m_message;
      Location m_location;
      Trace m_trace;
    public:
      Issue(const Message& message, const Location& location,
	    const Trace& trace = dummyTrace);
      Issue(const Issue& that);
      const Message& getMessage() const;
      const Location& getLocation() const;
      const Trace& getTrace() const;
      const std::string toXML() const;
    };

    // Dummy issue value
    static const Issue dummyIssue(dummyMessage, dummyLocation, dummyTrace);
    

    class Results: public XML {
    private:
      std::vector<Issue> m_issues;
    public:
      Results(const std::vector<Issue>& issues);
      Results(const Results& that);
      const std::vector<Issue>& getIssues() const;
      const std::string toXML() const;
    };

    static const Results dummyResults(std::vector<Issue>(1, dummyIssue));
    

    class Generator: public XML {
    private:
      std::string m_name;
      std::string m_version;
    public:
      Generator(const std::string& name, const std::string& version);
      Generator(const Generator& that);
      const std::string& getName() const;
      const std::string& getVersion() const;
      const std::string toXML() const;
    };

    // Dummy generator value
    static const Generator dummyGenerator(dummyString, dummyString);


    class SUT: public XML {
      // TODO: implement this class and add it to as a member of the
      // Metadata class
    };


    class Metadata: public XML {
    private:
      Generator m_generator;
      // SUT m_sut;
    public:
      Metadata(const Generator& generator);
      // Metadata(const Generator& generator, const SUT& sut);
      Metadata(const Metadata& that);
      const Generator& getGenerator() const;
      // const SUT& getSUT() const;
      const std::string toXML() const;
    };

    // Dummy metadata value
    static const Metadata dummyMetadata(dummyGenerator);

    
    class Analysis: public XML {
    private:
      Metadata m_metadata;
      Results m_results;
    public:
      Analysis(const Metadata& metadata, const Results& results);
      Analysis(const Analysis& that);
      const Metadata& getMetadata() const;
      const Results& getResults() const;
      const std::string toXML() const;
    };

    // Dummy analysis value
    static const Analysis dummyAnalysis(dummyMetadata, dummyResults);
  }
}

#endif
