//===-- Firehose.cpp ------------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/Firehose.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

using namespace klee::firehose;


template <typename T> std::string numberToString(T Number) {
  std::ostringstream ss;
  ss << Number;
  return ss.str();
}

const std::string XML::mkString(std::vector<std::string>& ss,
				const std::string& sep) const {
  std::vector<std::string> filtered;
  for(std::vector<std::string>::iterator iter = ss.begin();
      iter != ss.end();
      ++iter)
    if (*iter != "") filtered.push_back(*iter);
  
  std::ostringstream os;
  for(unsigned i = 0; i < filtered.size() - 1; ++i)
    os << filtered[i] << sep;
  if (filtered.size() > 0)
    os << filtered[filtered.size() - 1];
  
  return os.str();
}


Point::Point(unsigned column, unsigned line):
  m_column(column), m_line(line) {}

unsigned Point::getColumn() const { return this->m_column; }
unsigned Point::getLine() const   { return this->m_line; }

Point::Point(const Point& that) {
  this->m_column = that.getColumn();
  this->m_line = that.getLine();
}

bool Point::operator ==(const Point& that) const {
  return
    this->getColumn() == that.getColumn() &&
    this->getLine() == that.getLine();
}

const std::string Point::toXML() const {
  if (!(*this == dummyPoint))
    return std::string("<point column=\"" + numberToString(this->getColumn()) +
		       "\" line=\"" + numberToString(this->getLine())
		       + "\"/>");
  else return "";
}

Range::Range(const Point& p1, const Point& p2): m_p1(p1), m_p2(p2) {}

Range::Range(const Range& that): m_p1(that.getP1()), m_p2(that.getP2()) {}

const Point& Range::getP1() const { return this->m_p1; }
const Point& Range::getP2() const { return this->m_p2; }

bool Range::operator ==(const Range& that) const {
  return
    this->getP1() == that.getP1() &&
    this->getP2() == that.getP2();
}

const std::string Range::toXML() const {
  if (!(*this == dummyRange)) {
    std::vector<std::string> r;
    r.push_back("<range>");
    r.push_back(this->getP1().toXML());
    r.push_back(this->getP2().toXML());
    r.push_back("</range>");
    return mkString(r);
  }
  else return "";
}


File::File(const std::string& path): m_path(path) {}

File::File(const char* path): m_path(path) {}

const std::string& File::getPath() const { return this->m_path; }

File::File(const File& that) { this->m_path = that.getPath(); }

bool File::operator ==(const File& that) const {
  return this->getPath() == that.getPath();
}

const std::string File::toXML() const {
  if (!(*this == dummyFile))
    return std::string("<file given-path=\"" + this->getPath() + "\"/>");
  else return "";
}


Function::Function(const std::string& name): m_name(name) {}

Function::Function(const char* name): m_name(name) {}

const std::string& Function::getName() const { return this->m_name; }
  
Function::Function(const Function& that): m_name(that.getName()) {}

bool Function::operator ==(const Function& that) const {
  return this->getName() == that.getName();
}

const std::string Function::toXML() const {
  if (!(*this == dummyFunction))
    return std::string("<function name=\"" + this->getName() + "\"/>");
  else return "";
}


Location::Location(const File& file, const Function& function,
		   const Range& range):
  m_file(file), m_function(function), m_range(range), m_point(dummyPoint) {}

Location::Location(const File& file, const Function& function,
		   const Point& point):
  m_file(file), m_function(function), m_range(dummyRange), m_point(point) {}

const File& Location::getFile() const         { return this->m_file; }
const Function& Location::getFunction() const { return this->m_function; }
const Range& Location::getRange() const       { return this->m_range; }
const Point& Location::getPoint() const       { return this->m_point; }

Location::Location(const Location& that):
  m_file(that.getFile()), m_function(that.getFunction()),
  m_range(that.getRange()), m_point(that.getPoint()) {}

bool Location::operator ==(const Location& that) const {
  return
    this->getFile() == that.getFile() &&
    this->getFunction() == that.getFunction() &&
    this->getRange() == that.getRange() &&
    this->getPoint() == that.getPoint();
}

const std::string Location::toXML() const {
  if (!(*this == dummyLocation)) {
    std::vector<std::string> r;
    r.push_back("<location>");
    r.push_back(this->getFile().toXML());
    r.push_back(this->getFunction().toXML());
    r.push_back(this->getRange().toXML());
    r.push_back(this->getPoint().toXML());
    r.push_back("</location>");
    return mkString(r);
  }
  else return "";
}


State::State(const Location& location): m_location(location) {}

const Location& State::getLocation() const { return this->m_location; }

State::State(const State& that): m_location(that.getLocation()) {}

bool State::operator ==(const State& that) const {
  return this->getLocation() == that.getLocation();
}

const std::string State::toXML() const {
  if (!(*this == dummyState)) {
    std::vector<std::string> r;
    r.push_back("<state>");
    r.push_back(this->getLocation().toXML());
    r.push_back("</state>");
    return mkString(r);
  }
  else return "";
}


Trace::Trace(const std::vector<State>& states):
  m_states(std::vector<State>(states)) {}

Trace::Trace(const Trace& that):
  m_states(std::vector<State>(that.getStates())) {}

const std::vector<State>& Trace::getStates() const { return this->m_states; }

bool Trace::operator ==(const Trace& that) const {
  return this->getStates() == that.getStates();
}

const std::string Trace::toXML() const {
  if (!(*this == dummyTrace)) {
    std::vector<State> tmp(this->getStates().begin(), this->getStates().end());
    std::vector<std::string> r;
    r.push_back("<trace>");
    for (std::vector<State>::iterator iter = tmp.begin();
	iter != tmp.end();
	++iter)
      r.push_back(iter->toXML());
    r.push_back("</trace>");
    return mkString(r);
  }
  else return "";
}

Message::Message(const std::string& msg): m_msg(msg) {}

Message::Message(const Message& that): m_msg(that.get()) {}

const std::string& Message::get() const { return this->m_msg; }

bool Message::operator ==(const Message& that) const {
  return this->get() == that.get();
}

const std::string Message::toXML() const {
  if (!(*this == dummyMessage))
    return std::string("<message>" + this->get() + "</message>");
  else return "";
}


Issue::Issue(const Message& message, const Location& location,
	     const Trace& trace):
  m_message(message), m_location(location), m_trace(trace) {}

Issue::Issue(const Issue& that):
  m_message(that.getMessage()), m_location(that.getLocation()),
  m_trace(that.getTrace()) {}

const Message& Issue::getMessage() const   { return this->m_message; }
const Location& Issue::getLocation() const { return this->m_location; }
const Trace& Issue::getTrace() const       { return this->m_trace; }

bool Issue::operator ==(const Issue& that) const {
  return
    this->getMessage() == that.getMessage() &&
    this->getLocation() == that.getLocation() &&
    this->getTrace() == that.getTrace();
}

const std::string Issue::toXML() const {
  if (!(*this == dummyIssue)) {
    std::vector<std::string> r;
    r.push_back("<issue>");
    r.push_back(this->getMessage().toXML());
    r.push_back(this->getLocation().toXML());
    r.push_back(this->getTrace().toXML());
    r.push_back("</issue>");
    return mkString(r);
  }
  else return "";
}


Results::Results(const std::vector<Issue>& issues): m_issues(issues) {}

Results::Results(const Results& that): m_issues(that.getIssues()) {}

const std::vector<Issue>& Results::getIssues() const { return this->m_issues; }

bool Results::operator ==(const Results& that) const {
  return this->getIssues() == that.getIssues();
}

const std::string Results::toXML() const {
  if (!(*this == dummyResults)) {
    std::vector<Issue> tmp(this->getIssues().begin(), this->getIssues().end());
    std::vector<std::string> r;
    r.push_back("<results>");
    for(std::vector<Issue>::iterator iter = tmp.begin();
	iter != tmp.end();
	++iter)
      r.push_back(iter->toXML());
    r.push_back("</results>");
    return mkString(r);
  }
  else return "";
}


Generator::Generator(const std::string& name, const std::string& version):
  m_name(name), m_version(version) {}

Generator::Generator(const Generator& that):
  m_name(that.getName()), m_version(that.getVersion()) {}

const std::string& Generator::getName() const    { return this->m_name; }
const std::string& Generator::getVersion() const { return this->m_version; }

bool Generator::operator ==(const Generator& that) const {
  return
    this->getName() == that.getName() &&
    this->getVersion() == that.getVersion();
}

const std::string Generator::toXML() const {
  if (!(*this == dummyGenerator))
    return std::string("<generator name=\"" + this->getName() +
		       "\" version=\"" + this->getVersion() + "\"/>");
  else return "";
}


Metadata::Metadata(const Generator& generator): m_generator(generator) {}
// : m_sut(sut)

Metadata::Metadata(const Metadata& that): m_generator(that.getGenerator()) {}
// : m_sut(that.getSUT())

const Generator& Metadata::getGenerator() const { return this->m_generator; }
// const SUT& Metadata::getSUT() const             { return this->m_sut; }

bool Metadata::operator ==(const Metadata& that) const {
  return
    this->getGenerator() == that.getGenerator();
  // && this->getSUT() == that.getSUT();
}

const std::string Metadata::toXML() const {
  if (!(*this == dummyMetadata)) {
    std::vector<std::string> r;
    r.push_back("<metadata>");
    r.push_back(this->getGenerator().toXML());
    // r.push_back(this->getSUT().toXML());
    r.push_back("</metadata>");
    return mkString(r);
  }
  else return "";
}


Analysis::Analysis(const Metadata& metadata, const Results& results):
  m_metadata(metadata), m_results(results) {}

Analysis::Analysis(const Analysis& that):
  m_metadata(that.getMetadata()), m_results(that.getResults()) {}

const Metadata& Analysis::getMetadata() const { return this->m_metadata; }
const Results& Analysis::getResults() const   { return this->m_results; }

bool Analysis::operator ==(const Analysis& that) const {
  return
    this->getMetadata() == that.getMetadata() &&
    this->getResults() == that.getResults();
}

const std::string Analysis::toXML() const {
  if (!(*this == dummyAnalysis)) {
    std::vector<std::string> r;
    r.push_back("<analysis>");
    r.push_back(this->getMetadata().toXML());
    r.push_back(this->getResults().toXML());
    r.push_back("</analysis>");
    return mkString(r);
  }
  else return "";
}
