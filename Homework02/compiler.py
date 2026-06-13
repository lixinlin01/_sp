class Compiler:
    def __init__(self, tokens):
        self.tokens = tokens
        self.pos = 0
        self.instructions = []

    def match(self, expected_kind):
        if self.tokens[self.pos][0] == expected_kind:
            val = self.tokens[self.pos][1]
            self.pos += 1
            return val
        raise SyntaxError(f"Expected {expected_kind}, got {self.tokens[self.pos][0]}")

    def peek(self):
        return self.tokens[self.pos][0]

    def compile(self):
        while self.peek() != 'EOF':
            self.statement()
        self.instructions.append(('HALT',))
        return self.instructions

    def statement(self):
        if self.peek() == 'LET':
            self.match('LET')
            name = self.match('IDENT')
            self.match('ASSIGN')
            self.expr()
            self.match('SEMI')
            self.instructions.append(('STORE', name))
        else:
            self.expr()
            self.match('SEMI')
            self.instructions.append(('PRINT_EXPR',))

    def expr(self):
        self.term()
        while self.peek() in ('PLUS', 'MINUS'):
            op = self.match(self.peek())
            self.term()
            if op == '+': self.instructions.append(('ADD',))
            elif op == '-': self.instructions.append(('SUB',))

    def term(self):
        self.factor()
        while self.peek() in ('MUL', 'DIV', 'MOD'):
            op = self.match(self.peek())
            self.factor()
            if op == '*': self.instructions.append(('MUL',))
            elif op == '/': self.instructions.append(('DIV',))
            elif op == '%': self.instructions.append(('MOD',))

    def factor(self):
        self.power()
        while self.peek() == 'POW':
            self.match('POW')
            self.power()
            self.instructions.append(('POW',))

    def power(self):
        if self.peek() == 'FLOAT':
            val = float(self.match('FLOAT'))
            self.instructions.append(('PUSH', val))
        elif self.peek() == 'NUMBER':
            val = float(self.match('NUMBER'))
            self.instructions.append(('PUSH', val))
        elif self.peek() == 'IDENT':
            name = self.match('IDENT')
            if self.peek() == 'LPAREN':
                self.match('LPAREN')
                self.expr()
                self.match('RPAREN')
                self.instructions.append(('CALL', name))
            else:
                self.instructions.append(('LOAD', name))
        elif self.peek() == 'LPAREN':
            self.match('LPAREN')
            self.expr()
            self.match('RPAREN')