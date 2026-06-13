import re

class Lexer:
    def __init__(self, code):
        self.code = code
        self.tokens = []
        self.tokenize()

    def tokenize(self):
        specs = [
            ('WHITESPACE', r'[ \t\n]+'),
            ('LET',        r'\blet\b'),
            ('FLOAT',      r'\d+\.\d+'),
            ('NUMBER',     r'\d+'),
            ('IDENT',      r'[a-zA-Z_][a-zA-Z0-9_]*'),
            ('ASSIGN',     r'='),
            ('PLUS',       r'\+'),
            ('MINUS',      r'-'),
            ('MUL',        r'\*'),
            ('DIV',        r'/'),
            ('MOD',        r'%'),
            ('POW',        r'\^'),
            ('LPAREN',     r'\('),
            ('RPAREN',     r'\)'),
            ('SEMI',       r';'),
        ]
        tok_regex = '|'.join('(?P<%s>%s)' % pair for pair in specs)
        for mo in re.finditer(tok_regex, self.code):
            kind = mo.lastgroup
            value = mo.group()
            if kind != 'WHITESPACE':
                self.tokens.append((kind, value))
        self.tokens.append(('EOF', ''))