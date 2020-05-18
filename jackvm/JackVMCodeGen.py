class JackVMCodeGen():
    def __init__(self):
        pass

    def writeArithmetic(self, cmd):
        # get two numbers from sp
        # sp -= 1
        # *sp = result
        calc = ""
        if cmd == "add":
            calc = "D=D+M"
        elif cmd == "sub":
            calc = "D=D-M"
        elif cmd == "neg":
            calc = "D=!D"
            pass
        elif cmd == "eq":
            pass
        elif cmd == "gt":
            # if D > M:
            #   res = -1
            # else:
            #   res = 0

            # if D > M => M-D < 0, msb = 1
            # (M-D)&0b1
            pass
        elif cmd == "lt":
            # if D < M:
            #   res = -1
            # else:
            #   res = 0
            pass
        elif cmd == "and":
            calc = "D=D&M"
            pass
        elif cmd == "or":
            calc = "D=D|M"
            pass
        elif cmd == "not":
            calc = "D=!D"
            pass
        
        # template for 1-argument cmd
        # D=arg
        template1 = """
        @SP
        D=M
        """

        # template for 2-arguments cmd
        # D=1st, M=2nd
        # push 1
        # push 2
        # => D=1, M=2
        template2 = """
        @SP
        A=A-1
        D=M
        A=A+1
        """
        pass

    def writePushPop(self, cmd, segment, idx):
        pass

    def Close(self):
        pass