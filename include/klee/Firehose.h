/* -*- mode: c++; c-basic-offset: 2; -*- */

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
				 const std::string& sep = "\n") const;
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
      bool operator ==(const Point& that) const;
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
      bool operator ==(const Range& that) const;
      const std::string toXML() const;
    };

    // Dummy range value
    static const Range dummyRange = Range(dummyPoint, dummyPoint);

    
    // <stats wall-clock-time="5"/>
    // maybe <failure> that is anologous to issue
      
      
    class File: public XML {
    private:
      std::string m_path;
    public:
      File(const std::string& path);
      File(const char* path);
      File(const File& that);
      const std::string& getPath() const;
      bool operator ==(const File& that) const;
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
      bool operator ==(const Function& that) const;
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
      Location(const File& file, const Function& function,
	       const Range& range = dummyRange);
      Location(const File& file, const Function& function, const Point& point);
      Location(const Location& that);
      const File& getFile() const;
      const Function& getFunction() const;
      const Range& getRange() const;
      const Point& getPoint() const;
      bool operator ==(const Location& that) const;
      const std::string toXML() const;
    };

    //Dummy location value
    static const Location dummyLocation(dummyFile, dummyFunction, dummyRange);

    
    // An abstract class that is inherited by Message and Notes
    class Text: public XML {
    private:
      std::string m_text;
    public:
      Text(const std::string& text);
      Text(const char* text);
      const std::string& get() const;
    };
    
    class Message: public Text {
    public:
      Message(const std::string& msg);
      Message(const char* msg);
      Message(const Message& that);
      bool operator ==(const Message& that) const;
      const std::string toXML() const;
    };

    // Dummy message value
    static const Message dummyMessage(dummyString);


    class Notes: public Text {
    public:
      Notes(const std::string& notes);
      Notes(const char* notes);
      Notes(const Notes& that);
      bool operator ==(const Notes& that) const;
      const std::string toXML() const;
    };
    
    // Dummy notes value
    static const Notes dummyNotes(dummyString);


    class State: public XML {
    private:
      Location m_location;
      Notes m_notes;
    public:
      State(const Location& location, const Notes& notes = dummyNotes);
      State(const State& that);
      const Location& getLocation() const;
      const Notes& getNotes() const;
      bool operator ==(const State& that) const;
      const std::string toXML() const;
    };

    // Dummy location value
    static const State dummyState(dummyLocation, dummyNotes);
    

    class Trace: public XML {
    private:
      std::vector<State> m_states;
    public:
      Trace(const std::vector<State>& states);
      Trace(const Trace& that);
      const std::vector<State>& getStates() const;
      bool operator ==(const Trace& that) const;
      const std::string toXML() const;
    };

    // Dummy trace value
    static const Trace dummyTrace(std::vector<State>(1, dummyState));


    class ResultType: public XML {
    };

    
    class Issue: public ResultType {
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
      bool operator ==(const Issue& that) const;
      const std::string toXML() const;
    };

    // Dummy issue value
    static const Issue dummyIssue(dummyMessage, dummyLocation, dummyTrace);


    // Dimjašević: This is not a good design, but I do not want to
    // deal with object slicing, pointers, and exceptions.
    class Failure: public ResultType {
    private:
      std::string m_id;
      Message m_message;
      Location m_location;
    public:
      // the only two values you should pass as 'id' are
      // "symbol-loading" and "external-call"
      Failure(const std::string& id, const Message& message,
	      const Location& location = dummyLocation);
      Failure(const Failure& that);
      const std::string& getId() const;
      const Message& getMessage() const;
      const Location& getLocation() const;
      bool operator==(const Failure& that) const;
      const std::string toXML() const;
    };

    // Dummy failure value
    static const Failure dummyFailure(dummyString, dummyMessage,
				      dummyLocation);

    
    class Info: public ResultType {
    private:
      
    public:
    };

    class Results: public XML {
    private:
      std::vector<Issue> m_issues;
      // std::vector<Failure> m_failures;
      // std::vector<Info> m_infos;
    public:
      Results(const std::vector<Issue>& issues);
      // Results(const std::vector<Issue>& issues,
      // 	      const std::vector<Failure>& failures =
      // 	      std::vector<Failure>(),
      // 	      const std::vector<Info>& infos =
      // 	      std::vector<Info>());
      // Results(const std::vector<Failure>& failures);
      // Results(const std::vector<Info>& infos);
      Results(const Results& that);
      const std::vector<Issue>& getIssues() const;
      // const std::vector<Failure>& getFailures() const;
      // const std::vector<Info>& getInfos() const;
      bool operator ==(const Results& that) const;
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
      bool operator ==(const Generator& that) const;
      const std::string toXML() const;
    };

    // Dummy generator value
    static const Generator dummyGenerator(dummyString, dummyString);


    class SUT: public XML {
      // TODO: implement this class and add it to as a member of the
      // Metadata class
      // Ideally, it should have a <debian-source> tag, as illustrated at:
      //
      // https://github.com/fedora-static-analysis/firehose/blob/master/examples/example-debian-source.xml
      //
      // However, leave this to Debile, i.e. post-process the report
      // obtained from KLEE in Debile.
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
      bool operator ==(const Metadata& that) const;
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
      bool operator ==(const Analysis& that) const;
      const std::string toXML() const;
    };

    // Dummy analysis value
    static const Analysis dummyAnalysis(dummyMetadata, dummyResults);
  }
}

#endif
