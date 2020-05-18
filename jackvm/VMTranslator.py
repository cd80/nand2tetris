import sys
from JackVMParser import JackVMParser
from JackVMCodeGen import JackVMCodeGen
from JackVMConstants import *
class VMTranslator():
    def __init__(self):
        self.__vmFileContents = ""
        self.__parser = None
        self.__codegen = None
        pass

    def loadFile(self, filePath):
        if not filePath.endswith(".vm"):
            raise Exception
        with open(filePath, "rb") as f:
            self.__vmFileContents = f.read()

    def doParsing(self):
        self.__parser = JackVMParser(self.__vmFileContents)
        if self.__parser.hasMoreCommands():
            self.__parser.advance()
            return True
        else:
            return False

    def doCodeGen(self):
        self.__codegen = JackVMCodeGen()
        cmdType = self.__parser.commandType()
        if cmdType == CMDTYPE.C_ARITHMETIC:
            self.__codegen.writeArithmetic(self.__parser.curCmd)
        else:
            self.__codegen.writePushPop(self.__parser.curCmd, self.__parser.arg1, self.__parser.arg2)
    
    def cleanUp(self):
        self.__codegen.Close()

if __name__ == '__main__':
    vm = VMTranslator()
    if not len(sys.argv) == 2:
        print("Usage: VMTranslator.py [.vm file]")
        sys.exit(0)
    vm.loadFile(sys.argv[1])
    while vm.doParsing():
        vm.doCodeGen()
    vm.cleanUp()