#include "Tokenizer.hpp"

#include "CharacterAttributes.hpp"

// Automata state: START
Tokenizer::Tokenizer(
    Source& source,
    SymbolStore& symbols,
    CharacterAttributes& attributes,
    Logger<LexicalError>& logger
) :
    _source(source),
    _symbols(symbols),
    _attributes(attributes),
    _logger(logger),
    _character(source.current()) {}

void Tokenizer::scan() {
    // Automata state: LOOP
    while (!_source.done()) {
        switch (_attributes.lookup(_character)) {
            // Automata state: WHITESPACE
            case Attribute::Whitespace:
                scanWhitespaces();
                break;

            // Automata state: INTEGER
            case Attribute::Digit:
                scanInteger();
                break;

            // Automata state: STRING
            case Attribute::Letter:
                scanString();
                break;

            // Automata state: MULTI_DELIMITER
            case Attribute::MultiDelimiter:
                scanMultiDelimiter();
                break;

            // Automata state: COMMENT_START
            case Attribute::Comment:
                scanComment();
                break;

            // Automata state: DELIMITER
            case Attribute::Delimiter:
                scanDelimiter();
                break;

            // Automata state: ERROR
            case Attribute::Invalid:
                scanInvalid();
                break;
        }
    }
    // Automata state: END
}

void Tokenizer::scanWhitespaces() {
    while (!_source.done() &&
           _attributes.lookup(_character) == Attribute::Whitespace) {
        _character = _source.read();
    }
}

void Tokenizer::scanInteger() {
    while (!_source.done() && _attributes.lookup(_character) == Attribute::Digit) {
        _token += _character;
        _character = _source.read();
    };

    // Automata state: WRITE
    _code = _symbols.resolveLiteral(_token);

    addToken({
        .code = _code,
        .row = _source.cursor().row(),
        .column = _source.cursor().column() -
                  _token.length(),  // Point to first character of the token
    });

    _token.clear();
}

void Tokenizer::scanString() {
    while (!_source.done() &&
           (_attributes.lookup(_character) == Attribute::Letter ||
            _attributes.lookup(_character) == Attribute::Digit)) {
        _token += _character;
        _character = _source.read();
    }

    // Automata state: WRITE
    if (_symbols.isKeyword(_token)) {
        _code = _symbols.resolveKeyword(_token);
    } else {
        _code = _symbols.resolveIdentifier(_token);
    }

    addToken({
        .code = _code,
        .row = _source.cursor().row(),
        .column = _source.cursor().column() -
                  _token.length(),  // Point to first character of the token
    });

    _token.clear();
}

void Tokenizer::scanMultiDelimiter() {
    size_t startRow = _source.cursor().row();
    size_t startCol = _source.cursor().column();

    _token += _character;
    _character = _source.read();

    if (_source.done() || _attributes.lookup(_character) != Attribute::Letter) {
        addError({
            .message = LexicalError::InvalidMultiDelimiter(_token),
            .row = startRow,
            .column = startCol,
        });

        _token.clear();

        return;
    }

    while (!_source.done() &&
           (_attributes.lookup(_character) == Attribute::Letter)) {
        _token += _character;
        _character = _source.read();
    }

    if (!_symbols.isMultiDelimiter(_token)) {
        addError({
            .message = LexicalError::InvalidMultiDelimiter(_token),
            .row = startRow,
            .column = startCol,
        });

        _token.clear();
        return;
    }

    _code = _symbols.resolveMultiDelimiter(_token);

    // Automata state: WRITE
    addToken({
        .code = _code,
        .row = startRow,
        .column = startCol,
    });

    _token.clear();
}

void Tokenizer::scanComment() {
    size_t startRow = _source.cursor().row();
    size_t startCol = _source.cursor().column();

    _character = _source.read();

    // Automata state: WRITE
    if (_character != '*') {
        addToken({
            .code = static_cast<size_t>('('),
            .row = startRow,
            .column = startCol,
        });

        return;
    }

    _character = _source.read();

    // Automata state: COMMENT
    while (!_source.done()) {
        // Automata state: COMMENT_CLOSE
        if (_character == '*') {
            _character = _source.read();

            // Automata state: COMMENT_END
            if (_character == ')') {
                _character = _source.read();

                return;
            }
        } else {
            _character = _source.read();
        }
    }

    // Automata state: ERROR
    addError({
        .message = LexicalError::UnclosedComment(),
        .row = startRow,
        .column = startCol,
    });
}

void Tokenizer::scanDelimiter() {
    addToken({
        .code = static_cast<size_t>(_character),
        .row = _source.cursor().row(),
        .column = _source.cursor().column(),
    });

    _character = _source.read();
}

void Tokenizer::scanInvalid() {
    addError({
        .message = LexicalError::InvalidCharacter(_character),
        .row = _source.cursor().row(),
        .column = _source.cursor().column(),
    });

    _character = _source.read();
}

const std::vector<Token>& Tokenizer::tokens() const {
    return _tokens;
}

void Tokenizer::addToken(const Token& token) {
    _tokens.push_back(token);
}

void Tokenizer::addError(const LexicalError& error) {
    _logger.message(error);
}