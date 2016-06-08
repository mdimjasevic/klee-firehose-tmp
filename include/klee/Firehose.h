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

    class XML {
    public:
      virtual std::string toXML() const = 0;
    };

    class Point: public XML {
    private:
      unsigned m_column;
      unsigned m_line;
    public: 
      Point(); // This one is unfortunately needed in C++, though I
      	       // don't want the constructor public.
      Point(unsigned column, unsigned line);
      Point(const Point& that);
      Point& operator=(const Point& that);
      unsigned getColumn() const;
      unsigned getLine() const;
      std::string toXML() const;
    };
    
    class Range: public XML {
    private:
      Point m_p1, m_p2;
    public:
      Range(const Point& p1, const Point& p2);
      Range(const Range& that);
      Range(); // This one is unfortunately needed in C++, though I
      	       // don't want the constructor public.
      Range& operator=(const Range& that);
      Point getP1() const;
      Point getP2() const;
      std::string toXML() const;
    };
    
    class File: public XML {
    private:
      std::string m_path;
    public:
      File(const std::string& path);
      File(); // This one is unfortunately needed in C++, though I
	      // don't want the constructor public.
      File(const char* path);
      File(const File& that);
      File& operator=(const File& that);
      std::string getPath() const;
      std::string toXML() const;
    };
    
    class Function: public XML {
    private:
      std::string m_name;
    public: 
      Function(const std::string& name);
      Function(); // This one is unfortunately needed in C++, though I
		  // don't want the constructor public.
      Function(const char* name);
      Function(const Function& that);
      Function& operator=(const Function& that);
      std::string getName() const;
      std::string toXML() const;
    };

    class Location: public XML {
    private:
      File m_file;
      Function m_function;
      Range m_range;
    public:
      Location(const File& file, const Function& function, const Range& range);
      Location(const Location& that);
      Location(); // This one is unfortunately needed in C++, though I
		  // don't want the constructor public.
      Location& operator=(const Location& that);
      File getFile() const;
      Function getFunction() const;
      Range getRange() const;
      std::string toXML() const;
    };
    
    class State: public XML {
    private:
      Location m_location;
    public:
      State(const Location& location);
      State(const State& that);
      State(); // This one is unfortunately needed in C++, though I
	       // don't want the constructor public.
      State& operator=(const State& that);
      Location getLocation() const;
      std::string toXML() const;
    };

    class Trace: public XML {
    private:
      std::vector<State> m_states;
    public:
      Trace(const std::vector<State>& states);
      Trace(const Trace& that);
      Trace(); // This one is unfortunately needed in C++, though I
	       // don't want the constructor public.
      Trace& operator=(const Trace& that);
      std::vector<State> getStates() const;
      std::string toXML() const;
    };

    // class Analysis {
    // };
    
    // class Metadata {
    // };
    
    // class Generator {
    // };
    
    // class SUT {
    // };
    
    // class DebianSource {
    // };
    
    // class Results {
    // };
    
    // class Issue {
    // };
    
    // class Message {
    // };    
  }
}

#endif
