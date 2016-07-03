//===-- ErrorHandling.cpp -------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/Firehose.h"
#include "klee/Internal/Support/ErrorHandling.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>

#include <set>

#define NELEMS(array) (sizeof(array)/sizeof(array[0]))

using namespace klee;

FILE *klee::klee_warning_file = NULL;
FILE *klee::klee_message_file = NULL;
FILE *klee::klee_firehose_file = NULL;

static const char *warningPrefix = "WARNING";
static const char *warningOncePrefix = "WARNING ONCE";
static const char *errorPrefix = "ERROR";
static const char *notePrefix = "NOTE";

static bool shouldSetColor(const char *pfx, const char *msg,
                           const char *prefixToSearchFor) {
  if (pfx && strcmp(pfx, prefixToSearchFor) == 0)
    return true;

  if (llvm::StringRef(msg).startswith(prefixToSearchFor))
    return true;

  return false;
}

static void klee_vfmessage(FILE *fp, const char *pfx, const char *msg,
                           va_list ap) {
  if (!fp)
    return;

  llvm::raw_fd_ostream fdos(fileno(fp), /*shouldClose=*/false,
                            /*unbuffered=*/true);
  bool modifyConsoleColor = fdos.is_displayed() && (fp == stderr);

  if (modifyConsoleColor) {

    // Warnings
    if (shouldSetColor(pfx, msg, warningPrefix))
      fdos.changeColor(llvm::raw_ostream::MAGENTA,
                       /*bold=*/false,
                       /*bg=*/false);

    // Once warning
    if (shouldSetColor(pfx, msg, warningOncePrefix))
      fdos.changeColor(llvm::raw_ostream::MAGENTA,
                       /*bold=*/true,
                       /*bg=*/false);

    // Errors
    if (shouldSetColor(pfx, msg, errorPrefix))
      fdos.changeColor(llvm::raw_ostream::RED,
                       /*bold=*/true,
                       /*bg=*/false);

    // Notes
    if (shouldSetColor(pfx, msg, notePrefix))
      fdos.changeColor(llvm::raw_ostream::WHITE,
                       /*bold=*/true,
                       /*bg=*/false);
  }

  fdos << "KLEE: ";
  if (pfx)
    fdos << pfx << ": ";

  // FIXME: Can't use fdos here because we need to print
  // a variable number of arguments and do substitution
  vfprintf(fp, msg, ap);
  fflush(fp);

  fdos << "\n";

  if (modifyConsoleColor)
    fdos.resetColor();

  fdos.flush();
}

// Firehose-specific function for determining an id attribute of the
// Firehose failure or info elements
std::string determineFirehoseFailureInfoId(char *whole_msg) {

  static const char *message[] = {
    // infos
    "undefined reference to function",
    "undefined reference to variable",
    "calling external",
    "calling __user_main with extra arguments",
    "Large alloc",
    "execve",
    "executable has module level assembly",
    // failures
    "unable to load symbol",
    "failed external call"
  };

  static const char *id[] = {
    // infos
    "undefined-function-reference",
    "undefined-variable-reference",
    "calling-external",
    "calling-user-main",
    "large-alloc",
    "execve",
    "module-level-assembly",
    // failures
    "symbol-loading",
    "external-call"
  };

  assert(NELEMS(message) == NELEMS(id));

  for (unsigned i = 0; i < NELEMS(message); ++i)
  if (!strncmp(whole_msg, message[i], strlen(message[i])))
    return std::string(id[i]);

  if (strstr(whole_msg, "has inline asm"))
    // info
    return std::string("inline-asm");
  else if (strstr(whole_msg, "silently ignoring"))
    // info
    return std::string("silently-ignoring");
  else if (strstr(whole_msg, "when main() has less than two arguments"))
    // failure
    return std::string("posix-runtime");
  else
    return std::string("other");
}

/* Prints a message/warning.

   If pfx is NULL, this is a regular message, and it's sent to
   klee_message_file (messages.txt).  Otherwise, it is sent to
   klee_warning_file (warnings.txt).
   Finally, if Firehose output is enabled, a warning message is
   sent to klee_firehose_file (firehose.xml).

   Iff onlyToFile is false, the message is also printed on stderr.
*/
static void klee_vmessage(const char *pfx, bool onlyToFile, const char *msg,
                          va_list ap) {
  if (!onlyToFile) {
    va_list ap2;
    va_copy(ap2, ap);
    klee_vfmessage(stderr, pfx, msg, ap2);
    va_end(ap2);
  }

  va_list ap2;
  va_copy(ap2, ap);
  klee_vfmessage(pfx ? klee_warning_file : klee_message_file, pfx, msg, ap2);
  va_end(ap2);

  // Firehose
  if (pfx && klee_firehose_file) {
    char buf[512];
    vsprintf(buf, msg, ap);
    std::string elementId = determineFirehoseFailureInfoId(buf);
    // depending on the content of pfx, we need firehose::Info or
    // firehose::Failure
    if (!strncmp(pfx, "WARNING", strlen("WARNING")) ||
	!strcmp(pfx, notePrefix)) {
      firehose::Info info(elementId, std::string(buf));
      fprintf(klee_firehose_file, "%s\n", info.toXML().c_str());
    }
    else if(!strcmp(pfx, errorPrefix)) {
      firehose::Failure failure(elementId, std::string(buf));
      fprintf(klee_firehose_file, "%s\n", failure.toXML().c_str());
    }
    fflush(klee_firehose_file);
  }
}

void klee::klee_message(const char *msg, ...) {
  va_list ap;
  va_start(ap, msg);
  klee_vmessage(NULL, false, msg, ap);
  va_end(ap);
}

/* Message to be written only to file */
void klee::klee_message_to_file(const char *msg, ...) {
  va_list ap;
  va_start(ap, msg);
  klee_vmessage(NULL, true, msg, ap);
  va_end(ap);
}

static void close_firehose_file() {
  if (klee_firehose_file) {
    // write closing tags and close the file
    fprintf(klee_firehose_file, "</results>\n");
    fprintf(klee_firehose_file, "</analysis>\n");
    fclose(klee_firehose_file);
  }
}

void klee::klee_error(const char *msg, ...) {
  va_list ap;
  va_start(ap, msg);
  klee_vmessage(errorPrefix, false, msg, ap);
  va_end(ap);
  close_firehose_file();
  exit(1);
}

void klee::klee_warning(const char *msg, ...) {
  va_list ap;
  va_start(ap, msg);
  klee_vmessage(warningPrefix, false, msg, ap);
  va_end(ap);
}

/* Prints a warning once per message. */
void klee::klee_warning_once(const void *id, const char *msg, ...) {
  static std::set<std::pair<const void *, const char *> > keys;
  std::pair<const void *, const char *> key;

  /* "calling external" messages contain the actual arguments with
     which we called the external function, so we need to ignore them
     when computing the key. */
  if (strncmp(msg, "calling external", strlen("calling external")) != 0)
    key = std::make_pair(id, msg);
  else
    key = std::make_pair(id, "calling external");

  if (!keys.count(key)) {
    keys.insert(key);
    va_list ap;
    va_start(ap, msg);
    klee_vmessage(warningOncePrefix, false, msg, ap);
    va_end(ap);
  }
}
