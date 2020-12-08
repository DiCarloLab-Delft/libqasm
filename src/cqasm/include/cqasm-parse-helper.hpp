/** \file
 * Contains helper classes and objects for the lexer and parser generated by
 * flex/bison, as well as the entry points for invoking the parser directly, in
 * case you don't need semantic analysis.
 */

#pragma once

#include "cqasm-ast.hpp"
#include <cstdio>

namespace cqasm {

/**
 * Namespace for the parser functions and classes.
 */
namespace parser {

/**
 * Parse result information.
 */
class ParseResult {
public:

    /**
     * Root node of the AST, if analysis was sufficiently successful. This may
     * be set even if parsing was not ENTIRELY successful, in which case it
     * will contain one or more error nodes.
     */
    ast::One<ast::Root> root;

    /**
     * List of accumulated errors. Analysis was successful if and only if
     * `errors.empty()`.
     */
    std::vector<std::string> errors;

};

/**
 * Parse the given file.
 */
ParseResult parse_file(const std::string &filename);

/**
 * Parse using the given file pointer.
 */
ParseResult parse_file(FILE *file, const std::string &filename = "<unknown>");

/**
 * Parse the given string. A filename may be given in addition for use within
 * error messages.
 */
ParseResult parse_string(const std::string &data, const std::string &filename="<unknown>");

/**
 * Internal helper class for parsing cQASM files.
 */
class ParseHelper {
public:

    /**
     * File pointer being scanned, if no data was specified.
     */
    FILE *fptr = nullptr;

    /**
     * Flex data buffer, if data was specified.
     */
    void *buf = nullptr;

    /**
     * Flex reentrant scanner data.
     */
    void *scanner = nullptr;

    /**
     * Name of the file being parsed.
     */
    std::string filename;

    /**
     * The parse result.
     */
    ParseResult result;

private:
    friend ParseResult parse_file(const std::string &filename);
    friend ParseResult parse_file(FILE *file, const std::string &filename);
    friend ParseResult parse_string(const std::string &data, const std::string &filename);

    /**
     * Parse a string or file with flex/bison. If use_file is set, the file
     * specified by filename is read and data is ignored. Otherwise, filename
     * is used only for error messages, and data is read instead. Don't use
     * this directly, use parse().
     */
    ParseHelper(const std::string &filename, const std::string &data, bool use_file);

    /**
     * Construct the analyzer internals for the given filename, and analyze
     * the file.
     */
    ParseHelper(const std::string &filename, FILE *fptr);

    /**
     * Initializes the scanner. Returns whether this was successful.
     */
    bool construct();

    /**
     * Does the actual parsing.
     */
    void parse();

public:

    /**
     * Destroys the parse helper.
     */
    virtual ~ParseHelper();

    /**
     * Pushes an error.
     */
    void push_error(const std::string &error);

};

/**
 * Source location annotation object, containing source file line numbers etc.
 */
class SourceLocation {
public:

    /**
     * The name of the source file.
     */
    std::string filename;

    /**
     * The first line of the range, or 0 if unknown.
     */
    uint32_t first_line;

    /**
     * The first column of the range, or 0 if unknown.
     */
    uint32_t first_column;

    /**
     * The last line of the range, or 0 if unknown.
     */
    uint32_t last_line;

    /**
     * The last column of the range, or 0 if unknown.
     */
    uint32_t last_column;

    /**
     * Constructs a source location object.
     */
    SourceLocation(
        const std::string &filename,
        uint32_t first_line = 0,
        uint32_t first_column = 0,
        uint32_t last_line = 0,
        uint32_t last_column = 0
    );

    /**
     * Expands the location range to contain the given location in the source
     * file.
     */
    void expand_to_include(uint32_t line, uint32_t column = 1);

};

/**
 * Stream << overload for source location objects.
 */
std::ostream &operator<<(std::ostream &os, const SourceLocation &object);

} // namespace parser
} // namespace cqasm
