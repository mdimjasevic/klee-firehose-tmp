//===-- Firehose.cpp ------------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/Firehose.h"

#include <iostream>
#include <sstream>

using namespace klee::firehose;


Point::Point(unsigned column, unsigned line): m_column(column), m_line(line) {}

Point::Point(): m_column(0), m_line(0) {}

unsigned Point::getColumn() const { return this->m_column; }
unsigned Point::getLine() const   { return this->m_line; }

Point::Point(const Point& that) {
  this->m_column = that.getColumn();
  this->m_line = that.getLine();
}

Point& Point::operator=(const Point& that) {
  this->m_column = that.getColumn();
  this->m_line = that.getLine();
  return *this;
}

std::string Point::toXML() const {
  return std::string("<point column=\"" + std::to_string(this->getColumn()) +
		     "\" line=\"" + std::to_string(this->getLine()) + "\"/>");
}

Range::Range(const Point& p1, const Point& p2) {
  this->m_p1 = p1;
  this->m_p2 = p2;
}

Range::Range() {}

Range::Range(const Range& that) {
  this->m_p1 = that.getP1();
  this->m_p2 = that.getP2();
}

Range& Range::operator=(const Range& that) {
  this->m_p1 = that.getP1();
  this->m_p2 = that.getP2();
  return *this;
}

Point Range::getP1() const { return this->m_p1; }
Point Range::getP2() const { return this->m_p2; }

std::string Range::toXML() const {
  return std::string("<range>\n" +
		     this->getP1().toXML() + "\n" +
		     this->getP2().toXML() + "\n" +
		     "</range>");
}


File::File(const std::string& path): m_path(path) {}

File::File(): m_path("") {}

File::File(const char* path): m_path(path) {}

std::string File::getPath() const { return this->m_path; }

File::File(const File& that) { this->m_path = that.getPath(); }

File& File::operator=(const File& that) {
  this->m_path = that.getPath();
  return *this;
}

std::string File::toXML() const {
  return std::string("<file given-path=\"" + this->getPath() + "\"/>");
}

Function::Function(const std::string& name): m_name(name) {}

Function::Function(): m_name("") {}

Function::Function(const char* name): m_name(name) {}

std::string Function::getName() const { return this->m_name; }
  
Function::Function(const Function& that) { this->m_name = that.getName(); }

Function& Function::operator=(const Function& that) {
  this->m_name = that.getName();
  return *this;
}

std::string Function::toXML() const {
  return std::string("<function name=\"" + this->getName() + "\"/>");
}


Location::Location(const File& file, const Function& function,
		   const Range& range) {
  this->m_file = file;
  this->m_function = function;
  this->m_range = range;
}

File Location::getFile() const { return this->m_file; }
Function Location::getFunction() const { return this->m_function; }
Range Location::getRange() const { return this->m_range; }

Location::Location(const Location& that) {
  this->m_file = that.getFile();
  this->m_function = that.getFunction();
  this->m_range = that.getRange();
}

Location::Location() {}

Location& Location::operator=(const Location& that) {
  this->m_file = that.getFile();
  this->m_function = that.getFunction();
  this->m_range = that.getRange();
  return *this;
}

std::string Location::toXML() const {
  return std::string("<location>\n" +
		     this->getFile().toXML() + "\n" +
		     this->getFunction().toXML() + "\n" +
		     this->getRange().toXML() + "\n" +
		     "</location>");
}


Trace::Trace(const std::vector<State>& states) {
  this->m_states = std::vector<State>(states);
}

Trace::Trace(const Trace& that) {
  this->m_states = std::vector<State>(that.getStates());
}

Trace::Trace() {}

Trace& Trace::operator=(const Trace& that) {
  this->m_states = std::vector<State>(that.getStates());
  return *this;
}

std::vector<State> Trace::getStates() const {
  return this->m_states;
}

std::string Trace::toXML() const {
  std::ostringstream os;

  os << "<trace>\n";

  for(std::vector<State>::iterator iter = this->getStates().begin();
      iter != this->getStates().end();
      ++iter)
    os << iter->toXML() + "\n";

  os << "</trace>";

  return os.str();
}
