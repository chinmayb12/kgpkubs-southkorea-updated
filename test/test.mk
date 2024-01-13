##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=test
ConfigurationName      :=Debug
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
WorkspacePath          := "/home/robocup/kgpkubs-southkorea"
ProjectPath            := "/home/robocup/kgpkubs-southkorea/test"
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=robocup
Date                   :=Tuesday 28 July 2015
CodeLitePath           :="/home/robocup/.codelite"
LinkerName             :=g++
ArchiveTool            :=ar rcus
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-gstab
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
CompilerName           :=g++
C_CompilerName         :=gcc
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
MakeDirCommand         :=mkdir -p
CmpOptions             := -g $(Preprocessors)
LinkOptions            :=  
IncludePath            :=  "$(IncludeSwitch)." "$(IncludeSwitch)." 
RcIncludePath          :=
Libs                   :=
LibPath                := "$(LibraryPathSwitch)." 


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects=$(IntermediateDirectory)/main$(ObjectSuffix) $(IntermediateDirectory)/serial$(ObjectSuffix) 

##
## Main Build Targets 
##
all: $(OutputFile)

$(OutputFile): makeDirStep $(Objects)
	@$(MakeDirCommand) $(@D)
	$(LinkerName) $(OutputSwitch)$(OutputFile) $(Objects) $(LibPath) $(Libs) $(LinkOptions)

makeDirStep:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main$(DependSuffix)
	$(CompilerName) $(SourceSwitch) "/home/robocup/kgpkubs-southkorea/test/main.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/main$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main$(DependSuffix): main.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/main$(ObjectSuffix) -MF$(IntermediateDirectory)/main$(DependSuffix) -MM "/home/robocup/kgpkubs-southkorea/test/main.cpp"

$(IntermediateDirectory)/main$(PreprocessSuffix): main.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main$(PreprocessSuffix) "/home/robocup/kgpkubs-southkorea/test/main.cpp"

$(IntermediateDirectory)/serial$(ObjectSuffix): serial.cpp $(IntermediateDirectory)/serial$(DependSuffix)
	$(CompilerName) $(SourceSwitch) "/home/robocup/kgpkubs-southkorea/test/serial.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/serial$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/serial$(DependSuffix): serial.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/serial$(ObjectSuffix) -MF$(IntermediateDirectory)/serial$(DependSuffix) -MM "/home/robocup/kgpkubs-southkorea/test/serial.cpp"

$(IntermediateDirectory)/serial$(PreprocessSuffix): serial.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/serial$(PreprocessSuffix) "/home/robocup/kgpkubs-southkorea/test/serial.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/main$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/main$(DependSuffix)
	$(RM) $(IntermediateDirectory)/main$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/serial$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/serial$(DependSuffix)
	$(RM) $(IntermediateDirectory)/serial$(PreprocessSuffix)
	$(RM) $(OutputFile)


