import sys
from lexer import Lexer
from compiler import Compiler
from vm import VM

def run_code(code, env, debug=False):
    try:
        lexer = Lexer(code)
        compiler = Compiler(lexer.tokens)
        bytecode = compiler.compile()
        
        if debug:
            print("--- Bytecode ---")
            for i, inst in enumerate(bytecode):
                print(f"{i:02}: {inst}")
            print("----------------")
            
        vm = VM(bytecode, env)
        vm.run()
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    # 共用的環境變數表，讓 REPL 可以記住你之前宣告的變數
    global_env = {}

    if len(sys.argv) > 1:
        # 模式 1：讀取檔案執行
        filename = sys.argv[1]
        try:
            with open(filename, 'r', encoding='utf-8') as f:
                code = f.read()
            run_code(code, global_env)
        except FileNotFoundError:
            print(f"找不到檔案: {filename}")
    else:
        # 模式 2：互動模式 (REPL)
        print("NumScript 互動模式 - 輸入 'exit' 離開")
        while True:
            try:
                line = input(">> ").strip()
                if not line:
                    continue
                if line.lower() == 'exit':
                    break
                
                # 自動補上分號，讓使用者在互動模式下少打一個字
                if not line.endswith(';'):
                    line += ';'
                    
                run_code(line, global_env)
            except KeyboardInterrupt:
                break
            except EOFError:
                break