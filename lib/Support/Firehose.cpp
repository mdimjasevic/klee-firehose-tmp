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

using namespace klee;


firehose::Point::Point(unsigned column, unsigned line):
  m_column(column), m_line(line) {}

firehose::Point::Point(): m_column(0), m_line(0) {}

unsigned firehose::Point::getColumn() const { return this->m_column; }
unsigned firehose::Point::getLine() const   { return this->m_line; }

firehose::Point::Point(const firehose::Point& that) {
  this->m_column = that.getColumn();
  this->m_line = that.getLine();
}

firehose::Point& firehose::Point::operator=(const firehose::Point& that) {
  this->m_column = that.getColumn();
  this->m_line = that.getLine();
  return *this;
}

std::string firehose::Point::toXML() const {
  return std::string("<point column=\"" + std::to_string(this->getColumn()) +
		     "\" line=\"" + std::to_string(this->getLine()) + "\"/>");
}

firehose::Range::Range(const firehose::Point& p1, const firehose::Point& p2) {
  this->m_p1 = p1;
  this->m_p2 = p2;
}

firehose::Range::Range() {}

firehose::Range::Range(const firehose::Range& that) {
  this->m_p1 = that.getP1();
  this->m_p2 = that.getP2();
}

firehose::Range& firehose::Range::operator=(const firehose::Range& that) {
  this->m_p1 = that.getP1();
  this->m_p2 = that.getP2();
  return *this;
}

firehose::Point firehose::Range::getP1() const { return this->m_p1; }
firehose::Point firehose::Range::getP2() const { return this->m_p2; }

std::string firehose::Range::toXML() const {
  return std::string("<range>\n" +
		     this->getP1().toXML() + "\n" +
		     this->getP2().toXML() + "\n" +
		     "</range>");
}


firehose::File::File(const std::string& path): m_path(path) {}

firehose::File::File(): m_path("") {}

firehose::File::File(const char* path): m_path(path) {}

std::string firehose::File::getPath() const { return this->m_path; }

firehose::File::File(const firehose::File& that) {
  this->m_path = that.getPath();
}

firehose::File& firehose::File::operator=(const firehose::File& that) {
  this->m_path = that.getPath();
  return *this;
}

std::string firehose::File::toXML() const {
  return std::string("<file given-path=\"" + this->getPath() + "\"/>");
}

firehose::Function::Function(const std::string& name): m_name(name) {}

firehose::Function::Function(): m_name("") {}

firehose::Function::Function(const char* name): m_name(name) {}

std::string firehose::Function::getName() const { return this->m_name; }
  
firehose::Function::Function(const firehose::Function& that) {
  this->m_name = that.getName();
}

firehose::Function& firehose::Function::operator=(const firehose::Function&
						  that) {
  this->m_name = that.getName();
  return *this;
}

std::string firehose::Function::toXML() const {
  return std::string("<function name=\"" + this->getName() + "\"/>");
}


firehose::Location::Location(const firehose::File& file,
			     const firehose::Function& function,
			     const firehose::Range& range) {
  this->m_file = file;
  this->m_function = function;
  this->m_range = range;
}

firehose::File firehose::Location::getFile() const { return this->m_file; }
firehose::Function firehose::Location::getFunction() const {
  return this->m_function;
}
firehose::Range firehose::Location::getRange() const { return this->m_range; }

firehose::Location::Location(const firehose::Location& that) {
  this->m_file = that.getFile();
  this->m_function = that.getFunction();
  this->m_range = that.getRange();
}

firehose::Location::Location() {}

firehose::Location& firehose::Location::operator=(const firehose::Location&
						  that) {
  this->m_file = that.getFile();
  this->m_function = that.getFunction();
  this->m_range = that.getRange();
  return *this;
}

std::string firehose::Location::toXML() const {
  return std::string("<location>\n" +
		     this->getFile().toXML() + "\n" +
		     this->getFunction().toXML() + "\n" +
		     this->getRange().toXML() + "\n" +
		     "</location>");
}


firehose::Trace::Trace(const std::vector<firehose::State>& states) {
  this->m_states = std::vector<State>(states);
}

firehose::Trace::Trace(const firehose::Trace& that) {
  this->m_states = std::vector<State>(that.getStates());
}

firehose::Trace::Trace() {}

firehose::Trace& firehose::Trace::operator=(const firehose::Trace& that) {
  this->m_states = std::vector<State>(that.getStates());
  return *this;
}

std::vector<firehose::State> firehose::Trace::getStates() const {
  return this->m_states;
}

std::string firehose::Trace::toXML() const {
  std::ostringstream os;

  os << "<trace>\n";

  for(std::vector<State>::iterator iter = this->getStates().begin();
      iter != this->getStates().end();
      ++iter)
    os << iter->toXML() + "\n";

  os << "</trace>";

  return os.str();
}
