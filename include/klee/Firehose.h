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
    static Point& dummyPoint() {
      static Point dummy(0, 0);
      return dummy;
    }

    
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
    static Range& dummyRange() {
      static Range dummy(dummyPoint(), dummyPoint());
      return dummy;
    }

    
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
    static File& dummyFile() {
      static File dummy("");
      return dummy;
    }

    
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
    static Function& dummyFunction() {
      static Function dummy("");
      return dummy;
    }
    

    class Location: public XML {
    private:
      File m_file;
      Function m_function;
      Range m_range;
      Point m_point;
    public:
      Location(const File& file, const Function& function,
	       const Range& range = dummyRange());
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
    static Location& dummyLocation() {
      static Location dummy(dummyFile(), dummyFunction(), dummyRange());
      return dummy;
    }

    
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
    static Message& dummyMessage() {
      static Message dummy(std::string(""));
      return dummy;
    }


    class Notes: public Text {
    public:
      Notes(const std::string& notes);
      Notes(const char* notes);
      Notes(const Notes& that);
      bool operator ==(const Notes& that) const;
      const std::string toXML() const;
    };
    
    // Dummy notes value
    static Notes& dummyNotes() {
      static Notes dummy(std::string(""));
      return dummy;
    }


    class State: public XML {
    private:
      Location m_location;
      Notes m_notes;
    public:
      State(const Location& location, const Notes& notes = dummyNotes());
      State(const State& that);
      const Location& getLocation() const;
      const Notes& getNotes() const;
      bool operator ==(const State& that) const;
      const std::string toXML() const;
    };

    // Dummy location value
    static State& dummyState() {
      static State dummy(dummyLocation(), dummyNotes());
      return dummy;
    }
    

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
    static Trace& dummyTrace() {
      static Trace dummy(std::vector<State>(1, dummyState()));
      return dummy;
    }


    class ResultType: public XML {
    public:
      ResultType() {}
    };

    
    class Issue: public ResultType {
    // class Issue: public XML {
    private:
      Message m_message;
      Location m_location;
      Trace m_trace;
    public:
      Issue(const Message& message, const Location& location,
	    const Trace& trace = dummyTrace());
      Issue(const Issue& that);
      const Message& getMessage() const;
      const Location& getLocation() const;
      const Trace& getTrace() const;
      bool operator ==(const Issue& that) const;
      const std::string toXML() const;
    };

    // Dummy issue value
    static Issue& dummyIssue() {
      static Issue dummy(dummyMessage(), dummyLocation(), dummyTrace());
      return dummy;
    }


    // Dimjašević: This is not a good design, but I do not want to
    // deal with object slicing, pointers, and exceptions.
    class FailureOrInfo: public ResultType {
    protected:
      std::string m_id;
      Message m_message;
      Location m_location;
    public:
      // the only two values you should pass as 'id' are
      // "symbol-loading" and "external-call"
      FailureOrInfo(const std::string& id,
		    const Message& message,
		    const Location& location = dummyLocation());
      const std::string& getId() const;
      const Message& getMessage() const;
    };

    
    class Failure: public FailureOrInfo {
    public:
      // the only two values you should pass as 'id' are
      // "symbol-loading" and "external-call"
      Failure(const std::string& id, const Message& message,
	      const Location& location = dummyLocation());
      Failure(const Failure& that);
      const Location& getLocation() const;
      bool operator==(const Failure& that) const;
      const std::string toXML() const;
    };

    // Dummy failure value
    static Failure& dummyFailure() {
      static Failure dummy(std::string(""), dummyMessage(), dummyLocation());
      return dummy;
    }


    class Info: public FailureOrInfo {
    public:
      Info(const std::string& id, const Message& message);
      Info(const Info& that);
      bool operator==(const Info& that) const;
      const std::string toXML() const;
    };

    // Dummy info value
    static Info& dummyInfo() {
      static Info dummy(std::string(""), dummyMessage());
      return dummy;
    }
    

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

    static Results& dummyResults() {
      static Results dummy(std::vector<Issue>(1, dummyIssue()));
      return dummy;
    }
    

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
    static Generator& dummyGenerator() {
      static Generator dummy(std::string(""), std::string(""));
      return dummy;
    }


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
    static Metadata& dummyMetadata() {
      static Metadata dummy(dummyGenerator());
      return dummy;
    }

    
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
    static Analysis& dummyAnalysis() {
      static Analysis dummy(dummyMetadata(), dummyResults());
      return dummy;
    }
  }
}

#endif
