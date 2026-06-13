import math

class VM:
    def __init__(self, instructions, env=None):
        self.instructions = instructions
        self.stack = []
        # 允許外部傳入 env，這樣在 REPL 模式下變數可以保留
        self.env = env if env is not None else {} 
        self.pc = 0

    def run(self):
        while True:
            if self.pc >= len(self.instructions):
                break
                
            inst = self.instructions[self.pc]
            op = inst[0]

            if op == 'PUSH':
                self.stack.append(inst[1])
            elif op == 'LOAD':
                if inst[1] not in self.env:
                    raise NameError(f"Undefined variable: {inst[1]}")
                self.stack.append(self.env[inst[1]])
            elif op == 'STORE':
                self.env[inst[1]] = self.stack.pop()
            elif op == 'ADD':
                b, a = self.stack.pop(), self.stack.pop()
                self.stack.append(a + b)
            elif op == 'SUB':
                b, a = self.stack.pop(), self.stack.pop()
                self.stack.append(a - b)
            elif op == 'MUL':
                b, a = self.stack.pop(), self.stack.pop()
                self.stack.append(a * b)
            elif op == 'DIV':
                b, a = self.stack.pop(), self.stack.pop()
                self.stack.append(a / b)
            elif op == 'MOD':
                b, a = self.stack.pop(), self.stack.pop()
                self.stack.append(a % b)
            elif op == 'POW':
                b, a = self.stack.pop(), self.stack.pop()
                self.stack.append(math.pow(a, b))
            elif op == 'CALL':
                func_name = inst[1]
                arg = self.stack.pop()
                if func_name == 'sqrt': self.stack.append(math.sqrt(arg))
                elif func_name == 'sin': self.stack.append(math.sin(arg))
                elif func_name == 'cos': self.stack.append(math.cos(arg))
                elif func_name == 'abs': self.stack.append(abs(arg))
                else: raise NameError(f"Undefined function: {func_name}")
            elif op == 'PRINT_EXPR':
                result = self.stack.pop()
                if result.is_integer():
                    print(f"=> {int(result)}")
                else:
                    print(f"=> {result:.4f}")
            elif op == 'HALT':
                break
            
            self.pc += 1