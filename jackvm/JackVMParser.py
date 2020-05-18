from enum import Enum
from JackVMConstants import *

class JackVMParser():
    def __init__(self, code):
        self.code = code

        self.__arithmetics = ["add", "sub", "neg", "eq", "gt", "lt", "and", "or", "not"]
        self.__pushpop = ["push", "pop"]
        self.__segments = ["local", "argument", "this", "that",
                            "constant", "static", "temp", "pointer"]
        
        self.__preprocess()
        self.__commands = self.code.split("\n")
        self.curCmd = ""
        self.arg1 = ""
        self.arg2 = ""
    def __preprocess(self):
        # remove whitespaces, comments
        # put all elements in an array of str
        while self.code.find("\n\n") != -1:
            self.code = self.code.replace("\n\n", "\n")
        while self.code.find("//") != -1:
            idx = self.code.find("//")
            if self.code[idx:].find("\n") != -1:
                # if there is a new line after comment
                nextLineBreak = idx + self.code[idx:].find("\n")
                self.code = self.code[:idx] + self.code[nextLineBreak:]
            else:
                # if there is no new line after comment
                self.code = self.code[:idx]
        while self.code.find(" \n") != -1: # remove trailing whitespaces
            self.code = self.code.replace(" \n", "\n")
        while self.code.find("\n ") != -1: # remove front whitespaces
            self.code = self.code.replace("\n ", "\n")
        while self.code.find("  ") != -1: # remove repeating whitespaces
            self.code = self.code.replace("  ", " ")

    def hasMoreCommands(self):
        return len(self.__commands) > 0
    
    def advance(self):
        cmdStr = self.__commands.pop(0)
        curCmd = cmdStr.split(" ")
        self.curCmd = curCmd[0].lower()

        cmdType = self.commandType(self.curCmd)

        if cmdType == CMDTYPE.C_ARITHMETIC:
            self._arg1 = CMDTYPE.C_ARITHMETIC
        elif cmdType == CMDTYPE.C_RETURN:
            self._arg1 = None
        else:
            self._arg1 = curCmd[1].lower()

        if cmdType == CMDTYPE.C_PUSH or \
           cmdType == CMDTYPE.C_POP or \
           cmdType == CMDTYPE.C_FUNCTION or \
           cmdType == CMDTYPE.C_CALL:
           self.arg2 = curCmd[2].lower()
        else:
            self.arg2 = None
    
    def commandType(self, cmd=None):
        if cmd is None:
            cmd = self.curCmd
        if self.curCmd in self.__arithmetics:
            return CMDTYPE.C_ARITHMETIC
        elif self.curCmd in self.__pushpop:
            if self.curCmd == "push":
                return CMDTYPE.C_PUSH
            else:
                return CMDTYPE.C_POP
    